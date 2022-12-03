#include <QApplication>
#include "mainwindow.h"
int main(int argc, char* argv[])
{
    Mat Graycanvas = cv::imread("E:/C++/Debug/build-DebugTest-Desktop_Qt_5_11_1_MSVC2017_64bit-Debug/4K.png");
    cv::GaussianBlur(Graycanvas, Graycanvas, Size(3, 3), 2, 0);
    cv::imwrite("E:/C++/Debug/build-DebugTest-Desktop_Qt_5_11_1_MSVC2017_64bit-Debug/Graycanvas.png", Graycanvas);
    //test
    QApplication a(argc, argv);
    MainWindow x;
    x.show();
    return a.exec();
}
