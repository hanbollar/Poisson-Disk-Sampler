#pragma once

#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
#include <QRubberBand>
#include <QMouseEvent>

#include <openGL/glwidget277.h>
#include <la.h>
#include <openGL/shaderprogram.h>
#include <scene/camera.h>
#include <scene/scene.h>
#include <la.h>

#include <QTimer>
#include <QTime>
#include <QSound>

#include "samplers/poissonsampler.h"
#include <QStringRef>
#include <QFile>

class QOpenGLTexture;

class MyGL
    : public GLWidget277
{
    Q_OBJECT
private:
    QOpenGLVertexArrayObject vao;

    ShaderProgram prog_lambert;
    ShaderProgram prog_flat;

    Camera gl_camera;//This is a camera we can move around the scene to view it from any angle.
                                //However, the camera defined in the config file is the one from which the scene will be rendered.
                                //If you move gl_camera, you will be able to see the viewing frustum of the scene's camera.

    Scene scene;
    Sampler* sampler;

    QString output_filepath;

    QSound completeSFX;

    QTime renderTimer;

    PoissonSampler* poissonSampler;
    Mesh* poissonMesh;

public:
    explicit MyGL(QWidget *parent = 0);
    ~MyGL();

    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();

    void GLDrawScene();
    void ResizeToSceneCamera();

private:
    QRubberBand *rubberBand;
    bool move_rubberband;
    QPoint rubberband_offset;
    QPoint origin;
    bool progressive_render = true;
    bool something_rendered = false;

    QOpenGLShaderProgram prog_progressive;
    int prog_progressive_attribute_position;
    int prog_progressive_attribute_texcoord;

    GLuint progressive_position_buffer;
    QOpenGLTexture* progressive_texture = nullptr;

protected:
    void keyPressEvent(QKeyEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void reorderRect();


public slots:

    void slot_poissonClicked();
    void slot_loadPoissonObj();

signals:
    void sig_ResizeToCamera(int,int);
    void sig_DisableGUI(bool);
};
