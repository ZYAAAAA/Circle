#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <fstream>
#include <QOpenGLWidget>
#include <QVector>
#include <QOpenGLExtraFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QMatrix3x3>
#include <QMatrix4x4>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextCodec>
#include <QTimer>
#include <QTime>
#include <QMutex>


#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <opencv2/opencv.hpp>

#include"dia.h"
#include"model.h"
#include"slicer.h"
#include<iostream>
#include<vector>

struct MyCircles
{
    double xc, yc, r, inlierRatio;
    bool concentric = false;
};


using namespace cv ;

QT_BEGIN_NAMESPACE
namespace Ui
{
    class MainWindow;
}
QT_END_NAMESPACE
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = 0);
    ~MainWindow();
    bool readfile(const std::string& pfile);
    Mat QImage2cvMat(const QImage& image);
    QImage MatToImage(const Mat& m);
    void flashqueue();
    void clean();
    void Show_Image();
    void CalR(const Mat& m);
    QImage DrawCir(Mat& m);
    Mat ShowGray(std::vector<MyCircles>Calgray_Queue);
    std::vector<MyCircles> Sort_circles(std::vector<MyCircles>& preCircles);
    void Cal_circles(std::vector<MyCircles>& preCircles);
    void tocentre();
    float normalize(float input, float mid);
    Mat AddAlpha(const Mat&, int);
    std::vector<MyCircles> Processing_concentriccircles(std::vector<MyCircles>& preCircles, bool isGray);
    std::vector<MyCircles> DeleteSmallCircle(std::vector<MyCircles>& preCircles);
    double Deletestandard_deviation(QVector<QVector2D> cirspot, double x1, double y1, double err1, float& r_x, float& r_y, int k);

    void dragEnterEvent(QDragEnterEvent* event); //拖动进入事件
    void dropEvent(QDropEvent* event);


    int index;

private slots:
    void on_ImportButton_clicked();
    void on_horizontalSlider_ImageIndex_valueChanged(int value);
    void on_CalButton_clicked();
    void on_DefineButton_clicked();
    void on_SaveButton_clicked();
    void on_Calgray_clicked();
    void on_ExportButton_clicked();
    void on_pushButton_clicked();

private:
    Ui::MainWindow* ui;
    Model* model_temp = nullptr;
    QVector<QByteArray> imagesbyte;
    QVector<QVector<QVector2D>> spotbyte;
    QVector<struct Queue_Circle> queueimages;
    QImage NowImages;
    QString paths;
    float f = 1.0f;
    float thinkess;
    float Target_radius = 3;
    QSize resolution = QSize(1920, 1080);
    Dia* d = nullptr;
};

#endif // MAINWINDOW_H
