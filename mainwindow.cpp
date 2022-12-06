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
#include <QBuffer>
#include<QMimeData>

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
    float sharp_angle = 60;//35 40 45 50 55 60

} T;
struct Queue_Circle
{
    std::vector<MyCircles> cirdata;
    Mat imadata;
    bool isSaved = false;
    int id = -1;
} QC;

void myMessageOutput(QtMsgType type, const QMessageLogContext& context, const QString& msg);
QVector<QVector2D> sort_cirspot(QVector<QVector2D> cirspot, double x1, double y1);

MainWindow::MainWindow(QWidget* parent):
    QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    qInstallMessageHandler(myMessageOutput);
    ui->setupUi(this);
    //.exe
    paths = QCoreApplication::applicationDirPath();
    clean();
    utils::logging::setLogLevel(utils::logging::LOG_LEVEL_ERROR);
    this->setAcceptDrops(true);
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::clean()
{
    ui->horizontalSlider_ImageIndex->setEnabled(false);
    ui->horizontalSlider_ImageIndex->setRange(0, 0);
    ui->DefineButton->setEnabled(false);
    ui->CalButton->setEnabled(false);
    ui->SaveButton->setEnabled(false);
    ui->ExportButton->setEnabled(false);
    ui->Calgray->setEnabled(false);
    imagesbyte.clear();
    spotbyte.clear();
    queueimages.clear();
    queueimages.resize(5);
    QPixmap clear = QPixmap();
    ui->label_2->setPixmap(clear);
    ui->mlabel_1->setPixmap(clear);
    ui->mlabel_2->setPixmap(clear);
    ui->mlabel_3->setPixmap(clear);
    ui->mlabel_4->setPixmap(clear);
    ui->mlabel_5->setPixmap(clear);
    ui->textEdit->setText(QString::number(3));
    index = 0;
}
void MainWindow::on_ImportButton_clicked()
{
    if(model_temp)
    {
        delete model_temp;
        model_temp = nullptr;
        qDebug() << "delete model_temp";
    }
    QString path = QFileDialog::getOpenFileName(this, tr("导入stl文件"), paths, "stl(*.stl)");
    //QString path = "F:/STL/扫描切割/AP_2_0.5.stl";
    if(path.isEmpty())
    {
        return;
    }
    model_temp = Model::createModel(path);
    if(model_temp)
    {
        qDebug() << "Import OK" ;
    }
    tocentre();
    Show_Image();
}
void MainWindow::Show_Image()
{
    Slicer* slicer = new Slicer();
    if(model_temp)
    {
        for(int i = 0; i < model_temp->meshes.size(); i++)
        {
            Mesh* y = model_temp->meshes.at(i);
            slicer->items.append(y);
        }
        d = new Dia(this);
        if(d->exec() == QDialog::Accepted)
        {
            f = d->f;
            thinkess = d->mlayer;
            Target_radius = d->radius;
            resolution = QSize(d->x, d->y);
            ui->label_8->setText(QString::number(d->x));
            ui->label_9->setText(QString::number(d->y));
            slicer->information(f, thinkess, resolution, d->isCircle);
        }
        else
        {
            return ;
        }
        slicer->fillnormalandvertex();
        slicer->start();
    }
    else
    {
        return;
    }
    clean();
    imagesbyte = slicer->resultbyte();
    spotbyte = slicer->resultspot();
    ui->horizontalSlider_ImageIndex->setEnabled(true);
    ui->horizontalSlider_ImageIndex->setRange(0, imagesbyte.size() - 1);
    ui->horizontalSlider_ImageIndex->setValue(imagesbyte.size() / 3);
    ui->DefineButton->setEnabled(true);
}

void MainWindow::tocentre()
{
    Mesh* M = model_temp->meshes.at(0);
    float minx = 100000, miny = 1000000, maxx = -10000, maxy = -100000;
    for(int i = 0; i < M->vertices.size(); i++)
    {
        Vertex x = model_temp->meshes.at(0)->vertices.at(i);
        if(minx > x.Position.x())
        {
            minx = x.Position.x();
        }
        if(maxx < x.Position.x())
        {
            maxx = x.Position.x();
        }
        if(miny > x.Position.y())
        {
            miny = x.Position.y();
        }
        if(maxy < x.Position.y())
        {
            maxy = x.Position.y();
        }
    }
    float centrex = (minx + maxx) / 2.0;
    float centrey = (miny + maxy) / 2.0;
    for(int i = 0; i < M->vertices.size(); i++)
    {
        model_temp->meshes[0]->vertices[i].Position.setX( model_temp->meshes.at(0)->vertices.at(i).Position.x() - centrex);
        model_temp->meshes[0]->vertices[i].Position.setY( model_temp->meshes.at(0)->vertices.at(i).Position.y() - centrey);
    }
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
    int err = Target_radius * f;
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
    center.setX( center.x() + Xmove);
    center.setY( center.y() + Ymove);
}
double MainWindow::Deletestandard_deviation(QVector<QVector2D> cirspot, double x1, double y1, double err1, float& r_x, float& r_y, int k)
{
    QVector<double> RA_;
    cirspot = sort_cirspot(cirspot, x1, y1);
    double mean = 0;
    int num = 0;
    int i = 0;
    while(num < 1000000)
    {
        bool flag = false;
        QVector<double> RA;
        int ve_size = cirspot.size() / 3;
        QVector2D A = cirspot.at(i);
        QVector2D B = cirspot.at(i + ve_size);
        QVector2D C = cirspot.at(i + ve_size * 2);
        i = (i + 1) % ve_size;
        QVector2D center_res;
        miniCircle(A, B, C, center_res);
        if(std::isnan(center_res.x()) | std::isnan(center_res.y()) | (distance(center_res.x(), center_res.y(), x1, y1) > qPow(err1, 2)))
        {
            num++;
            continue;
        }
        float res_x = center_res.x(), res_y = center_res.y();
        for(; i < ve_size; i++)
        {
            A = cirspot.at(i);
            B = cirspot.at((i + ve_size));
            C = cirspot.at((i + ve_size * 2));
            QVector2D center;
            miniCircle(A, B, C, center);
            if(std::isnan(center.x()) | std::isnan(center.y()) | (distance(center.x(), center.y(), x1, y1) > qPow(err1, 2)))
            {
                num++;
                if(num > 1000000)
                {
                    qDebug() << "---------1000000---------";
                    return mean;
                }
                else
                {
                    //i--;
                    continue;
                }
            }
            // xy_.append(center);
            res_x = (res_x + center.x()) / 2.0;
            res_y = (res_y + center.y()) / 2.0;
        }
        center_res.setX(res_x);
        center_res.setY(res_y);
        r_x = center_res.x();
        r_y = center_res.y();
        for(i = 0; i < cirspot.size(); i++)
        {
            double rrr = cirspot.at(i).distanceToPoint(center_res);
            RA.append(rrr);
        }
        sort(RA.begin(), RA.end(), less<double>());
        RA_ = RA;
        double sum = accumulate(RA.begin(), RA.end(), 0.0);
        mean =  sum / RA.size();
        // 求方差与标准差
        double variance  = 0.0;
        for (i = 0 ; i < RA.size() ; i++)
        {
            variance = variance + pow(RA.at(i) - mean, 2);
        }
        variance = variance / RA.size();
        double standard_deviation = sqrt(variance);
        qDebug() << "standard_deviation:" << standard_deviation << "RA.size()" << RA.size();
        if(standard_deviation < 30)
        {
            break;
        }
        QVector<QVector2D> REcirspot;
        for(i = 0; i < cirspot.size(); i++)
        {
            double rrr = cirspot.at(i).distanceToPoint(center_res);
            if(abs(rrr - mean) < 3 * standard_deviation)
            {
                REcirspot.append(cirspot.at(i));
            }
            else
            {
                flag = true;
            }
        }
        if(flag)
        {
            i = 0;
            cirspot = REcirspot;
            cirspot = sort_cirspot(cirspot, x1, y1);
        }
        else
        {
            break;
        }
        num++;
    }
    //CSV
    {
        QString m_strFilePath = QCoreApplication::applicationDirPath() + QString("/csv/csv%1.csv").arg(k);
        // 判断文件是否不存在
        QFile file(m_strFilePath);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            QTextStream in(&file);
            QString strText("");
            // 文件不存在，第一次，我们就给他写个列表名字，这样csv文件打开时候查看的时候就比较清晰
            strText = QString("T");
            in << strText << ',' << (mean / (f * Target_radius / 3)) << ',' << r_x << ',' << r_y << '\n';
            for(int i = 0; i < RA_.size(); i++)
            {
                in << (RA_.at(i) / (f * Target_radius / 3)) << ',';
            }
            in << '\n';
            file.close();
        }
    }
    qDebug() << "num:" << num;
    return mean;
}
void MainWindow::Cal_circles(std::vector<MyCircles>& preCircles)
{
    float err1 = f * Target_radius * 1.1;
    float err2 = f * Target_radius * 0.9;
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
        double x1 = preCircles.at(k).xc;
        double y1 = preCircles.at(k).yc;
        err1 = f * Target_radius * 0.3;
        float r_x = 0, r_y = 0;
        float mean = Deletestandard_deviation(cirspots.at(k), x1, y1, err1, r_x, r_y, k);
        preCircles.at(k).r = mean;
        if(mean != 0)
        {
            preCircles.at(k).xc = r_x;
            preCircles.at(k).yc = r_y;
        }
        qDebug() << "-------------k << mean << preCircles---------" << k << mean / (f * Target_radius / 3) << preCircles.at(k).xc << preCircles.at(k).yc;
    }
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
        double r = pcircles.at(i).r;
        int pennum = 2;
        painter.setPen(Qt::blue);
        painter.drawText(center, QString::number(r / (f * Target_radius / 3), 'f', 4));
        //画圆
        painter.setPen(QPen(Qt::red, pennum));
        qreal rxxx = 1.0 * r;
        painter.drawEllipse(center, rxxx, rxxx );
    }
    return temp;
}
float MainWindow::normalize(float input, float mid)
{
    //将Omin，Omax上每个数映射到Nmin，Nmax上
    float Nmin, Nmax, Omax, Omin;
    float normalized_x;
    if(input >= mid)
    {
        Nmin = 0;
        Nmax = 255;
        Omax = ui->Max_->text().toFloat() * f * Target_radius / 3;
        Omin = mid;
    }
    else if(input < mid)
    {
        Nmin = -255;
        Nmax = 0;
        Omax = mid;
        Omin = ui->Min_->text().toFloat() * f * Target_radius / 3;
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
        imgpath = QCoreApplication::applicationDirPath() + "/in/3522.png";
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
std::vector<MyCircles> MainWindow::Processing_concentriccircles(std::vector<MyCircles>& preCircles, bool isGray)
{
    vector<MyCircles> resCir;
    float err = Target_radius * f;
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
                if(isGray)
                {
                    x2 = (x1 + x2) / 2.0;
                    y2 = (y1 + y2) / 2.0;
                    r2 = (r1 + r2) / 2.0;
                    inlierRatio2 = (inlierRatio1 + inlierRatio2) / 2.0;
                }
                else
                {
                    if(r1 > r2)
                    {
                        x2 = x1;
                        y2 = y1;
                        r2 = r1;
                        inlierRatio2 = inlierRatio1;
                    }
                }
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
    return resCir;
}
Mat MainWindow::ShowGray(vector<MyCircles>Calgray_Queue)
{
    float mid;
    mid = ui->textEdit->toPlainText().toFloat() * f * Target_radius / 3.0;
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
        float Difference = (r - mid);
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
        float m = normalize(r, mid);
        m =  qAbs(m);
        //qDebug() << r << m;
        Graycolor[3] = m;
        GRcolor[3] = m;
        cv::rectangle(Graycanvas, pt1, pt2, Graycolor, -1);
        cv::rectangle(GRcanvas, pt1, pt2, GRcolor, -1);
    }
    vector<int> compression_params;
    compression_params.push_back(IMWRITE_PNG_COMPRESSION); //PNG格式图像压缩标志
    cv::imwrite(paths.toStdString() + "/img/Graycanvas.png", Graycanvas);
    cv::GaussianBlur(Graycanvas, Graycanvas, Size(0, 0), 40, 0);
    cv::GaussianBlur(Graycanvas, Graycanvas, Size(0, 0), 40, 0);
    cv::imwrite(paths.toStdString() + "/img/GaussianBlur.png", Graycanvas);
    Mat ComGraycanvas(resolution.height(), resolution.width(), CV_8UC4);
    ComGraycanvas = AddAlpha(Graycanvas, 1);
    cv::imwrite(paths.toStdString() + "/img/ComGraycanvas.png", ComGraycanvas);
    Mat ComGRcanvas(resolution.height(), resolution.width(), CV_8UC4);
    ComGRcanvas = AddAlpha(GRcanvas, 0);
    cv::imwrite(paths.toStdString() + "/img/ResGRcanvas.png", ComGRcanvas);
    return ComGraycanvas;
}
void MainWindow::flashqueue()
{
    QPixmap clear = QPixmap();
    QImage temp = MatToImage(queueimages.at(index).imadata);
    switch (index)
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
    QC.cirdata = Processing_concentriccircles(QC.cirdata, false);
    qDebug() << "CircleNum:" << QC.cirdata.size();
    QC.cirdata = Sort_circles(QC.cirdata);
    Cal_circles(QC.cirdata);
    qDebug() << "-------------Cal_circles---------";
    now = DrawCir(temp);
    ui->label_2->setPixmap(QPixmap::fromImage(now).scaled(ui->label_2->width(), ui->label_2->height(), Qt::KeepAspectRatio));
    now.save(paths + "/img/canvas.png");
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
    index = index % 5;
    queueimages.replace(index, QC);
    flashqueue();
    index = index + 1;
    ui->SaveButton->setEnabled(false);
    ui->Calgray->setEnabled(true);
    ui->ExportButton->setEnabled(true);
}
void MainWindow::on_Calgray_clicked()
{
    vector<MyCircles> Calgray_Queue;
    Calgray_Queue.clear();
    for(int i = 0; i < index ; i++)
    {
        for(int j = 0; j < queueimages.at(i).cirdata.size(); j++)
        {
            Calgray_Queue.push_back(queueimages.at(i).cirdata.at(j));
        }
    }
    Calgray_Queue = DeleteSmallCircle(Calgray_Queue);
    Calgray_Queue = Processing_concentriccircles(Calgray_Queue, true);
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
    ui->Max_->setText(QString::number(max / (f * Target_radius / 3)));
    ui->Min_->setText(QString::number(min / (f * Target_radius / 3)));
    Mat y ;
    y = ShowGray(Calgray_Queue);
    qDebug() << "Calgray OK";
}
void MainWindow::on_ExportButton_clicked()
{
    for(int i = 0; i < index; i++)
    {
        QString path = paths + QString("/xml/xml%1.xml").arg(i);
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
void MainWindow::dragEnterEvent(QDragEnterEvent* event) //将文件拖进来的事件
{
    //如果类型是stl文件才能接受拖动。这里的compare字符串比较函数，相等的时候返回0，所以要取反
    if(!event->mimeData()->urls()[0].fileName().right(3).compare("stl"))
    {
        event->acceptProposedAction();
    }
    else
    {
        event->ignore();    //否则不接受鼠标事件
    }
}
void MainWindow::dropEvent(QDropEvent* event)   //放下事件
{
    const QMimeData* qm = event->mimeData(); //获取MIMEData
    QString path  = qm->urls()[0].toLocalFile();	//获取拖入的文件名
    model_temp = Model::createModel(path);
    if(model_temp)
    {
        qDebug() << "Import OK" ;
    }
    tocentre();
    Show_Image();
}
QVector<QVector2D> sort_cirspot(QVector<QVector2D> cirspot, double x1, double y1)
{
    QVector<QVector2D> reQ;
    QVector<QVector2D> c_1, c_2, c_3, c_4;
    for(int i = 0; i < cirspot.size(); i++)
    {
        QVector2D Q = cirspot.at(i);
        if(Q.x() > x1 && Q.y() > y1)
        {
            c_1.append(Q);
        }
        else if(Q.x() < x1 && Q.y() > y1)
        {
            c_2.append(Q);
        }
        else if(Q.x() < x1 && Q.y() < y1)
        {
            c_3.append(Q);
        }
        else if(Q.x() > x1 && Q.y() < y1)
        {
            c_4.append(Q);
        }
    }
    sort(c_1.begin(), c_1.end(), [](QVector2D p1, QVector2D p2)
    {
        return p1.x() < p2.x();
    });
    sort(c_4.begin(), c_4.end(), [](QVector2D p1, QVector2D p2)
    {
        return p1.x() > p2.x();
    });
    sort(c_3.begin(), c_3.end(), [](QVector2D p1, QVector2D p2)
    {
        return p1.x() > p2.x();
    });
    sort(c_2.begin(), c_2.end(), [](QVector2D p1, QVector2D p2)
    {
        return p1.x() < p2.x();
    });
    for(int i = 0; i < c_1.size(); i++)
    {
        reQ.append(c_1.at(i));
    }
    for(int i = 0; i < c_4.size(); i++)
    {
        reQ.append(c_4.at(i));
    }
    for(int i = 0; i < c_3.size(); i++)
    {
        reQ.append(c_3.at(i));
    }
    for(int i = 0; i < c_2.size(); i++)
    {
        reQ.append(c_2.at(i));
    }
    return reQ;
}
void MainWindow::on_pushButton_clicked()
{
}
void myMessageOutput(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
    static QMutex mutex;
    mutex.lock();
    QByteArray localMsg = msg.toLocal8Bit();
    QString strMsg("");
    switch(type)
    {
        case QtDebugMsg:
            strMsg = QString("Debug:");
            break;
        case QtWarningMsg:
            strMsg = QString("Warning:");
            break;
        case QtCriticalMsg:
            strMsg = QString("Critical:");
            break;
        case QtFatalMsg:
            strMsg = QString("Fatal:");
            break;
    }
    // 设置输出信息格式
    QString strMessage = QString("Message:%1 ").arg(localMsg.constData());
    // 输出信息至文件中（读写、追加形式）
    QString logpath = QCoreApplication::applicationDirPath() + QString("/log/log.txt");
    QFile file(logpath);
    file.open(QIODevice::ReadWrite | QIODevice::Append);
    QTextStream stream(&file);
    stream << strMessage << "\r\n";
    file.flush();
    file.close();
    // 解锁
    mutex.unlock();
}
