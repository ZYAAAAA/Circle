#include "myglwidget.h"
#include <QDebug>
#include <QStringList>
#include <QFile>
#include <QtMath>
#include<QApplication>
#include<QDesktopWidget>
#include <opencv2/imgproc.hpp>
#include <iostream>
using namespace cv;
using namespace std;
MyGLWidget::MyGLWidget(QWidget* parent, QString path)
    : QOpenGLWidget(parent),
      VBO(QOpenGLBuffer::VertexBuffer),
      xtrans(0), ytrans(0), ztrans(0.0),
      verticesCnt(0)//顶点坐标计数
{
    QSurfaceFormat format;
    format.setAlphaBufferSize(24);
    format.setVersion(3, 3);
    format.setSamples(10);//设置重采样次数，用于反走样
    this->setFormat(format);
    if(path.isEmpty())
    {
        path = "F:/STL/FuElssler1.stl";
    }
    model_temp = Model::createModel(path);
    PLATFORM = Model::createModel("F:/STL/PLATFORM4K_1.stl");
    tocentre();
    MovePLATFORM();
    f = 1.0f;
    vertices = loadStl(f);
    PLATFORMvertices = loadPLATFORM(0.7);
}

MyGLWidget::~MyGLWidget()
{
    if(!isValid())
    {
        return;
    }
    model_temp->destroy();
    model_temp = nullptr;
    makeCurrent();
    VAO.destroy();
    VBO.destroy();
    doneCurrent();
}

QVector<float> MyGLWidget::ReturnVertices()
{
    QVector<float> vertices_res;
    if(minz.z() > maxz.z())
    {
        QVector3D xx;
        xx = minz;
        minz = maxz;
        maxz = xx;
    }
    minz.setZ(minz.z() * 2.0 / 3.0);
    maxz.setZ(maxz.z() * 4.0 / 3.0);
    qDebug() << minz.z() << maxz.z();
    for(int i = 0; i < vertices.size(); i = i + 18)
    {
        QVector3D v1, v2, v3, n1;
        v1 = QVector3D(vertices.at(i), vertices.at(i + 1), vertices.at(i + 2));
        v2 = QVector3D(vertices.at(i + 6), vertices.at(i + 1 + 6), vertices.at(i + 2 + 6));
        v3 = QVector3D(vertices.at(i + 12), vertices.at(i + 1 + 12), vertices.at(i + 2 + 12));
        n1 = QVector3D(vertices.at(i + 3), vertices.at(i + 4), vertices.at(i + 5));
        if(v1.z() >= minz.z() && v1.z() <= maxz.z())
        {
            if(v2.z() >= minz.z() && v2.z() <= maxz.z())
            {
                if(v3.z() >= minz.z() && v3.z() <= maxz.z())
                {
                    Normal = {n1.x(), n1.y(), n1.z()};
                    vertices_res.append(v1.x());
                    vertices_res.append(v1.y());
                    vertices_res.append(v1.z());
                    vertices_res.append(Normal);
                    vertices_res.append(v2.x());
                    vertices_res.append(v2.y());
                    vertices_res.append(v2.z());
                    vertices_res.append(Normal);
                    vertices_res.append(v3.x());
                    vertices_res.append(v3.y());
                    vertices_res.append(v3.z());
                    vertices_res.append(Normal);
                }
                else
                {
                    continue;
                }
            }
            else
            {
                continue;
            }
        }
        else
        {
            continue;
        }
    }
    float minx = 10000, miny = 10000, cenz = 10000;
    float maxx = -10000, maxy = -10000;
    for(int i = 0; i < vertices_res.size(); i = i + 6)
    {
        if(minx > vertices_res.at(i))
        {
            minx = vertices_res.at(i);
        }
        if(maxx < vertices_res.at(i))
        {
            maxx = vertices_res.at(i);
        }
        if(miny > vertices_res.at(i + 1))
        {
            miny = vertices_res.at(i + 1);
        }
        if(maxy < vertices_res.at(i + 1))
        {
            maxy = vertices_res.at(i + 1);
        }
        if(cenz > vertices_res.at(i + 2))
        {
            cenz = vertices_res.at(i + 2);
        }
    }
    float cenx = (minx + maxx) / 2.0;
    float ceny = (miny + maxy) / 2.0;
    for(int i = 0; i < vertices_res.size(); i = i + 6)
    {
        vertices_res.replace(i, vertices_res.at(i) - cenx);
        vertices_res.replace(i + 1, vertices_res.at(i + 1) - ceny);
        vertices_res.replace(i + 2, vertices_res.at(i + 2) - cenz);
    }
    return vertices_res;
}

Mesh* MyGLWidget::ReturnMesh()
{
    aiMatrix4x4 TT;
    for(int i = 0; i < 4; i++)
    {
        for(int j = 0; j < 4; j++)
        {
            TT[i][j] = 1;
        }
    }
    Mesh* res_y = new Mesh(TT);
    if(minz.z() > maxz.z())
    {
        QVector3D xx;
        xx = minz;
        minz = maxz;
        maxz = xx;
    }
    minz.setZ(minz.z() * 2.0 / 3.0);
    maxz.setZ(maxz.z() * 4.0 / 3.0);
    qDebug() << minz.z() << maxz.z();
    Mesh* y = this->model_temp->meshes.at(0);
    float minx = 10000, miny = 10000, cenz = 10000;
    float maxx = -10000, maxy = -10000;
    for(int i = 0; i < y->vertices.size(); i = i + 3)
    {
        Vertex v1;
        v1.Position = y->vertices.at(i).Position;
        v1.Normal = y->vertices.at(i).Normal;
        v1.TexCoords = y->vertices.at(i).TexCoords;
        Vertex v2;
        v2.Position = y->vertices.at(i + 1).Position;
        v2.Normal = y->vertices.at(i + 1).Normal;
        v2.TexCoords = y->vertices.at(i + 1).TexCoords;
        Vertex v3;
        v3.Position = y->vertices.at(i + 2).Position;
        v3.Normal = y->vertices.at(i + 2).Normal;
        v3.TexCoords = y->vertices.at(i + 2).TexCoords;
        if(v1.Position.z() >= minz.z() && v1.Position.z() <= maxz.z()
                && v2.Position.z() >= minz.z() && v2.Position.z() <= maxz.z()
                && v3.Position.z() >= minz.z() && v3.Position.z() <= maxz.z())
        {
            res_y->vertices.append(v1);
            res_y->vertices.append(v2);
            res_y->vertices.append(v3);
            res_y->indices.append(i);
            res_y->indices.append(i + 1);
            res_y->indices.append(i + 2);
        }
    }
    for(int i = 0; i < res_y->vertices.size(); i++)
    {
        if(minx >= res_y->vertices.at(i).Position.x())
        {
            minx = res_y->vertices.at(i).Position.x();
        }
        if(maxx <= res_y->vertices.at(i).Position.x())
        {
            maxx = res_y->vertices.at(i).Position.x();
        }
        if(miny >= res_y->vertices.at(i).Position.y())
        {
            miny = res_y->vertices.at(i).Position.y();
        }
        if(maxy <= res_y->vertices.at(i).Position.y())
        {
            maxy = res_y->vertices.at(i).Position.y();
        }
        if(cenz >= res_y->vertices.at(i).Position.z())
        {
            cenz = res_y->vertices.at(i).Position.z();
        }
    }
    float cenx = (minx + maxx) / 2.0;
    float ceny = (miny + maxy) / 2.0;
    for(int i = 0; i < res_y->vertices.size(); i++)
    {
        res_y->vertices[i].Position.setX(res_y->vertices.at(i).Position.x() - cenx);
        res_y->vertices[i].Position.setY(res_y->vertices.at(i).Position.y() - ceny);
        res_y->vertices[i].Position.setZ(res_y->vertices.at(i).Position.z() - cenz);
    }
    return res_y;
}

//渲染OpenGL场景，widget需要更新时调用
void MyGLWidget::initializeGL()
{
    this->initializeOpenGLFunctions();
    shaderprogram.create();
    if(!shaderprogram.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/stl.vert"))
    {
        qDebug() << "ERROR:" << shaderprogram.log();
    }
    if(!shaderprogram.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/stl.frag"))
    {
        qDebug() << "ERROR:" << shaderprogram.log();    //如果编译出错,打印报错信息
    }
    //将添加到此程序的着色器与addshader链接在一起
    if(!shaderprogram.link())
    {
        qDebug() << "ERROR:" << shaderprogram.log();    //如果链接出错,打印报错信息
    }
    VAO.create();
    VAO.bind();
    VBO.create();
    VBO.bind();
    VBO.allocate(vertices.data(), sizeof(float)*vertices.size());
    shaderprogram.setAttributeBuffer("aPos", GL_FLOAT, 0, 3, sizeof(GLfloat) * 6);
    shaderprogram.enableAttributeArray("aPos");
    shaderprogram.setAttributeBuffer("aNormal", GL_FLOAT, sizeof(GLfloat) * 3, 3, sizeof(GLfloat) * 6);
    shaderprogram.enableAttributeArray("aNormal");
    VAO.release();
    VBO.release();
    this->glEnable(GL_DEPTH_TEST);
    view.setToIdentity();
    carpos = QVector3D(0.0f, 0.0f, 20.0f);
    view.lookAt(carpos, QVector3D(0.0f, 0.0f, 0.0f), QVector3D(0.0f, 1.0f, 0.0f));
    RO.setToIdentity();
    //
    //
    //
    PLATFORMshaderprogram.create();
    if(!PLATFORMshaderprogram.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/stl.vert"))
    {
        qDebug() << "ERROR:" << PLATFORMshaderprogram.log();
    }
    if(!PLATFORMshaderprogram.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/stl.frag"))
    {
        qDebug() << "ERROR:" << PLATFORMshaderprogram.log();    //如果编译出错,打印报错信息
    }
    //将添加到此程序的着色器与addshader链接在一起
    if(!PLATFORMshaderprogram.link())
    {
        qDebug() << "ERROR:" << PLATFORMshaderprogram.log();    //如果链接出错,打印报错信息
    }
    PLATFORMVAO.create();
    PLATFORMVAO.bind();
    PLATFORMVBO.create();
    PLATFORMVBO.bind();
    PLATFORMVBO.allocate(PLATFORMvertices.data(), sizeof(float)*PLATFORMvertices.size());
    PLATFORMshaderprogram.setAttributeBuffer("aPos", GL_FLOAT, 0, 3, sizeof(GLfloat) * 6);
    PLATFORMshaderprogram.enableAttributeArray("aPos");
    PLATFORMshaderprogram.setAttributeBuffer("aNormal", GL_FLOAT, sizeof(GLfloat) * 3, 3, sizeof(GLfloat) * 6);
    PLATFORMshaderprogram.enableAttributeArray("aNormal");
    PLATFORMVAO.release();
    PLATFORMVBO.release();
}

//设置OpenGL视口、投影等。
void MyGLWidget::resizeGL(int w, int h)
{
    this->glViewport(0, 0, w, h);
    projection.setToIdentity();
    projection.perspective(60.0f, (GLfloat)w / (GLfloat)h, 0.001f, 100.0f); //视角-宽高比例-zNear-zFar
}

//设置OenGL资源和状态；第一次调用resizeGL/paintGL调用
void MyGLWidget::paintGL()
{
    this->glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    this->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // 框线模式
    shaderprogram.bind();
    //定义参数
    QVector3D lightColor(1.0f, 1.0f, 1.0f);
    QVector3D objectColor(1.0f, 0.0f, 0.0f); //模型颜色
    QVector3D lightdirection(0.0f, -10.0f, -10.0f);
    //传值到小程序
    shaderprogram.setUniformValue("objectColor", objectColor);
    shaderprogram.setUniformValue("lightColor", lightColor);
    shaderprogram.setUniformValue("lightdirection", lightdirection);
    model.setToIdentity();
    model.translate(xtrans, ytrans, ztrans);
    model.rotate(rotation);
    shaderprogram.setUniformValue("view", view);
    shaderprogram.setUniformValue("projection", projection);
    shaderprogram.setUniformValue("model", model);
    int n = vertices.capacity() / sizeof(float);
    //qDebug() << n;//打印stl中三角形的数量（stl是多个三角形组成的文件）
    QOpenGLVertexArrayObject::Binder bind(&VAO);//绑定VAO
    this->glDrawArrays(GL_TRIANGLES, 0, n);
    //
    //
    //
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    PLATFORMshaderprogram.bind();
    //定义参数
    QVector3D PLATFORMobjectColor(1.0f, 1.0f, 0.88f);
    //传值到小程序
    PLATFORMshaderprogram.setUniformValue("objectColor", PLATFORMobjectColor);
    PLATFORMshaderprogram.setUniformValue("lightColor", lightColor);
    PLATFORMshaderprogram.setUniformValue("lightdirection", lightdirection);
    PLATFORMmodel.setToIdentity();
    PLATFORMmodel.translate(xtrans, ytrans, ztrans);
    PLATFORMmodel.rotate(rotation);
    PLATFORMshaderprogram.setUniformValue("view", view);
    PLATFORMshaderprogram.setUniformValue("projection", projection);
    PLATFORMshaderprogram.setUniformValue("model", PLATFORMmodel);
    int PLATFORMn = PLATFORMvertices.capacity() / sizeof(float);
    //qDebug() << n;//打印stl中三角形的数量（stl是多个三角形组成的文件）
    QOpenGLVertexArrayObject::Binder PLATFORMbind(&PLATFORMVAO);//绑定VAO
    this->glDrawArrays(GL_TRIANGLES, 0, PLATFORMn);
}
void MyGLWidget::tocentre()
{
    Mesh* M = model_temp->meshes.at(0);
    float minx = 100000, miny = 1000000, maxx = -10000, maxy = -100000, centrez = 1000000;;
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
        if(centrez > x.Position.z())
        {
            centrez = x.Position.z();
        }
    }
    float centrex = (minx + maxx) / 2.0;
    float centrey = (miny + maxy) / 2.0;
    for(int i = 0; i < M->vertices.size(); i++)
    {
        model_temp->meshes[0]->vertices[i].Position.setX( model_temp->meshes.at(0)->vertices.at(i).Position.x() - centrex);
        model_temp->meshes[0]->vertices[i].Position.setY( model_temp->meshes.at(0)->vertices.at(i).Position.y() - centrey);
        model_temp->meshes[0]->vertices[i].Position.setZ( model_temp->meshes.at(0)->vertices.at(i).Position.z() - centrez);
    }
}

void MyGLWidget::MovePLATFORM()
{
    Mesh* M = PLATFORM->meshes.at(0);
    float minx = 100000, miny = 1000000, maxx = -10000, maxy = -100000, centrez = -1000000;;
    for(int i = 0; i < M->vertices.size(); i++)
    {
        Vertex x = PLATFORM->meshes.at(0)->vertices.at(i);
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
        if(centrez < x.Position.z())
        {
            centrez = x.Position.z();
        }
    }
    float centrex = (minx + maxx) / 2.0;
    float centrey = (miny + maxy) / 2.0;
    for(int i = 0; i < M->vertices.size(); i++)
    {
        PLATFORM->meshes[0]->vertices[i].Position.setX( PLATFORM->meshes.at(0)->vertices.at(i).Position.x() - centrex);
        PLATFORM->meshes[0]->vertices[i].Position.setY( PLATFORM->meshes.at(0)->vertices.at(i).Position.y() - centrey);
        PLATFORM->meshes[0]->vertices[i].Position.setZ( PLATFORM->meshes.at(0)->vertices.at(i).Position.z() - centrez);
    }
}

void MyGLWidget::updateModel(QQuaternion rotation_res)
{
    Mesh* y = this->model_temp->meshes.at(0);
    float minx = 10000, miny = 10000, cenz = 10000;
    float maxx = -10000, maxy = -10000;
    for(int i = 0; i < y->vertices.size(); i++)
    {
        Vertex v1;
        v1.Position = rotation_res.rotatedVector(y->vertices.at(i).Position);
        v1.Normal = rotation_res.rotatedVector(y->vertices.at(i).Normal);
        v1.TexCoords = y->vertices.at(i).TexCoords;
        this->model_temp->meshes[0]->vertices.replace(i, v1);
    }
    tocentre();
    vertices = loadStl(f);
    glBindBuffer(GL_ARRAY_BUFFER, VBO.bufferId());
    glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(vertices[0]), &vertices[0]);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
QVector<float> MyGLWidget::loadStl(float f)
{
    QVector<float> vertices_temp;
    indices.clear();
    for(auto i = 0; i < model_temp->meshes.at(0)->vertices.size(); i++) //读取顶点
    {
        Vertex A = model_temp->meshes.at(0)->vertices.at(i);
        Normal = {A.Normal.x(), A.Normal.y(), A.Normal.z()};
        Position = {A.Position.x()* f, A.Position.y()* f, A.Position.z()* f};
        vertices_temp.append(Position);
        vertices_temp.append(Normal);
        indices.append(model_temp->meshes.at(0)->indices.at(i));
    }
    return vertices_temp;
}

QVector<float> MyGLWidget::loadPLATFORM(float f)
{
    QVector<float> vertices_temp;
    PLATFORMindices.clear();
    for(auto i = 0; i < PLATFORM->meshes.at(0)->vertices.size(); i++) //读取顶点
    {
        Vertex A = PLATFORM->meshes.at(0)->vertices.at(i);
        Normal = {A.Normal.x(), A.Normal.y(), A.Normal.z()};
        Position = {A.Position.x()* f, A.Position.y()* f, A.Position.z()* f};
        vertices_temp.append(Position);
        vertices_temp.append(Normal);
        PLATFORMindices.append(PLATFORM->meshes.at(0)->indices.at(i));
    }
    return vertices_temp;
}
//转换鼠标位置到3维空间
QVector3D MyGLWidget::getViewPos(float mx, float my, QMatrix4x4 pro, QMatrix4x4 view)
{
    float SCR_WIDTH = this->width();
    float SCR_HEIGHT = this->height();
    float x = (2.0f * mx) / SCR_WIDTH - 1.0f;
    float y = 1.0f - (2.0f * my) / SCR_HEIGHT;
    float z = 1.0f;
    QVector3D ray_nds = QVector3D (x, y, z);
    QVector4D ray_clip = QVector4D (ray_nds.x(), ray_nds.y(), ray_nds.z(), 1.0);
    QVector4D ray_eye = pro.inverted() * ray_clip;
    QVector4D ray_world = view.inverted() * ray_eye;
    if (ray_world.w() != 0.0)
    {
        ray_world.setX(ray_world.x() / ray_world.w());
        ray_world.setY(ray_world.y() / ray_world.w());
        ray_world.setZ(ray_world.z() / ray_world.w());
    }
    QVector3D ray_world_xyz = QVector3D(ray_world.x(), ray_world.y(), ray_world.z());
    QVector3D ray_dir = QVector3D(ray_world_xyz - carpos).normalized();
    //qDebug() << "2.mouse =>3D Coord " << ray_dir.x() << ",y:" << ray_dir.y() << ",z:" << ray_dir.z() ;
    return ray_dir;
}
// Determine whether a ray intersect with a triangle
// Parameters
// orig: origin of the ray
// dir: direction of the ray
// v0, v1, v2: vertices of triangle
// t(out): weight of the intersection for the ray
// u(out), v(out): barycentric coordinate of intersection

bool IntersectTriangle(const QVector3D& orig, const QVector3D& dir, QVector3D& v0, QVector3D& v1, QVector3D& v2, float* t, float* u, float* v)
{
    // E1
    QVector3D E1 = v1 - v0;
    // E2
    QVector3D E2 = v2 - v0;
    // P
    QVector3D P = QVector3D::crossProduct(dir, E2);
    // determinant
    float det = QVector3D::dotProduct(E1, P);
    // keep det > 0, modify T accordingly
    QVector3D T;
    if( det > 0 )
    {
        T = orig - v0;
    }
    else
    {
        T = v0 - orig;
        det = -det;
    }
    // If determinant is near zero, ray lies in plane of triangle
    if( det < 0.0001f )
    {
        return false;
    }
    // Calculate u and make sure u <= 1
    *u = QVector3D::dotProduct(T, P);
    if( *u < 0.0f || *u > det )
    {
        return false;
    }
    // Q
    QVector3D Q = QVector3D::crossProduct(T, E1);
    // Calculate v and make sure u + v <= 1
    *v = QVector3D::dotProduct(dir, Q);
    if( *v < 0.0f || *u + *v > det )
    {
        return false;
    }
    // Calculate t, scale parameters, ray intersects triangle
    *t = QVector3D::dotProduct(E2, Q);
    float fInvDet = 1.0f / det;
    *t *= fInvDet;
    *u *= fInvDet;
    *v *= fInvDet;
    return true;
}
bool PointinTriangle(QVector3D A, QVector3D B, QVector3D C, QVector3D P)
{
    QVector3D v0 = C - A ;
    QVector3D v1 = B - A ;
    QVector3D v2 = P - A ;
    float dot00 = QVector3D::dotProduct(v0, v0) ;
    float dot01 = QVector3D::dotProduct(v0, v1) ;
    float dot02 = QVector3D::dotProduct(v0, v2) ;
    float dot11 = QVector3D::dotProduct(v1, v1) ;
    float dot12 = QVector3D::dotProduct(v1, v2) ;
    float inverDeno = 1 / (dot00 * dot11 - dot01 * dot01) ;
    float u = (dot11 * dot02 - dot01 * dot12) * inverDeno ;
    if (u < 0 || u > 1) // if u out of range, return directly
    {
        return false ;
    }
    float v = (dot00 * dot12 - dot01 * dot02) * inverDeno ;
    if (v < 0 || v > 1) // if v out of range, return directly
    {
        return false ;
    }
    return u + v <= 1 ;
}
bool MyGLWidget::hasPickingFace(QVector3D d, QVector3D cameraPos, QVector3D pos, const QVector<unsigned>& indicess, const QVector<float>& vaGrps)
{
    int index = 0;
    QQuaternion temp;
    //  首先遍历顶点索引数组，获取每个面片的顶点位置信息  因为采用的三角网格模型，所以每次遍历3个顶点
    for (int i = 0; i < indicess.size(); i = i + 3)
    {
        //注意因为默认都是模型在(0,0,0)时的坐标，所以都要加上pos，变换到模型所在位置
        index = i;
        QVector3D v1 = rotation.rotatedVector(QVector3D(vaGrps[index * 6], vaGrps[index * 6 + 1], vaGrps[index * 6 + 2])) + pos;
        index = i + 1;
        QVector3D v2 = rotation.rotatedVector(QVector3D(vaGrps[index * 6], vaGrps[index * 6 + 1], vaGrps[index * 6 + 2])) + pos;
        index = i + 2;
        QVector3D v3 = rotation.rotatedVector(QVector3D(vaGrps[index * 6], vaGrps[index * 6 + 1], vaGrps[index * 6 + 2])) + pos;
        float T, U, V;
        if (IntersectTriangle(cameraPos, d, v1, v2, v3, &T, &U, &V))
        {
            QVector3D intpos = cameraPos + QVector3D(d.x() * T, d.y() * T, d.z() * T);
            if (PointinTriangle(v1, v2, v3, intpos))
            {
                index = i;
                QVector3D a = (QVector3D(vaGrps[index * 6], vaGrps[index * 6 + 1], vaGrps[index * 6 + 2]));
                index = i + 1;
                QVector3D b = (QVector3D(vaGrps[index * 6], vaGrps[index * 6 + 1], vaGrps[index * 6 + 2]));
                index = i + 2;
                QVector3D c = (QVector3D(vaGrps[index * 6], vaGrps[index * 6 + 1], vaGrps[index * 6 + 2]));
                if(isStartTest)
                {
                    index = i;
//                //a,b,c为求解出的拟合平面的法向量，是进行归一化处理之后的向量。
                    cv::Point3d plane_norm(vaGrps[index * 6 + 3], vaGrps[index * 6 + 4], vaGrps[index * 6 + 5]);
//                //xy_norm是参考向量，也就是XOY坐标平面的法向量
                    cv::Point3d xy_norm(0.0, 0.0, 1.0);
                    //std::cout << "plane_norm: " << plane_norm << std::endl;
                    //std::cout << "xy_norm: " << xy_norm << std::endl;
                    //求解两个向量的点乘
                    double v1v2 = plane_norm.dot(xy_norm);
//                //计算平面法向量和参考向量的模长，因为两个向量都是归一化之后的，所以这里的结果都是1.
                    double v1_norm = plane_norm.x * plane_norm.x + plane_norm.y * plane_norm.y + plane_norm.z * plane_norm.z;
                    double v2_norm = xy_norm.x * xy_norm.x + xy_norm.y * xy_norm.y + xy_norm.z * xy_norm.z;
//                //计算两个向量的夹角
                    double theta  = std::acos(v1v2 / (std::sqrt(v1_norm) * std::sqrt(v2_norm)));
//                //根据向量的叉乘求解同时垂直于两个向量的法向量 归一化
                    double a1 = plane_norm.x;
                    double b1 = plane_norm.y;
                    double c1 = plane_norm.z;
                    double a2 = xy_norm.x;
                    double b2 = xy_norm.y;
                    double c2 = xy_norm.z;
                    cv::Point3d axis_v1v2 = cv::Point3d(b1 * c2 - b2 * c1, a2 * c1 - a1 * c2, a1 * b2 - a2 * b1);
                    double v1v2_2 = axis_v1v2.x * axis_v1v2.x + axis_v1v2.y * axis_v1v2.y + axis_v1v2.z * axis_v1v2.z;
                    double v1v2_n = std::sqrt(v1v2_2);
                    axis_v1v2 = axis_v1v2 / v1v2_n;
                    //std::cout << "axis_v1v2: " << axis_v1v2 << std::endl;
                    //std::cout << "theta: " << theta << std::endl;
                    if(plane_norm.x == 0 && plane_norm.y == 0)
                    {
                        temp = QQuaternion::fromEulerAngles(0, 0, 0);
                    }
                    else
                    {
                        temp = QQuaternion::fromAxisAndAngle(axis_v1v2.x, axis_v1v2.y, axis_v1v2.z, theta * 180 / M_PI);
                    }
                    ;
                    updateModel(temp);
                    qDebug() << temp;
                    //qDebug() << a << b << c;
                }
                if(isSelectMM == 1)
                {
                    minz = ((a.z() < b.z()) ? (a.z() < c.z() ? a : c) : (b.z() < c.z() ? b : c));
                    qDebug() << "-----minz------" << minz;
                }
                else if(isSelectMM == 2)
                {
                    maxz = ((a.z() > b.z()) ? (a.z() > c.z() ? a : c) : (b.z() > c.z() ? b : c));
                    qDebug() << "-----maxz------" << maxz;
                }
                return true;
            }
        }
    }
    //遍历所有面片仍未返回说明这个网格与鼠标无交点，返回false
    return false;
}
void MyGLWidget::mousePressEvent(QMouseEvent* event)
{
    makeCurrent();
    mousePos = QVector2D(event->pos());
    if(isStartTest | isSelectMM)
    {
        QVector3D x = getViewPos(mousePos.x(),  mousePos.y(),  projection,  view);
        if(hasPickingFace(x, carpos, QVector3D(xtrans, ytrans, ztrans), indices, vertices))
        {
            // qDebug() << "-----select------";
        }
        else
        {
            qDebug() << "---NO--select------";
        }
    }
    this->update();
    event->accept();
    doneCurrent();
}

void MyGLWidget::mouseMoveEvent(QMouseEvent* event)
{
    if(event->buttons() == Qt::LeftButton)
    {
        QVector2D newPos = (QVector2D)event->pos();
        QVector2D diff = newPos - mousePos;
        qreal angle = (diff.length()) / 3.6;
        QVector3D rotationAxis = QVector3D(diff.y(), diff.x(), 0.0).normalized();
        rotation = QQuaternion::fromAxisAndAngle(rotationAxis, angle) * rotation;
        mousePos = newPos;
    }
    if(event->buttons() == Qt::RightButton)
    {
        QVector2D newPos = (QVector2D)event->pos();
        QVector2D diff = newPos - mousePos;
        xtrans -= diff.x() * 0.01;
        ytrans += diff.y() * 0.01;
        mousePos = newPos;
    }
    this->update();
    event->accept();
}
//鼠标滚轮缩放模型
void MyGLWidget::wheelEvent(QWheelEvent* event)
{
    QPoint numDegrees = event->angleDelta() / 8;
    if(numDegrees.y() > 0)
    {
        ztrans += (0.25f * 5);
    }
    else if(numDegrees.y() < 0)
    {
        ztrans -= (0.25f * 5);
    }
    this->update();
    event->accept();
}

