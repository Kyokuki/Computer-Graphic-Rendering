#ifndef MYGLWIDGET_H
#define MYGLWIDGET_H

#ifdef MAC_OS
#include <QtOpenGL/QtOpenGL>
#else
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#endif
#include <QtGui>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include "utils.h"

#define MAX_Z_BUFFER 99999999.0f
#define MIN_FLOAT 1e-10f

using namespace glm;

class MyGLWidget : public QOpenGLWidget, protected QOpenGLExtraFunctions{
    Q_OBJECT

public:
    MyGLWidget(QWidget *parent = nullptr);
    ~MyGLWidget();

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int width, int height) override;
    void keyPressEvent(QKeyEvent* e);

private:
    void scene_0();
    void scene_1();
    void drawTriangle(Triangle triangle);
    int edge_walking(FragmentAttr& a, FragmentAttr& b, FragmentAttr& c);
    void bresenham(FragmentAttr& start, FragmentAttr& end, int id);
    void clearBuffer(vec3* now_render_buffer);
    void clearBuffer(int* now_buffer);
    void clearZBuffer(float* now_buffer);
    void clearRecorder(vec3* now_buffer);
    void resizeBuffer(int newW, int newH);
    void updateRecorder(int x, int y);
    vec3 PhongShading(vec3 cur_norm, vec3 cur_pos);

    int WindowSizeH = 0;
    int WindowSizeW = 0;
    int scene_id;
    int degree = 0;

    // buffers
    vec3* edge_recorder;
    vec3* render_buffer;
    vec3* temp_render_buffer;
    float* temp_z_buffer;
    float* z_buffer;
    vec2 offset;

    Model objModel;

    vec3 camPosition;
    vec3 camLookAt;
    vec3 camUp;
    mat4 projMatrix;
    mat4 viewMatrix;
    vec3 lightPosition;
};

#endif // MYGLWIDGET_H