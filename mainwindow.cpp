#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPixmap>
#include <qmath.h>
#include <iostream>
#include <QPainter>
#include <opencv2/imgproc.hpp>
#include <QFileDialog>
#include <QInputDialog>
#include <QtMath>
#include <QDateTime>
#include "edlib.h"
using namespace cv;
using namespace std;

//内部比率阈值'T_inlier'，越大越严格。因此，要获得更多的圆圈，您可以稍微调低它。
//锐角阈值“sharp_angle”。要检测小圆圈，您可以稍微调整一下
typedef struct threshold
{
    int T_l = 20;
    float T_ratio = 0.001;
    int T_o = 5;// 5 10 15 20 25
    int T_r = 5;// 5 10 15 20 25
    float T_inlier = 0.5;//0.3 0.35 0.4 0.45 0.5 (the larger the more strict)
    float T_angle = 2.0;//
    float T_inlier_closed = 0.5;//0.5,0.6 0.7 0.8,0.9
    float sharp_angle = 35;//35 40 45 50 55 60

} T;

struct Queue_Circle
{
    std::vector<MyCircles> cirdata;
    Mat imadata;
    bool isSaved = false;
    int id = -1;
} QC;
MainWindow::MainWindow(QWidget* parent):
    QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //.exe
    paths = QCoreApplication::applicationDirPath();
    Initialization();
    utils::logging::setLogLevel(utils::logging::LOG_LEVEL_ERROR);
}


MainWindow::~MainWindow()
{
    delete myopenglwidget;
    delete ui;
}
void MainWindow::Initialization()
{
    ui->horizontalSlider_ImageIndex->setEnabled(false);
    ui->horizontalSlider_ImageIndex->setRange(0, 0);
    ui->DefineButton->setEnabled(false);
    ui->CalButton->setEnabled(false);
    ui->SaveButton->setEnabled(false);
    ui->ExportButton->setEnabled(false);
    ui->Calgray->setEnabled(false);
    ui->pushButton->setEnabled(false);
    ui->pushButton_2->setEnabled(false);
    ui->pushButton_3->setEnabled(false);
    QPixmap clear = QPixmap();
    ui->label_2->setPixmap(clear);
    ui->mlabel_1->setPixmap(clear);
    ui->mlabel_2->setPixmap(clear);
    ui->mlabel_3->setPixmap(clear);
    ui->mlabel_4->setPixmap(clear);
    ui->mlabel_5->setPixmap(clear);
    ui->textEdit->setText(QString::number(Target_radius * f));
    if(ui->stackedWidget_5->count() == 2)
    {
        ui->stackedWidget_5->removeWidget(ui->stackedWidget_5->widget(1));
    }
    ui->stackedWidget_5->setCurrentIndex(0);
}

void MainWindow::Cleandata()
{
    QC.cirdata.clear();
    imagesbyte.clear();
    spotbyte.clear();
    queueimages.clear();
    queueimages.resize(5);
    queueindex = 0;
    NowImages = QImage();
}
void MainWindow::on_ImportButton_clicked()
{
    Initialization();
    if(myopenglwidget)
    {
        delete myopenglwidget;
        myopenglwidget = nullptr;
    }
    QString path = QFileDialog::getOpenFileName(this, tr("导入stl文件"), paths, "stl(*.stl)");
    myopenglwidget = new MyGLWidget(this, path);
    ui->stackedWidget_5->addWidget(myopenglwidget);
    ui->stackedWidget_5->setCurrentIndex(1);
    if(myopenglwidget->model_temp)
    {
        qDebug() << "Import OK" ;
    }
    ui->pushButton->setEnabled(true);
    ui->pushButton_2->setEnabled(true);
    ui->pushButton_3->setEnabled(true);
}

void MainWindow::Show_Image()
{
    Slicer* slicer = new Slicer();
    d = new Dia(this);
    if(d->exec() == QDialog::Accepted)
    {
        f = d->f;
        thinkess = d->mlayer;
        Target_radius = d->radius;
        resolution = QSize(d->x, d->y);
        ui->label_8->setText(QString::number(d->x));
        ui->label_9->setText(QString::number(d->y));
        Mesh* y = myopenglwidget->ReturnMesh();
        slicer->items.append(y);
        // minz=QVector3D(0,0,-10000);
        // maxz=QVector3D(0,0,10000);
        slicer->information(f, thinkess, resolution);
    }
    else
    {
        return ;
    }
    Initialization();
    slicer->fillnormalandvertex();
    slicer->start();
    imagesbyte = slicer->resultbyte();
    spotbyte = slicer->resultspot();
    ui->horizontalSlider_ImageIndex->setEnabled(true);
    ui->horizontalSlider_ImageIndex->setRange(0, imagesbyte.size() - 1);
    ui->horizontalSlider_ImageIndex->setValue(imagesbyte.size() / 3);
    ui->DefineButton->setEnabled(true);
    delete myopenglwidget;
    myopenglwidget = nullptr;
}
void MainWindow::CalR(const Mat& m)
{
    qDebug() << "------Start Add-------";
    T test_threshold;
    float timeSum = 0.0;
    // Detect each image in the directory 'Images1'
    //the name of saved detected images
    Mat testImgOrigin = m;
    Mat testImg = testImgOrigin.clone();
    cvtColor(testImg, testImg, COLOR_BGR2GRAY);
    GaussianBlur(testImg, testImg, Size(9, 9), 2, 2);
    int height = testImg.rows;
    int width = testImg.cols;
    /*---------Illustration for each step---------*/
    Mat test1 = Mat(height, width, CV_8UC1, Scalar(255));
    Mat test2 = Mat(height, width, CV_8UC1, Scalar(255));
    Mat test3 = Mat(height, width, CV_8UC1, Scalar(255));
    Mat test4 = Mat(height, width, CV_8UC1, Scalar(255));
    Mat test5 = Mat(height, width, CV_8UC1, Scalar(255));
    Mat test6 = Mat(height, width, CV_8UC1, Scalar(255));
    Mat test7 = Mat(height, width, CV_8UC1, Scalar(255));
    Mat test8 = Mat(height, width, CV_8UC1, Scalar(255));
    Mat test9 = Mat(height, width, CV_8UC1, Scalar(255));
    Mat test10 = Mat(height, width, CV_8UC1, Scalar(255));
    Mat test11 = Mat(height, width, CV_8UC1, Scalar(255));
    // EDPF Parameter-free Edge Segment Detection
    clock_t start, finish;
    start = clock();
    EDPF testEDPF = EDPF(testImg);
    Mat edgePFImage = testEDPF.getEdgeImage();
    Mat edge = edgePFImage.clone();
    edge = edge * -1 + 255;
    vector<vector<Point> >EDPFsegments = testEDPF.getSegments();// get edge segments
    //plot edge images
    cvtColor(test10, test10, COLOR_GRAY2BGR);
    for (int es1 = 0; es1 < EDPFsegments.size(); es1++)
    {
        int r = rand() % 256;
        int g = rand() % 256;
        int b = rand() % 256;
        Scalar SegEdgesColor = Scalar(b, g, r);
        for (int es2 = 0; es2 < EDPFsegments[es1].size() - 1; es2++)
        {
            cv::line(test10, EDPFsegments[es1][es2], EDPFsegments[es1][es2 + 1], SegEdgesColor, 2);//Scalar(0, 0, 0)
        }
    }
    /*--------delete edge segments whose pixel number is less than 16-------------*/
    vector<vector<Point>> edgeList;
    for (int i = 0; i < EDPFsegments.size(); i++)
    {
        if (EDPFsegments[i].size() >= 16)// segments should have at least 16 pixels
        {
            edgeList.push_back(EDPFsegments[i]);
        }//endif
    }//endfor
    /*----------extract closed edges-------------------*/
    closedEdgesExtract* closedAndNotClosedEdges;
    closedAndNotClosedEdges = extractClosedEdges(edgeList);
    vector<vector<Point> > closedEdgeList;
    closedEdgeList = closedAndNotClosedEdges->closedEdges;
    /*--------approximate edge segments using line segments by method RDP-------*/
    vector<vector<Point> > segList;
    for (int s0 = 0; s0 < edgeList.size(); s0++)
    {
        vector<Point> segTemp;
        RamerDouglasPeucker(edgeList[s0], 2.5, segTemp);//3.0
        segList.push_back(segTemp);
    }
    /*-------------reject sharp turn angles---------------*/
    sharpTurn* newSegEdgeList;
    newSegEdgeList = rejectSharpTurn(edgeList, segList, test_threshold.sharp_angle);
    //new seglist and edgelist
    vector<vector<Point>> newSegList = newSegEdgeList->new_segList;
    vector<vector<Point>> newEdgeList = newSegEdgeList->new_edgeList;
    //plot segLists after sharp turn splitting
    cvtColor(test2, test2, COLOR_GRAY2BGR);
    for (int j = 0; j < newSegList.size(); j++)
    {
        int r = rand() % 256;
        int g = rand() % 256;
        int b = rand() % 256;
        Scalar colorSharpTurn = Scalar(b, g, r);
        for (int jj2 = 0; jj2 < newEdgeList[j].size() - 1; jj2++)
        {
            line(test2, newEdgeList[j][jj2], newEdgeList[j][jj2 + 1], colorSharpTurn, 2);
        }
    }
    /*-----------------Detect inflexion points--------------*/
    InflexionPt* newSegEdgeListAfterInflexion;
    newSegEdgeListAfterInflexion = detectInflexPt(newEdgeList, newSegList);
    // new seglist and edgelist
    vector<vector<Point>> newSegListAfterInflexion = newSegEdgeListAfterInflexion->new_segList;
    vector<vector<Point>> newEdgeListAfterInfexion = newSegEdgeListAfterInflexion->new_edgeList;
    /*--------delete short edgeLists or near line segments----------*/
    vector<vector<Point>>::iterator it = newEdgeListAfterInfexion.begin();
    while (it != newEdgeListAfterInfexion.end())
    {
        /*compute the line segment generated by the two endpoints of the arc,
            and then judge the midpoint of the arc if lying on or near the line
            */
        Point edgeSt = Point((*it).front().x, (*it).front().y);
        Point edgeEd = Point((*it).back().x, (*it).back().y);
        int midIndex = (*it).size() / 2;
        Point edgeMid = Point((*it)[midIndex].x, (*it)[midIndex].y);
        double distStEd = sqrt(pow(edgeSt.x - edgeEd.x, 2) + pow(edgeSt.y - edgeEd.y, 2));
        double distStMid = sqrt(pow(edgeSt.x - edgeMid.x, 2) + pow(edgeSt.y - edgeMid.y, 2));
        double distMidEd = sqrt(pow(edgeEd.x - edgeMid.x, 2) + pow(edgeEd.y - edgeMid.y, 2));
        double distDifference = abs((distStMid + distMidEd) - distStEd);
        if ((*it).size() <= test_threshold.T_l || distDifference <= test_threshold.T_ratio * (distStMid + distMidEd))// 2 3 fixed number; (*it).size() <=20
        {
            it = newEdgeListAfterInfexion.erase(it);
        }
        else
        {
            it++;
        }
    }//endwhile
    cvtColor(test11, test11, COLOR_GRAY2BGR);
    for (int j = 0; j < newEdgeListAfterInfexion.size(); j++)
    {
        int r = rand() % 256;
        int g = rand() % 256;
        int b = rand() % 256;
        Scalar colorAfterDeleteLinePt = Scalar(b, g, r);
        for (int jj2 = 0; jj2 < newEdgeListAfterInfexion[j].size() - 1; jj2++)
        {
            line(test11, newEdgeListAfterInfexion[j][jj2], newEdgeListAfterInfexion[j][jj2 + 1], colorAfterDeleteLinePt, 2);
        }
    }
    /*-----extract closed edgeLists and not closed edgeLists after inflexion point operation------*/
    closedEdgesExtract* closedAndNotClosedEdges1;
    closedAndNotClosedEdges1 = extractClosedEdges(newEdgeListAfterInfexion);
    vector<vector<Point> > closedEdgeList1;
    vector<vector<Point> > notclosedEdgeList1;
    closedEdgeList1 = closedAndNotClosedEdges1->closedEdges;
    notclosedEdgeList1 = closedAndNotClosedEdges1->notClosedEdges;
    //plot closed edgeLists
    cvtColor(test4, test4, COLOR_GRAY2BGR);
    for (int j = 0; j < closedEdgeList1.size(); j++)
    {
        int r = rand() % 256;
        int g = rand() % 256;
        int b = rand() % 256;
        Scalar colorClosedEdges = Scalar(b, g, r);
        for (int jj = 0; jj < closedEdgeList1[j].size() - 1; jj++)
        {
            cv::line(test4, closedEdgeList1[j][jj], closedEdgeList1[j][jj + 1], colorClosedEdges, 2);
        }
    }
    /*----------sort notclosedEdgeList for grouping-------------*/
    std::vector<std::vector<Point>> sortedEdgeList = sortEdgeList(notclosedEdgeList1);
    /*--------------group sortededgeList---------------*/
    groupArcs* arcs = coCircleGroupArcs(sortedEdgeList, test_threshold.T_o, test_threshold.T_r);
    vector<vector<Point> > groupedArcs = arcs->arcsFromSameCircles;
    vector<vector<Point> > groupedArcsThreePt = arcs->arcsStartMidEnd;
    vector<Vec3f>  groupedOR = arcs->recordOR;
    /*--------circle verification using estimated center and radius parameters*/
    vector<Circles> groupedCircles;// grouped arcs
    groupedCircles = circleEstimateGroupedArcs(groupedArcs, groupedOR, groupedArcsThreePt, test_threshold.T_inlier);//fit grouped arcs
    // closed arcs
    for (auto ite = closedEdgeList1.begin(); ite != closedEdgeList1.end(); ite++)
    {
        closedEdgeList.push_back(*ite);
    }//endfor
    vector<Circles> closedCircles;// closedCircles
    closedCircles = circleEstimateClosedArcs(closedEdgeList);// fit closed edges
    //put grouped and closed circles together
    vector<Circles> totalCircles;
    if (!groupedCircles.empty())
    {
        totalCircles = groupedCircles;
    }
    if (!closedCircles.empty())
    {
        for (auto it = closedCircles.begin(); it != closedCircles.end(); it++)
        {
            totalCircles.push_back(*it);
        }
    }
    //cluster circles----------------->no clusteringT_inlier
    finish = clock();
    vector<Circles> preCircles;
    preCircles = clusterCircles(totalCircles);
    QC.cirdata.clear();
    for(int i = 0; i < preCircles.size(); i++)
    {
        struct MyCircles x;
        x.concentric = preCircles.at(i).concentric;
        x.inlierRatio = preCircles.at(i).inlierRatio;
        x.r = preCircles.at(i).r;
        x.xc = preCircles.at(i).xc;
        x.yc = preCircles.at(i).yc;
        QC.cirdata.push_back(x);
    }
    QC.id = ui->horizontalSlider_ImageIndex->value();
    QC.imadata = m;
    QC.isSaved = true;
    //finish = clock();
    timeSum += ((float)(finish - start) / CLOCKS_PER_SEC);
    qDebug() << "---------add circle--ok------";
}
double distance (double x1, double y1, double  x2, double  y2)
{
    return double(qPow((x1 - x2), 2) + qPow((y1 - y2), 2));
}
float getDist_P2L(MyCircles p, MyCircles a, MyCircles b)
{
//    PointP：定点坐标
//    Pointa：直线a点坐标
//    Pointb：直线b点坐标
    float A = 0;
    float B = 0;
    float C = 0;
    A = a.yc - b.yc;
    B = b.xc - a.xc;
    C = a.xc * b.yc - a.yc * b.xc;
    float distance = 0;
    distance = (A * p.xc + B * p.yc + C) / sqrt(A * A + B * B);
    return distance;
}
vector<MyCircles> MainWindow::Sort_circles(vector<MyCircles>& preCircles)
{
    vector<MyCircles> result;
    int err = ui->textEdit->toPlainText().toFloat() * 2 / 3;
    vector<MyCircles> temp = preCircles;
    while(temp.size() > 0)
    {
        sort(temp.begin(), temp.end(), [](MyCircles p1, MyCircles p2)
        {
            return p1.xc + p1.yc < p2.xc + p2.yc;
        });
        MyCircles left = temp.at(0);
        sort(temp.begin(), temp.end(), [](MyCircles p1, MyCircles p2)
        {
            return p1.xc - p1.yc > p2.xc - p2.yc;
        });
        MyCircles right = temp.at(0);
        vector<MyCircles> row_points;
        vector<MyCircles> remaining_points;
        for(int i = 0; i < temp.size(); i++)
        {
            MyCircles p = temp.at(i);
            float dis = getDist_P2L(p, left, right);
            if(dis < err)
            {
                row_points.push_back(p);
            }
            else
            {
                remaining_points.push_back(p);
            }
        }
        sort(row_points.begin(), row_points.end(), [](MyCircles a, MyCircles b)
        {
            return a.xc < b.xc;
        });
        result.insert(result.end(), row_points.begin(), row_points.end());
        temp = remaining_points;
    }
    return result;
}
void miniCircle(QVector2D A, QVector2D B, QVector2D C, QVector2D& center)
{
    double Xmove = A.x();
    double Ymove = A.y();
    B.setX(B.x() - A.x());
    B.setY(B.y() - A.y());
    C.setX(C.x() - A.x());
    C.setY(C.y() - A.y());
    A.setX(0);
    A.setY(0);
    double x1 = B.x(), y1 = B.y(), x2 = C.x(), y2 = C.y();
    double m = 2.0 * (x1 * y2 - y1 * x2);
    center.setX((x1 * x1 * y2 - x2 * x2 * y1 + y1 * y2 * (y1 - y2)) / m);
    center.setY((x1 * x2 * (x2 - x1) - y1 * y1 * x2 + x1 * y2 * y2) / m);
    center.setX(center.x() + Xmove);
    center.setY(center.y() + Ymove);
}
void MainWindow::Cal_circles(std::vector<MyCircles>& preCircles)
{
    QImage img = QImage(resolution, QImage::Format_RGB32);
    img.fill(Qt::black);
    QPainter painter(&img);
    painter.setRenderHint(QPainter::Antialiasing, true);
    //画出所有的线
    float err1 = ui->textEdit->toPlainText().toFloat() * 1.1;
    float err2 = ui->textEdit->toPlainText().toFloat() * 0.9;
    QVector<QVector<QVector2D>> cirspots;
    for(int i = 0; i < preCircles.size(); i++)
    {
        QVector<QVector2D> cirspot;
        double x1 = preCircles.at(i).xc;
        double y1 = preCircles.at(i).yc;
        for(int j = 0; j < spotbyte.at(QC.id).size(); j++)
        {
            double x2 = spotbyte.at(QC.id).at(j).x();
            double y2 = spotbyte.at(QC.id).at(j).y();
            if(distance(x1, y1, x2, y2) < qPow(err1, 2) && distance(x1, y1, x2, y2) > qPow(err2, 2))
            {
                cirspot.append(spotbyte.at(QC.id).at(j));
            }
        }
        cirspots.append(cirspot);
    }
    QTime time;
    time = QTime::currentTime();
    qsrand(time.msec() + time.second() * 1000);
    for(int k = 0; k < cirspots.size(); k++)
    {
        err1 = ui->textEdit->toPlainText().toFloat() * 0.1;
        double x1 = preCircles.at(k).xc;
        double y1 = preCircles.at(k).yc;
        QVector<double> RA;
        QVector2D A = cirspots.at(k).at(qrand() % cirspots.at(k).size());
        QVector2D B = cirspots.at(k).at(qrand() % cirspots.at(k).size());
        QVector2D C = cirspots.at(k).at(qrand() % cirspots.at(k).size());
        QVector2D center_res;
        miniCircle(A, B, C, center_res);
        if(std::isnan(center_res.x()) | std::isnan(center_res.y()) | (distance(center_res.x(), center_res.y(), x1, y1) > qPow(err1, 2)))
        {
            k--;
            continue;
        }
        float res_x = center_res.x(), res_y = center_res.y();
        for(int i = 1; i < 100; i++)
        {
            A = cirspots.at(k).at(qrand() % cirspots.at(k).size());
            B = cirspots.at(k).at(qrand() % cirspots.at(k).size());
            C = cirspots.at(k).at(qrand() % cirspots.at(k).size());
            QVector2D center;
            miniCircle(A, B, C, center);
            if(std::isnan(center.x()) | std::isnan(center.y()) | (distance(center.x(), center.y(), x1, y1) > qPow(err1, 2)))
            {
                i--;
                continue;
            }
            res_x = (res_x + center.x()) / 2.0;
            res_y = (res_y + center.y()) / 2.0;
        }
        center_res.setX(res_x);
        center_res.setY(res_y);
        for(int i = 0; i < cirspots.at(k).size(); i++)
        {
            double rrr = cirspots.at(k).at(i).distanceToPoint(center_res);
            if(!std::isnan(rrr))
            {
                RA.append(rrr);
            }
        }
        double sum = accumulate(RA.begin(), RA.end(), 0.0);
        double mean =  sum / RA.size();
        // 求方差与标准差
        double variance  = 0.0;
        for (uint16_t i = 0 ; i < RA.size() ; i++)
        {
            variance = variance + pow(RA.at(i) - mean, 2);
        }
        variance = variance / RA.size();
        double standard_deviation = sqrt(variance);
        qDebug() << "------mean << standard_deviation---------" << mean << standard_deviation;
        int num = 0;
        sum = 0.0;
        if(standard_deviation > 1)
        {
            sort(RA.begin(), RA.end(), [](double p1, double p2)
            {
                return p1 < p2;
            });
            num = RA.size() / 10;
            for(int i = 0; i < num; i++)
            {
                sum += RA.at(i);
            }
            mean = sum * 1.0 / (num * 1.0);
            if(abs(mean - preCircles.at(k).r) > 5)
            {
                painter.setPen(QPen(Qt::blue, 5));
                painter.drawPoint(QPointF(preCircles.at(k).xc, preCircles.at(k).yc));
                painter.setPen(QPen(Qt::white, 5));
                painter.drawPoint(QPointF(center_res.x(), center_res.y()));
                painter.setPen(QPen(Qt::white, 3));
                for(int i = 0; i < cirspots.at(k).size(); i++)
                {
                    painter.drawPoint(QPointF(cirspots.at(k).at(i).x(), cirspots.at(k).at(i).y()));
                }
            }
        }
        else
        {
            for (uint16_t i = 0 ; i < RA.size() ; i++)
            {
                if(pow(RA.at(i) - mean, 2) < 0.1)
                {
                    sum += RA.at(i);
                    num++;
                }
            }
            mean = (sum * 1.0) / (num * 1.0);
            if(abs(mean - preCircles.at(k).r) > 5)
            {
                painter.setPen(QPen(Qt::blue, 5));
                painter.drawPoint(QPointF(preCircles.at(k).xc, preCircles.at(k).yc));
                painter.setPen(QPen(Qt::white, 5));
                painter.drawPoint(QPointF(center_res.x(), center_res.y()));
                painter.setPen(QPen(Qt::white, 3));
                for(int i = 0; i < cirspots.at(k).size(); i++)
                {
                    painter.drawPoint(QPointF(cirspots.at(k).at(i).x(), cirspots.at(k).at(i).y()));
                }
            }
        }
        //preCircles.at(k).xc = center_res.x();
        //preCircles.at(k).yc = center_res.y();
        preCircles.at(k).r = mean;
        qDebug() << "-------------k << mean << num---------" << k << mean << num;
    }
    img.save(paths + "/out/Cal_circles.png");
}

cv::Mat MainWindow::QImage2cvMat(const QImage& image)
{
    cv::Mat mat;
    switch(image.format())
    {
        case QImage::Format_Grayscale8: //灰度图，每个像素点1个字节（8位）
            //Mat构造：行数，列数，存储结构，数据，step每行多少字节
            mat = cv::Mat(image.height(), image.width(), CV_8UC1, (void*)image.constBits(), image.bytesPerLine());
            break;
        case QImage::Format_ARGB32: //32位存储0xAARRGGBB，pc一般小端存储低位在前，所以字节顺序就成了BGRA
        case QImage::Format_RGB32: //Alpha为FF
        case QImage::Format_ARGB32_Premultiplied:
            mat = cv::Mat(image.height(), image.width(), CV_8UC4, (void*)image.constBits(), image.bytesPerLine());
            break;
        case QImage::Format_RGB888: //RR,GG,BB字节顺序存储
            mat = cv::Mat(image.height(), image.width(), CV_8UC3, (void*)image.constBits(), image.bytesPerLine());
            cv::cvtColor(mat, mat, cv::COLOR_RGB2BGR); //opencv需要转为BGR的字节顺序
            break;
        case QImage::Format_RGBA8888: //64为存储，顺序和Format_ARGB32相反
            mat = cv::Mat(image.height(), image.width(), CV_16UC4, (void*)image.constBits(), image.bytesPerLine());
            cv::cvtColor(mat, mat, cv::COLOR_RGB2BGR); //opencv需要转为BGR的字节顺序
            break;
    }
    return mat;
}
QImage MainWindow::MatToImage(const Mat& mat)   //Mat格式转化QImage类型
{
    QImage image;
    switch(mat.type())
    {
        case CV_8UC1:
            //QImage构造：数据，宽度，高度，每行多少字节，存储结构
            image = QImage((const unsigned char*)mat.data, mat.cols, mat.rows, mat.cols, QImage::Format_Grayscale8);
            break;
        case CV_8UC3:
            image = QImage((const unsigned char*)mat.data, mat.cols, mat.rows, mat.cols * 3, QImage::Format_RGB888);
            image = image.rgbSwapped(); //BRG转为RGB
            //Qt5.14增加了Format_BGR888
            //image = QImage((const unsigned char*)mat.data, mat.cols, mat.rows, mat.cols * 3, QImage::Format_BGR888);
            break;
        case CV_8UC4:
            image = QImage((const unsigned char*)mat.data, mat.cols, mat.rows, mat.cols * 4, QImage::Format_ARGB32);
            break;
    }
    return image;
}
QImage MainWindow::DrawCir(Mat& m)
{
    vector<MyCircles> pcircles = QC.cirdata;
    QImage temp = MatToImage(m);
    QPainter painter(&temp);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(Qt::blue);
    painter.setFont(QFont("Arial", 18));
    for (size_t i = 0; i < pcircles.size(); i++)
    {
        QPointF center(pcircles[i].xc, pcircles[i].yc);
        double r = pcircles[i].r;
        int pennum = 2;
        painter.setPen(Qt::blue);
        painter.drawText(center, QString::number(r, 'f', 4));
        //画圆
        painter.setPen(QPen(Qt::red, pennum));
        qreal rxxx = 1.0 * r;
        painter.drawEllipse(center, rxxx, rxxx );
    }
    return temp;
}
float MainWindow::normalize(float input)
{
    //将Omin，Omax上每个数映射到Nmin，Nmax上
    float Nmin, Nmax, Omax, Omin;
    float normalized_x;
    float mid;
    mid = ui->textEdit->toPlainText().toFloat();
    if(input >= mid)
    {
        Nmin = 0;
        Nmax = 255;
        Omax = ui->Max_->text().toFloat();
        Omin = mid;
    }
    else if(input < mid)
    {
        Nmin = -255;
        Nmax = 0;
        Omax = mid;
        Omin = ui->Min_->text().toFloat();
    }
    //qDebug() << Omin << Omax << Nmin << Nmax;
    if(normalized_x > 255)
    {
        normalized_x = 255;
    }
    if(normalized_x < -255)
    {
        normalized_x = -255;
    }
    normalized_x = (Nmax - Nmin) * 1.0 / (Omax - Omin) * (input - Omin) + Nmin;
    return normalized_x;
}

Mat MainWindow::AddAlpha(const Mat& canvas, int x)
{
    QString imgpath = "";
    Mat Rescombine(resolution.height(), resolution.width(), CV_8UC4);
    int w = canvas.cols;
    int h = canvas.rows;
    Mat Baseimg;
    if(!x)
    {
        Baseimg = Mat(resolution.height(), resolution.width(), CV_8UC4, Scalar(255, 255, 255, 255));
        for (int row = 0; row < h; row++)
        {
            //获取每行首元素地址：该步还可优化成pdata += image.step
            //从而去掉乘号，用加号代替(加号比乘号的效率高)
            uchar* pdataRes = Rescombine.data + row * Rescombine.step;
            uchar* pdatacanvas = canvas.data + row * canvas.step;
            uchar* pdataBaseimg = Baseimg.data + row * Baseimg.step;
            for (int col = 0; col < w; col++)
            {
                //(sRGB*srcA+dRGB*dstA*(1-srcA))/outA
                float srcA = pdatacanvas[3] / 255.0;
                float dstA = pdataBaseimg[3] / 255.0;
                float outA = srcA + dstA * (1 - srcA);
                pdataRes[0] = (pdatacanvas[0] * srcA + pdataBaseimg[0] * dstA * (1 - srcA)) / outA; // 作为彩图一个位置的b/g/r是连续存放的
                pdataRes[1] = (pdatacanvas[1] * srcA + pdataBaseimg[1] * dstA * (1 - srcA)) / outA;
                pdataRes[2] = (pdatacanvas[2] * srcA + pdataBaseimg[2] * dstA * (1 - srcA)) / outA;
                pdataRes[3] = outA * 255;
                pdataRes += 4;           //每移动4个位置才相当于移动一个像素点
                pdatacanvas += 4;
                pdataBaseimg += 4;
            }
        }
    }
    else
    {
        //imgpath = QFileDialog::getOpenFileName(this, tr("导入png文件"), paths, "png(*.png)");
        imgpath = "E:/C++/build-GreyImage-Desktop_Qt_5_11_1_MSVC2017_64bit-Release/release/stl/3522.png";
        if(imgpath.isEmpty())
        {
            Baseimg = Mat(resolution.height(), resolution.width(), CV_8UC4, Scalar(255, 255, 255, 255));
        }
        else
        {
            Baseimg = imread(imgpath.toStdString(), IMREAD_UNCHANGED);
            if(Baseimg.channels() == 3)
            {
                qDebug() << "------BGR2BGRA--------";
                cvtColor(Baseimg, Baseimg, COLOR_BGR2BGRA);
            }
        }
        addWeighted(canvas, 0.5, Baseimg, 0.5, 0, Rescombine);
    }
    return Rescombine;
}

std::vector<MyCircles> MainWindow::Processing_concentriccircles(std::vector<MyCircles>& preCircles)
{
    vector<MyCircles> resCir;
    float err = ui->textEdit->toPlainText().toFloat();
    for(int i = 0; i < preCircles.size(); i++)
    {
        if(preCircles[i].concentric == true)
        {
            continue;
        }
        bool issame = false;
        double x2 = preCircles[i].xc;
        double y2 = preCircles[i].yc;
        double r2 = preCircles[i].r;
        double inlierRatio2 = preCircles[i].inlierRatio;
        for(int j = i + 1; j < preCircles.size(); j++)
        {
            double x1 = preCircles[j].xc;
            double y1 = preCircles[j].yc;
            double r1 = preCircles[j].r;
            double inlierRatio1 = preCircles[j].inlierRatio;
            if(distance(x1, y1, x2, y2) < qPow(err, 2))
            {
                x2 = (x1 + x2) / 2.0;
                y2 = (y1 + y2) / 2.0;
                r2 = (r1 + r2) / 2.0;
                inlierRatio2 = (inlierRatio1 + inlierRatio2) / 2.0;
                issame = true;
                preCircles[j].concentric = true;
            }
        }
        if(!issame)
        {
            resCir.push_back(preCircles[i]);
        }
        else
        {
            MyCircles new_;
            new_.inlierRatio = inlierRatio2;
            new_.r = r2;
            new_.xc = x2;
            new_.yc = y2;
            resCir.push_back(new_);
        }
    }
    return resCir;
}

std::vector<MyCircles> MainWindow::DeleteSmallCircle(std::vector<MyCircles>& preCircles)
{
    vector<MyCircles> resCir;
    float errlow = Target_radius * f * 2 / 3.0;
    float errtop = Target_radius * f * 4 / 3.0;
    for(int i = 0; i < preCircles.size(); i++)
    {
        MyCircles temp = preCircles[i];
        if(temp.r >= errlow && temp.r <= errtop)
        {
            resCir.push_back(temp);
        }
    }
    qDebug() << "DeleteSmallCircle:" << resCir.size();
    return resCir;
}

Mat MainWindow::ShowGray(vector<MyCircles>Calgray_Queue)
{
    float x;
    x = ui->textEdit->toPlainText().toFloat();
    Mat Graycanvas(resolution.height(), resolution.width(), CV_8UC4);
    Mat GRcanvas(resolution.height(), resolution.width(), CV_8UC4);
    int rownum = 1;
    int colnum = 0;
    for (int i = 1; i < Calgray_Queue.size(); i++)
    {
        if(Calgray_Queue.at(i).xc < Calgray_Queue.at(i - 1).xc)
        {
            rownum++;
        }
    }
    colnum = ceil(Calgray_Queue.size() * 1.0 / rownum);
    int width = resolution.width() / colnum;
    int height = resolution.height() / rownum;
    for (size_t i = 0; i < Calgray_Queue.size(); i++)
    {
        Point pt1;
        Point pt2;
        pt1.x = (i % colnum) * width;
        pt1.y = (i / colnum) * height;
        pt2.x = pt1.x + width;
        pt2.y = pt1.y + height;
        float r = Calgray_Queue.at(i).r;
        float Difference = (r - x);
        cv::Scalar Graycolor; //BGRA
        cv::Scalar GRcolor; //BGRA
        if(Difference > 0)
        {
            Graycolor[0] = 0;
            Graycolor[1] = 0;
            Graycolor[2] = 0;
            GRcolor[0] = 0;
            GRcolor[1] = 0;
            GRcolor[2] = 255;
        }
        else
        {
            Graycolor[0] = 255;
            Graycolor[1] = 255;
            Graycolor[2] = 255;
            GRcolor[0] = 0;
            GRcolor[1] = 255;
            GRcolor[2] = 0;
        }
        float m = normalize(r);
        m =  qAbs(m);
        //qDebug() << r << m;
        Graycolor[3] = m;
        GRcolor[3] = m;
        cv::rectangle(Graycanvas, pt1, pt2, Graycolor, -1);
        cv::rectangle(GRcanvas, pt1, pt2, GRcolor, -1);
    }
    vector<int> compression_params;
    compression_params.push_back(IMWRITE_PNG_COMPRESSION); //PNG格式图像压缩标志
    cv::imwrite(paths.toStdString() + "/out/Graycanvas.png", Graycanvas);
    cv::GaussianBlur(Graycanvas, Graycanvas, Size(0, 0), 40, 0);
    cv::GaussianBlur(Graycanvas, Graycanvas, Size(0, 0), 40, 0);
    cv::imwrite(paths.toStdString() + "/out/GaussianBlur.png", Graycanvas);
    Mat ComGraycanvas(resolution.height(), resolution.width(), CV_8UC4);
    ComGraycanvas = AddAlpha(Graycanvas, 1);
    cv::imwrite(paths.toStdString() + "/out/ComGraycanvas.png", ComGraycanvas);
    Mat ComGRcanvas(resolution.height(), resolution.width(), CV_8UC4);
    ComGRcanvas = AddAlpha(GRcanvas, 0);
    cv::imwrite(paths.toStdString() + "/out/ResGRcanvas.png", ComGRcanvas);
    return ComGraycanvas;
}
void MainWindow::flashqueue()
{
    QPixmap clear = QPixmap();
    QImage temp = MatToImage(queueimages.at(queueindex).imadata);
    switch (queueindex)
    {
        case 0:
            ui->mlabel_1->setPixmap(clear);
            ui->mlabel_1->setPixmap(QPixmap::fromImage(temp).scaled(ui->mlabel_1->width(), ui->mlabel_1->height(), Qt::KeepAspectRatio));
            break;
        case 1:
            ui->mlabel_2->setPixmap(clear);
            ui->mlabel_2->setPixmap(QPixmap::fromImage(temp).scaled(ui->mlabel_2->width(), ui->mlabel_2->height(), Qt::KeepAspectRatio));
            break;
        case 2:
            ui->mlabel_3->setPixmap(clear);
            ui->mlabel_3->setPixmap(QPixmap::fromImage(temp).scaled(ui->mlabel_3->width(), ui->mlabel_3->height(), Qt::KeepAspectRatio));
            break;
        case 3:
            ui->mlabel_4->setPixmap(clear);
            ui->mlabel_4->setPixmap(QPixmap::fromImage(temp).scaled(ui->mlabel_4->width(), ui->mlabel_4->height(), Qt::KeepAspectRatio));
            break;
        case 4:
            ui->mlabel_5->setPixmap(clear);
            ui->mlabel_5->setPixmap(QPixmap::fromImage(temp).scaled(ui->mlabel_5->width(), ui->mlabel_5->height(), Qt::KeepAspectRatio));
            break;
        default:
            break;
    }
}
void MainWindow::on_horizontalSlider_ImageIndex_valueChanged(int value)
{
    ui->label_2->setPixmap((QPixmap::fromImage(QImage::fromData(imagesbyte.at(value)))).scaled(ui->label_2->width(), ui->label_2->height(), Qt::KeepAspectRatio));
    QString strtext = QString("%1/%2").arg(value + 1).arg(imagesbyte.size());
    ui->label_layer->setText(strtext);
}
void MainWindow::on_CalButton_clicked()
{
    ui->CalButton->setEnabled(false);
    Mat temp;
    QImage now;
    temp = QImage2cvMat(NowImages);
    CalR(temp);
    QC.cirdata = DeleteSmallCircle(QC.cirdata);
    QC.cirdata = Processing_concentriccircles(QC.cirdata);
    QC.cirdata = Sort_circles(QC.cirdata);
    Cal_circles(QC.cirdata);
    now = DrawCir(temp);
    ui->label_2->setPixmap(QPixmap::fromImage(now).scaled(ui->label_2->width(), ui->label_2->height(), Qt::KeepAspectRatio));
    now.save(paths + "/out/canvas.png");
    ui->DefineButton->setEnabled(true);
    ui->SaveButton->setEnabled(true);
}
void MainWindow::on_DefineButton_clicked()
{
    QByteArray dataX = imagesbyte.at(ui->horizontalSlider_ImageIndex->value());
    NowImages = QImage::fromData(dataX);
    NowImages = NowImages.convertToFormat(QImage::Format_ARGB32);
    ui->CalButton->setEnabled(true);
    ui->DefineButton->setEnabled(true);
}
void MainWindow::on_SaveButton_clicked()
{
    queueindex = queueindex % 5;
    queueimages.replace(queueindex, QC);
    flashqueue();
    queueindex = queueindex + 1;
    ui->SaveButton->setEnabled(false);
    ui->Calgray->setEnabled(true);
    ui->ExportButton->setEnabled(true);
}
void MainWindow::on_Calgray_clicked()
{
    vector<MyCircles> Calgray_Queue;
    Calgray_Queue.clear();
    for(int i = 0; i < queueindex ; i++)
    {
        for(int j = 0; j < queueimages.at(i).cirdata.size(); j++)
        {
            Calgray_Queue.push_back(queueimages.at(i).cirdata.at(j));
        }
    }
    Calgray_Queue = DeleteSmallCircle(Calgray_Queue);
    Calgray_Queue = Processing_concentriccircles(Calgray_Queue);
    float max = -10000.0f;
    float min = 10000.0f;
    for (size_t i = 0; i < Calgray_Queue.size(); i++)
    {
        if(max < Calgray_Queue.at(i).r)
        {
            max = Calgray_Queue.at(i).r;
        }
        if(min > Calgray_Queue.at(i).r)
        {
            min = Calgray_Queue.at(i).r;
        }
    }
    Calgray_Queue = Sort_circles(Calgray_Queue);
    ui->Max_->setText(QString::number(max));
    ui->Min_->setText(QString::number(min));
    Mat y ;
    y = ShowGray(Calgray_Queue);
    qDebug() << "Calgray OK";
}

void MainWindow::on_ExportButton_clicked()
{
    for(int i = 0; i < queueindex; i++)
    {
        QString path = paths + QString("/out/%1.xml").arg(i);
        FileStorage fs(path.toStdString(), FileStorage::WRITE);
        Mat mat = queueimages.at(i).imadata;
        fs << "mat" << mat;
        for(int j = 0; j < queueimages.at(i).cirdata.size(); j++)
        {
            MyCircles x = queueimages.at(i).cirdata.at(j);
            fs << "x" << x.xc;
            fs << "y" << x.yc;
            fs << "r" << x.r;
            fs << "inlierRatio" << QString::number(x.inlierRatio, 'f', 3).toFloat();
        }
        int id = queueimages.at(i).id;
        fs << "id" << id;
        fs.release();
    }
    qDebug() << "Export ok" ;
    ui->ExportButton->setEnabled(false);
}
void MainWindow::on_pushButton_clicked()
{
    if(!myopenglwidget)
    {
        return;
    }
    Cleandata();
    Initialization();
    Show_Image();
}

void MainWindow::on_pushButton_2_clicked()
{
    if(myopenglwidget)
    {
        vertices_res = myopenglwidget->ReturnVertices();
        QString fileName = "rotatedVector.stl";
        QFile file(fileName);//可以自己选择路径来保存文件名
        if(!file.open(QIODevice::WriteOnly))
        {
            qDebug() << "Open failed";
            return;
        }
        else
        {
            qDebug() << "Open success";
            QDataStream stream(&file);
            stream.setByteOrder(QDataStream::LittleEndian);
            stream.setFloatingPointPrecision(QDataStream::SinglePrecision);
            QString head = "My_Stl";
            stream.writeRawData(QByteArray(head.toUtf8()).constData(), head.size());
            stream.writeRawData(QByteArray(80 - head.size(), '\0').constData(), 80 - head.size());
            stream << quint32(vertices_res.size() / 18);
            //循环写入法线、顶点等数据
            for (int i = 0; i < vertices_res.size(); i = i + 18)
            {
                QVector3D tempnormal = QVector3D(vertices_res.at(i + 3), vertices_res.at(i + 4), vertices_res.at(i + 5));
                stream << tempnormal.x() << tempnormal.y() << tempnormal.z();
                QVector3D tempvertex = QVector3D(vertices_res.at(i), vertices_res.at(i + 1), vertices_res.at(i + 2));
                stream << tempvertex.x() << tempvertex.y() << tempvertex.z();
                tempvertex = QVector3D(vertices_res.at(i + 6), vertices_res.at(i + 7), vertices_res.at(i + 8));
                stream << tempvertex.x() << tempvertex.y() << tempvertex.z();
                tempvertex = QVector3D(vertices_res.at(i + 12), vertices_res.at(i + 13), vertices_res.at(i + 14));
                stream << tempvertex.x() << tempvertex.y() << tempvertex.z();
                stream << quint16(0);
            }
            file.close();
            qDebug() << "---stl--ok---";
        }
    }
    else
    {
        return ;
    }
}

void MainWindow::on_pushButton_3_clicked()
{
    if(myopenglwidget)
    {
        if(!myopenglwidget->isStartTest)
        {
            ui->pushButton_3->setText("Finish");
        }
        else
        {
            ui->pushButton_3->setText("RO");
        }
        myopenglwidget->isStartTest = (!myopenglwidget->isStartTest);
    }
    else
    {
        return;
    }
}

void MainWindow::on_pushButton_4_clicked()
{
    if(myopenglwidget)
    {
        if(myopenglwidget->isSelectMM == 0)
        {
            ui->pushButton_4->setText("MINZ");
        }
        else if(myopenglwidget->isSelectMM == 1)
        {
            ui->pushButton_4->setText("MAXZ");
        }
        else
        {
            ui->pushButton_4->setText("SL");
        }
        myopenglwidget->isSelectMM = (myopenglwidget->isSelectMM + 1) % 3;
    }
    else
    {
        return;
    }
}

void MainWindow::on_pushButton_5_clicked()
{
    Mesh* y = myopenglwidget->ReturnMesh();
    QString fileName = "rotatedVector.stl";
    QFile file(fileName);//可以自己选择路径来保存文件名
    if(!file.open(QIODevice::WriteOnly))
    {
        qDebug() << "Open failed";
        return;
    }
    else
    {
        qDebug() << "Open success";
        QDataStream stream(&file);
        stream.setByteOrder(QDataStream::LittleEndian);
        stream.setFloatingPointPrecision(QDataStream::SinglePrecision);
        QString head = "My_Stl";
        stream.writeRawData(QByteArray(head.toUtf8()).constData(), head.size());
        stream.writeRawData(QByteArray(80 - head.size(), '\0').constData(), 80 - head.size());
        stream << quint32(y->vertices.size() / 3);
        //循环写入法线、顶点等数据
        for (int i = 0; i < y->vertices.size(); i = i + 3)
        {
            stream << y->vertices.at(i).Normal.x() << y->vertices.at(i).Normal.y() << y->vertices.at(i).Normal.z();
            stream << y->vertices.at(i).Position.x() << y->vertices.at(i).Position.y() << y->vertices.at(i).Position.z();
            stream << y->vertices.at(i + 1).Position.x() << y->vertices.at(i + 1).Position.y() << y->vertices.at(i + 1).Position.z();
            stream << y->vertices.at(i + 2).Position.x() << y->vertices.at(i + 2).Position.y() << y->vertices.at(i + 2).Position.z();
            stream << quint16(0);
        }
        file.close();
        qDebug() << "---stl--ok---";
    }
}
