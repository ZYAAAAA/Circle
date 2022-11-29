#ifndef SLICER_H
#define SLICER_H

#include <QList>
#include <QThread>
#include <QVector>
#include <QVector2D>
#include <QVector3D>
#include <QAtomicInt>
#include <QPainterPath>
#include <vector3.h>
#include <QImage>
#include <QMatrix4x4>


class Sliceable;

class Slicer : public QThread
{
    Q_OBJECT

private:
    struct MLayer
    {
        float z;
        QList<int> index;
        QByteArray data;
        QByteArray spotdata;
        bool isEmpty = false;
    };
    struct Pixinfo //一个像素点经过颜色绘图后的颜色集合
    {
        QList<QRgb> result;
    };
    struct Line
    {
        QVector2D p1, p2;
        bool connected;
    };
    int max_layer;
    QVector<struct MLayer> queue;
    QVector<QVector3D> normal, vertex;

    QVector<QByteArray> images;
    QVector<QByteArray> spot;
public:
    float f = 1.0f;
    float thickness = 1.0f;
    QSize resolution;
    QVector<Sliceable*> items;
    bool isBlackWhite = false;
    int threshold = 128;

public:
    explicit Slicer();
    ~Slicer();
    QVector<QByteArray> resultbyte();
    QVector<QVector<QVector2D>> resultspot();
    void fillnormalandvertex();
    void information(float f, float thickness, QSize resolution);
    bool bisclockwise(QVector<QPointF>& loop);//是否是顺时针，顺时针填充
protected:
    void run() override;
private:
    void clean();
    void cleanAll();
    void groupMLayers(Sliceable* item);
    void dispatchMLayers();
    void processMLayer(struct MLayer& layer);
    QVector2D intersect(QVector3D v1, QVector3D v2, float z);
    void SubtractVoidFromFill(QImage* img, Pixinfo** pixinfo);
};

#endif
