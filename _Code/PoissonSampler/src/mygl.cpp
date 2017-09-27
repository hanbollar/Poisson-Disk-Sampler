#include "mygl.h"
#include <la.h>

#include <iostream>
#include <stdio.h>
#include <fstream>
#include <string>

#include <QApplication>
#include <QKeyEvent>

#include <QFile>
#include <QFileDialog>
#include <QTextStream>

#include <QXmlStreamReader>
#include <QFileDialog>
#include <QOpenGLTexture>

#include <QDateTime>

#include "samplers/poissonsampler.h"
#include <openGL/drawable.h>
#include <vector>

MyGL::MyGL(QWidget *parent)
    : GLWidget277(parent),
      gl_camera(),
      poissonSampler(nullptr), poissonMesh(nullptr),
      completeSFX(":/include/complete.wav"),
      view_PBVH(false), view_OBJ(true)

{
    // Connect the timer to a function so that when the timer ticks the function is executed
    connect(&timer, SIGNAL(timeout()), this, SLOT(timerUpdate()));
    // Tell the timer to redraw 60 times per second
    timer.start(16);

    this->setFocusPolicy(Qt::StrongFocus);
}

MyGL::~MyGL()
{
    makeCurrent();
    vao.destroy();
}

void MyGL::initializeGL()
{
    // Create an OpenGL context using Qt's QOpenGLFunctions_3_2_Core class
    // If you were programming in a non-Qt context you might use GLEW (GL Extension Wrangler)instead
    initializeOpenGLFunctions();
    // Print out some information about the current OpenGL context
    debugContextVersion();

    // Set a few settings/modes in OpenGL rendering
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POLYGON_SMOOTH);

    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    // Set the size with which points should be rendered
    glPointSize(3);
    // Set the color with which the screen is filled at the start of each render call.
    glClearColor(0.2, 0.2, 0.2, 1);

    printGLErrorLog();

    // Create a Vertex Attribute Object
    vao.create();

    scene.CreateTestScene();
    scene.camera.create();

    prog_lambert.create(":/glsl/lambert.vert.glsl", ":/glsl/lambert.frag.glsl");
    prog_flat.create(":/glsl/flat.vert.glsl", ":/glsl/flat.frag.glsl");

    vao.bind();
}

void MyGL::resizeGL(int w, int h)
{
    //This code sets the concatenated view and perspective projection matrices used for
    //our scene's camera view.
    gl_camera = Camera(w, h);
    glm::mat4 viewproj = gl_camera.GetViewProj();

    // Upload the view-projection matrix to our shaders (i.e. onto the graphics card)
    prog_lambert.setViewProjMatrix(viewproj);
    prog_flat.setViewProjMatrix(viewproj);
    prog_lambert.setModelMatrix(glm::mat4(1.0f));
    prog_flat.setModelMatrix(glm::mat4(1.0f));

    printGLErrorLog();
}

void MyGL::paintGL()
{
    // Clear the screen so that we only see newly drawn images
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Update the viewproj matrix
    prog_lambert.setViewProjMatrix(gl_camera.GetViewProj());
    prog_flat.setViewProjMatrix(gl_camera.GetViewProj());
    GLDrawScene();
}

void MyGL::GLDrawScene()
{
    prog_lambert.setModelMatrix(glm::mat4(1.0f));

    if (poissonSampler == nullptr && poissonMesh != nullptr) {
        if (view_OBJ) {
            prog_lambert.draw(*this, *poissonMesh);
        }
    } else if (poissonSampler != nullptr){

        if (view_OBJ) {
            prog_lambert.draw(*this, *poissonMesh);
        }
        prog_flat.draw(*this, *poissonSampler);
        if (view_PBVH) {
            poissonSampler->bvh->drawAll(*this, prog_flat);
        }
    }

}

void MyGL::timerUpdate()
{
    // This function is called roughly 60 times per second.
    // Use it to perform any repeated actions you want to do,

    timeCount++;
//    prog_lambert.setPosTime(timeCount);
//    prog_lambert.setColTime(timeCount);
}

void MyGL::keyPressEvent(QKeyEvent *e)
{
    float amount = 2.0f;
    if(e->modifiers() & Qt::ShiftModifier){
        amount = 10.0f;
    }

    bool invalid_key = false;

    // http://doc.qt.io/qt-5/qt.html#Key-enum
    if (e->key() == Qt::Key_Escape) {
        QApplication::quit();
    } else if (e->key() == Qt::Key_Right) {
        gl_camera.RotateAboutUp(-amount);
    } else if (e->key() == Qt::Key_Left) {
        gl_camera.RotateAboutUp(amount);
    } else if (e->key() == Qt::Key_Up) {
        gl_camera.RotateAboutRight(-amount);
    } else if (e->key() == Qt::Key_Down) {
        gl_camera.RotateAboutRight(amount);
    } else if (e->key() == Qt::Key_1) {
        gl_camera.fovy += amount;
    } else if (e->key() == Qt::Key_2) {
        gl_camera.fovy -= amount;
    } else if (e->key() == Qt::Key_W) {
        gl_camera.TranslateAlongLook(amount);
    } else if (e->key() == Qt::Key_S) {
        gl_camera.TranslateAlongLook(-amount);
    } else if (e->key() == Qt::Key_D) {
        gl_camera.TranslateAlongRight(amount);
    } else if (e->key() == Qt::Key_A) {
        gl_camera.TranslateAlongRight(-amount);
    } else if (e->key() == Qt::Key_Q) {
        gl_camera.TranslateAlongUp(-amount);
    } else if (e->key() == Qt::Key_E) {
        gl_camera.TranslateAlongUp(amount);
    } else if (e->key() == Qt::Key_F) {
        gl_camera.CopyAttributes(scene.camera);
    } else if (e->key() == Qt::Key_G) {
        scene.camera = Camera(gl_camera);
        scene.camera.recreate();
    } else
    {
        invalid_key = true;
    }

    if (!invalid_key)
    {
        gl_camera.RecomputeAttributes();

        update();  // Calls paintGL, among other things
    }
}

void MyGL::slot_poissonClicked() {
    if (this->poissonMesh != nullptr) {
        bool threeDim = true;

        poissonSampler = new PoissonSampler(*poissonMesh, scene, threeDim);
        poissonSampler->create();
        poissonSampler->bvh->create();
        std::cout<<"numSamples:"<<poissonSampler->numPoints<<std::endl;
    }

    this->update();
}

void MyGL::slot_loadPoissonObj() {
    QString filepath = QFileDialog::getOpenFileName(0, QString("Load Obj"), QString("../scene_files"), tr("*.obj"));
    std::cout<<"filepath:"<<filepath.toStdString()<<std::endl;

    if(filepath.length() == 0)
    {
        return;
    }

    int i = filepath.length() - 1;
    while(QString::compare(filepath.at(i), QChar('/')) != 0)
    {
        i--;
    }
    QStringRef local_path = filepath.leftRef(i+1);
    QStringRef file_name = filepath.rightRef(filepath.length() - 1 - (i));

    Transform t = Transform();
    poissonMesh = new Mesh();
    if (poissonSampler != nullptr) { poissonSampler = nullptr; }
    poissonMesh->LoadOBJ(file_name, local_path, t);
    poissonMesh->create();

    scene.all_mesh.append(poissonMesh);
    this->update();
}

void MyGL::slot_viewPBVH(bool b) {
    view_PBVH = b;
    this->update();
}

void MyGL::slot_viewOBJ(bool b) {
    view_OBJ = b;
    this->update();
}
