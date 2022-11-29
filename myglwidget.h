#ifndef MYGLWIDGET_H
#define MYGLWIDGET_H

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
#include <QOpenGLFunctions_3_3_Core>
#include "model.h"

class MyGLWidget : public QOpenGLWidget, public QOpenGLExtraFunctions
{
    Q_OBJECT
public:
    explicit MyGLWidget(QWidget* parent = nullptr, QString path = "");
    ~MyGLWidget();
public:
    bool isStartTest = false;
    int isSelectMM = 0;
    QVector<unsigned int> indices;
    uint32_t triangleNum;
    QVector3D minz, maxz;
    Model* model_temp = nullptr;
    QVector<float> ReturnVertices();
    Mesh* ReturnMesh();

protected:
    void initializeGL();
    void resizeGL(int width, int height);
    void paintGL();
    void tocentre();
    void MovePLATFORM();
    void updateModel(QQuaternion);
    QVector<float> loadStl(float f);
    QVector<float> loadPLATFORM(float f);
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void wheelEvent(QWheelEvent* event);//图片缩放功能
    QVector3D getViewPos(float x, float y, QMatrix4x4 pro, QMatrix4x4 view);
    bool hasPickingFace(QVector3D d, QVector3D cameraPos, QVector3D pos, const QVector<unsigned>&, const QVector<float>&);
private:
    QVector<float> vertices;
    QVector<float> Position;
    QVector<float> Normal;//读文件时的两个临时变量顶点位置、法向量
    float f;

    QOpenGLShaderProgram shaderprogram;
    QOpenGLVertexArrayObject VAO;
    QOpenGLBuffer VBO;

    QMatrix4x4 model;
    QMatrix4x4 view;
    QMatrix4x4 projection;
    QMatrix3x3 RO;
    QVector3D carpos;

    int verticesCnt;
    GLfloat xtrans, ytrans, ztrans;
    QVector2D mousePos;
    QQuaternion rotation;

    Model* PLATFORM = nullptr;
    QVector<float> PLATFORMvertices;
    QOpenGLShaderProgram PLATFORMshaderprogram;
    QOpenGLVertexArrayObject PLATFORMVAO;
    QOpenGLBuffer PLATFORMVBO;
    QMatrix4x4 PLATFORMmodel;
    QVector<unsigned int> PLATFORMindices;

};

#endif
