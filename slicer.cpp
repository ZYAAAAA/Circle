﻿#include "slicer.h"

#include <cmath>
#include <cfloat>
#include <QDebug>
#include <QFuture>
#include <QPainter>
#include <algorithm>
#include <QImageWriter>
#include <QFutureWatcher>
#include<QtConcurrent/QtConcurrent>
#include "sliceable.h"
#include "qmath.h"

Slicer::Slicer()
{
    setTerminationEnabled(true);
    resolution.setWidth(1920);
    resolution.setHeight(1080);
    thickness = 2.0f;
}

Slicer::~Slicer()
{
    requestInterruption();
    quit();
    wait();
}
QVector<QByteArray> Slicer::resultbyte()
{
    wait();
    return images;
}
QVector<QVector<QVector2D>> Slicer::resultspot()
{
    wait();
    QVector<QVector<QVector2D>> tttt;
    for(int i = 0; i < spot.size(); i++)
    {
        QVector<QVector2D> xxxxx;
        QByteArray testArraySe = spot.at(i);
        QBuffer buffer(&testArraySe);
        if( buffer.open(QIODevice::ReadOnly) )
        {
            float x, y;
            QDataStream in(&buffer);
            while(!in.atEnd())
            {
                in >> x >> y;
                xxxxx.append(QVector2D(x, y));
            }
        }
        buffer.close();
        tttt.append(xxxxx);
    }
    return tttt;
}
void Slicer::fillnormalandvertex()
{
    for(int i = 0; i < items.size(); i++)
    {
        Sliceable* item = items.at(i);
        item->dump(normal, vertex, this->f);
    }
    normal.detach();
    vertex.detach();
}

void Slicer::information(float f, float thickness, QSize resolution)
{
    this->thickness = thickness;
    this->resolution = resolution;
    this->f = f;
}

void Slicer::run()
{
    qDebug() << "Slicer Start";
    max_layer = 0;
    if (isInterruptionRequested())
    {
        cleanAll();
        return;
    }
    //创建queue
    groupMLayers(items.at(0));
    if (isInterruptionRequested())
    {
        cleanAll();
        return;
    }
    //切片得到queue中的data
    dispatchMLayers();
    if (isInterruptionRequested())
    {
        cleanAll();
        return;
    }
    //将queue中的切片结果data都给images
    //先过滤掉负层
    int startindex = items.at(0)->first_layer < 0 ? qAbs(items.at(0)->first_layer) : 0;
    //找到最后一层有数据的
    int lastIndex = queue.size() - 1;
    while (queue[lastIndex].isEmpty)
    {
        lastIndex --;
    }
    images.resize(lastIndex + 1 - startindex);
    spot.resize(lastIndex + 1 - startindex);
    for (int i = 0; i < lastIndex + 1 - startindex; i++)
    {
        images[i] = queue[i + startindex].data;
        spot[i] = queue[i + startindex].spotdata;
    }
    clean();
    if (isInterruptionRequested())
    {
        cleanAll();
        return;
    }
    cleanAll();
}

void Slicer::clean()
{
    queue.clear();
    normal.clear();
    vertex.clear();
}

void Slicer::cleanAll()
{
    clean();
    for (int i = 0; i < items.size(); i++)
    {
        items[i]->first_layer = 0;
        items[i]->last_layer = 0;
    }
}

void Slicer::groupMLayers(Sliceable* item)
{
    struct T
    {
        int n;
        float z;
    };
    QVector<struct T> tmax, tmin;
    tmax.resize(normal.size());
    tmin.resize(normal.size());
    //求出每个三角形的最大z和最小z，n为vertex的面片编号
    for (int i = 0; i < normal.size(); i++)
    {
        const QVector3D& v1 = vertex.at(i * 3 + 0);
        const QVector3D& v2 = vertex.at(i * 3 + 1);
        const QVector3D& v3 = vertex.at(i * 3 + 2);
        tmax[i].z = qMax(qMax(v1.z(), v2.z()), v3.z());
        tmin[i].z = qMin(qMin(v1.z(), v2.z()), v3.z());
        tmax[i].n = tmin[i].n = i;
    }
    auto comp = [](const T & t1, const T & t2)
    {
        return t1.z < t2.z;
    };
    //将tmin根据z从小到大排序
    std::sort(tmin.begin(), tmin.end(), comp);
    float zmin = tmin.isEmpty() ? 0 : tmin.first().z + thickness / 2;
    float zmax = tmax.isEmpty() ? 0 : std::max_element(tmax.begin(), tmax.end(), comp)->z;
    int layers = int(qMax(0.0f, zmax - zmin) / thickness) + 1;
    item->first_layer = qFloor(qreal(zmin / thickness));
    item->last_layer = item->first_layer + layers - 1;
    max_layer = qMax(max_layer, item->last_layer);
    queue.resize(layers);
    for (int i = 0; i < queue.size(); i++)
    {
        queue[i].z = zmin + thickness * i;
    }
    int hi, lo = 0;
    for (int i = 0; i < tmin.size(); i++)
    {
        for (; lo < queue.size() && tmin[i].z > queue[lo].z; lo++)
        {
        }
        for (hi = lo; hi < queue.size() && tmax[tmin[i].n].z >= queue[hi].z; hi++)
        {
            queue[hi].index.append(tmin[i].n);
        }
    }
}

void Slicer::dispatchMLayers()
{
    auto func = [&](struct MLayer & layer)
    {
        processMLayer(layer);
    };
    QFuture<void> future = QtConcurrent::map(queue, func);
    QFutureSynchronizer<void> synchronizer;
    synchronizer.setCancelOnWait(true);
    synchronizer.addFuture(future);
    QFutureWatcher<void> watcher;
    watcher.setFuture(future);
    QEventLoop eventloop;
    QTimer timer;
    connect(&watcher, &QFutureWatcher<void>::finished, &eventloop, &QEventLoop::quit);
    connect(&timer, &QTimer::timeout, [&]()
    {
        if (isInterruptionRequested())
        {
            watcher.cancel();
        }
        if (watcher.isFinished())
        {
            eventloop.quit();
        }
    });
    timer.start(200);
    eventloop.exec();
    future.waitForFinished();
    qDebug() << "processMLayer";
}


void Slicer::processMLayer(struct MLayer& layer)
{
    //层厚LAYER.Z高度与三角形边去求交点,交到两个点即为轮廓线上的小线段
    QVector<QVector2D> spots;
    QVector<Line> lines;
    for (int n : layer.index)
    {
        //v1,v2,v3三角形面片的三个顶点
        const QVector3D& v1 = vertex[n * 3 + 0];
        const QVector3D& v2 = vertex[n * 3 + 1];
        const QVector3D& v3 = vertex[n * 3 + 2];
        //与三角形边的交点
        QVector<QVector2D> intersections;
        if ((v1.z() < layer.z) != (v2.z() < layer.z))
        {
            intersections.append(intersect(v1, v2, layer.z));
        }
        if ((v1.z() < layer.z) != (v3.z() < layer.z))
        {
            intersections.append(intersect(v1, v3, layer.z));
        }
        if ((v2.z() < layer.z) != (v3.z() < layer.z))
        {
            intersections.append(intersect(v2, v3, layer.z));
        }
        if (intersections.size() != 2)//交点不是2个不符合
        {
            continue;
        }
        //交点s1,s2,组成一条line添加入lines
        QVector3D s1(intersections[0]), s2(intersections[1]);//交点s1和s2
        if (QVector3D::crossProduct(s2 - s1, normal[n]).z() > 0)
        {
            lines.append(Line{intersections[0], intersections[1], false});
            spots.append(intersections[0]);
            spots.append(intersections[1]);
        }
        else
        {
            lines.append(Line{intersections[1], intersections[0], false});
            spots.append(intersections[1]);
            spots.append(intersections[0]);
        }
    }
    unsigned int xres = resolution.width();
    unsigned int yres = resolution.height();
    QImage img = QImage(resolution, QImage::Format_ARGB32);
    img.fill(Qt::black);
    QPainter painter(&img);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(QPen(Qt::white, 2));
    //画出所有的线
    painter.translate(xres / 2, yres / 2);
    for(int i = 0; i < lines.size(); i++)
    {
        Line a =  lines.at(i);
        painter.drawLine(QLine(a.p1.x(), a.p1.y(), a.p2.x(), a.p2.y()));
    }
    painter.end();
    img = img.mirrored(true, false);
    //将spots数据写入spotdata
    for(int i = 0; i < spots.size(); i++)
    {
        spots[i].setX(spots.at(i).x() + xres / 2);
        spots[i].setY(spots.at(i).y() + yres / 2);
        spots[i].setX(xres - spots.at(i).x());
        //painter.drawPoint(spots[i].x(),spots[i].y());
    }
    QBuffer bufferspot(&layer.spotdata);
    QDataStream out(&bufferspot);
    bufferspot.open(QIODevice::WriteOnly);
    for(int i = 0; i < spots.size(); i++)
    {
        out << spots.at(i);
    }
    bufferspot.close();
    QTime time;
    time = QTime::currentTime();
    QString strTime;
    strTime = QCoreApplication::applicationDirPath() + "/out/" + time.toString("hh_mm_ss_") + QString::number((int)QThread::currentThreadId()) + ".png";
    img.save(strTime);
    //将image数据写入slice.data
    QBuffer buffer(&layer.data);
    QImageWriter writer(&buffer, "png");
    writer.setCompression(100);
    writer.write(img);
}


QVector2D Slicer::intersect(QVector3D v1, QVector3D v2, float z)
{
    float m1 = (v2.z() - z) / (v2.z() - v1.z()), m2 = (z - v1.z()) / (v2.z() - v1.z());
    return m1 * v1.toVector2D() + m2 * v2.toVector2D();
}
void Slicer::SubtractVoidFromFill(QImage* img, Pixinfo** pixinfo)
{
    unsigned int xres = resolution.width();
    unsigned int yres = resolution.height();
    QRgb pickedcolor;
    int red;
    int green;
    int result;
    unsigned int x, y;
    for(x = 0; x < xres; x++)
    {
        for(y = 0; y < yres; y++)
        {
            red = 0;
            green = 0;
            for(int i = 0; i < pixinfo[x][y].result.size(); i++)
            {
                pickedcolor = pixinfo[x][y].result.at(i);
                red += qRed(pickedcolor);
                green += qGreen(pickedcolor);
            }
            if(red - green > 0)
            {
                result = red - green;
                if(result > 255)
                {
                    result = 255;
                }
                if (!isBlackWhite)
                {
                    img->setPixel(x, y, QColor(result, result, result).rgb());
                }
                else
                {
                    if (result < threshold)
                    {
                        img->setPixel(x, y, QColor(0, 0, 0).rgb());
                    }
                    else
                    {
                        img->setPixel(x, y, QColor(255, 255, 255).rgb());
                    }
                }
            }
        }
    }
}
