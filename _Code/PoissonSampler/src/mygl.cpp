//#include "mygl.h"
//#include <la.h>

//#include <iostream>
//#include <QApplication>
//#include <QKeyEvent>
//#include <QXmlStreamReader>
//#include <QFileDialog>
//#include <QThreadPool>
//#include <QOpenGLTexture>

//#include <QDateTime>

//#include "samplers/poissonsampler.h"

//#include <openGL/drawable.h>

//#include <vector>

//constexpr float screen_quad_pos[] = {
//    1.0f, -1.0f, 0.0f, 1.0f, 1.0f,
//    1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
//    -1.0f, -1.0f, 0.0f, 0.0f, 1.0f,
//    -1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
//};

//MyGL::MyGL(QWidget *parent)
//    : GLWidget277(parent),
//      sampler(new Sampler(100, 0)),
//      completeSFX(":/include/complete.wav")
//{
//    setFocusPolicy(Qt::ClickFocus);
//    move_rubberband = false;
//    rubberBand = new QRubberBand(QRubberBand::Rectangle, this);
//    rubberBand->setGeometry(0,0,0,0);
//    rubberBand->show();
//    origin = QPoint(0, 0);
//    rubberband_offset = QPoint(0, 0);

//    //-------------------------------------------
//    poissonSampler = nullptr;
//    poissonMesh = nullptr;
//}

//void MyGL::mousePressEvent(QMouseEvent *e)
//{
//    origin = e->pos();
//    if (!rubberBand)
//        rubberBand = new QRubberBand(QRubberBand::Rectangle, this);
//    else rubberBand->hide();
//    rubberBand->setGeometry(QRect(origin, QSize()));
//    rubberBand->show();
//}

//void MyGL::mouseMoveEvent(QMouseEvent *e)
//{
//    rubberBand->setGeometry(QRect(origin, e->pos()).normalized());
//}

//void MyGL::mouseReleaseEvent(QMouseEvent *e)
//{
//    rubberband_offset = e->pos();
//    reorderRect();
//}

//void MyGL::reorderRect() {
//    int tmp_originx = (std::min(origin.x(), rubberband_offset.x()));
//    int tmp_originy = (std::min(origin.y(), rubberband_offset.y()));

//    int tmp_offsetx = (std::max(origin.x(), rubberband_offset.x()));
//    int tmp_offsety = (std::max(origin.y(), rubberband_offset.y()));

//    origin.setX(std::max(tmp_originx, 0));
//    origin.setY(std::max(tmp_originy,0));
//    rubberband_offset.setX(std::min(tmp_offsetx, (int)scene.camera.width));
//    rubberband_offset.setY(std::min(tmp_offsety, (int)scene.camera.height));
//}

//MyGL::~MyGL()
//{
//    makeCurrent();

//    vao.destroy();
//}

//void MyGL::initializeGL()
//{
//    // Create an OpenGL context
//    initializeOpenGLFunctions();
//    // Print out some information about the current OpenGL context
//    debugContextVersion();

//    // Set a few settings/modes in OpenGL rendering
//    glEnable(GL_DEPTH_TEST);
//    glEnable(GL_LINE_SMOOTH);
//    glEnable(GL_POLYGON_SMOOTH);
//    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
//    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);

//    // Enable backface culling
//    //    glEnable(GL_CULL_FACE);
//    //    glCullFace(GL_BACK);
//    //    glFrontFace(GL_CCW);

//    // Set the size with which points should be rendered
//    glPointSize(5);
//    // Set the color with which the screen is filled at the start of each render call.
//    glClearColor(0.5, 0.5, 0.5, 1);

//    printGLErrorLog();

//    // Create a Vertex Attribute Object
//    vao.create();

//    // Create and set up the diffuse shader
//    prog_lambert.create(":/glsl/lambert.vert.glsl", ":/glsl/lambert.frag.glsl");
//    // Create and set up the flat-color shader
//    prog_flat.create(":/glsl/flat.vert.glsl", ":/glsl/flat.frag.glsl");

//    // We have to have a VAO bound in OpenGL 3.2 Core. But if we're not
//    // using multiple VAOs, we can just bind one once.
//    vao.bind();

//    // Create shader program for progressive render view
//    prog_progressive.addShaderFromSourceFile(QOpenGLShader::Vertex  , ":/glsl/renderview.vert.glsl");
//    prog_progressive.addShaderFromSourceFile(QOpenGLShader::Fragment,  ":/glsl/renderview.frag.glsl");
//    prog_progressive_attribute_position = 0;
//    prog_progressive_attribute_texcoord = 1;
//    prog_progressive.bindAttributeLocation("position", prog_progressive_attribute_position);
//    prog_progressive.bindAttributeLocation("uv", prog_progressive_attribute_texcoord);
//    prog_progressive.link();
//    // create full screen quad for progressive rendering
//    glGenBuffers(1, &progressive_position_buffer);
//    glBindBuffer(GL_ARRAY_BUFFER, progressive_position_buffer);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(screen_quad_pos), screen_quad_pos, GL_STATIC_DRAW);
//    glBindBuffer(GL_ARRAY_BUFFER, 0);

//    //Test scene data initialization
//    scene.CreateTestScene();
//    ResizeToSceneCamera();
//    gl_camera.CopyAttributes(scene.camera);
//    rubberband_offset.setX(scene.camera.width);
//    rubberband_offset.setY(scene.camera.height);
//}

//void MyGL::resizeGL(int w, int h)
//{
//    gl_camera = Camera(w, h);

//    glm::mat4 viewproj = gl_camera.GetViewProj();

//    // Upload the projection matrix
//    prog_lambert.setViewProjMatrix(viewproj);
//    prog_flat.setViewProjMatrix(viewproj);

//    printGLErrorLog();
//    gl_camera.CopyAttributes(scene.camera);
//}

//// This function is called by Qt any time your GL window is supposed to update
//// For example, when the function updateGL is called, paintGL is called implicitly.
//void MyGL::paintGL()
//{
//    // Clear the screen so that we only see newly drawn images
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

//    // Update the viewproj matrix
//    prog_lambert.setViewProjMatrix(gl_camera.GetViewProj());
//    prog_flat.setViewProjMatrix(gl_camera.GetViewProj());
//    GLDrawScene();
//}

//void MyGL::GLDrawScene()
//{
//    if (poissonSampler == nullptr && poissonMesh != nullptr) {
//        prog_lambert.setModelMatrix(glm::mat4(1.0f));
//        prog_lambert.draw(*this, *poissonMesh);
//    } else if (poissonSampler = nullptr){
//        prog_lambert.setModelMatrix(glm::mat4(1.0f));
//        prog_lambert.draw(*this, *poissonSampler);
//    }

//    prog_flat.setModelMatrix(glm::mat4(1.0f));
//    prog_flat.draw(*this, scene.camera);

//    //        for (Mesh* m: scene.all_mesh) {
//    //            prog_lambert.setModelMatrix(glm::mat4(1.0f));
//    //            prog_lambert.draw(*this, *m);
//    //        }
//}

//void MyGL::ResizeToSceneCamera()
//{
//    this->setFixedWidth(scene.camera.width);
//    this->setFixedHeight(scene.camera.height);
//    rubberband_offset.setX(scene.camera.width);
//    rubberband_offset.setY(scene.camera.height);
//    //    if(scene.camera.aspect <= 618/432)
//    //    {
//    //        this->setFixedWidth(432 / scene.camera.aspect);
//    //        this->setFixedHeight(432);
//    //    }


////    else
//    //    {
//    //        this->setFixedWidth(618);
//    //        this->setFixedHeight(618 * scene.camera.aspect);
//    //    }
////    gl_camera = Camera(scene.camera);
//}




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
      completeSFX(":/include/complete.wav")

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
    glDeleteVertexArrays(1, &vao);
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
    glPointSize(5);
    // Set the color with which the screen is filled at the start of each render call.
    glClearColor(0.5, 0.5, 0.5, 1);

    printGLErrorLog();

    // Create a Vertex Attribute Object
    glGenVertexArrays(1, &vao);

    //Create the instances of Cylinder and Sphere.
    //geom_cylinder.create();

    //geom_sphere.create();

//    //-HB from here:
//    geom_halfMesh.makeCube();
//    for (HalfEdge* h: geom_halfMesh.halfEdges) {
//        emit sig_addHalfEdge(h);
//    }
//    for (Vertex* v: geom_halfMesh.vertices) {
//        emit sig_addVertex(v);
//    }
//    for (Face* f: geom_halfMesh.faces) {
//        emit sig_addFace(f);
//    }

//    renderMode = 0;
    //-HB to here.

    // Create and set up the diffuse shader
    prog_lambert.create(":/glsl/lambert.vert.glsl", ":/glsl/lambert.frag.glsl");
    // Create and set up the flat lighting shader
    prog_flat.create(":/glsl/flat.vert.glsl", ":/glsl/flat.frag.glsl");

    // We have to have a VAO bound in OpenGL 3.2 Core. But if we're not
    // using multiple VAOs, we can just bind one once.
//    vao.bind();
    glBindVertexArray(vao);
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
    if (poissonSampler == nullptr && poissonMesh != nullptr) {
        prog_lambert.setModelMatrix(glm::mat4(1.0f));
        prog_lambert.draw(*this, *poissonMesh);
    } else if (poissonSampler = nullptr){
        prog_lambert.setModelMatrix(glm::mat4(1.0f));
        prog_lambert.draw(*this, *poissonSampler);
    }

    prog_flat.setModelMatrix(glm::mat4(1.0f));
    prog_flat.draw(*this, scene.camera);

    //        for (Mesh* m: scene.all_mesh) {
    //            prog_lambert.setModelMatrix(glm::mat4(1.0f));
    //            prog_lambert.draw(*this, *m);
    //        }
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
    std::cout<<"starting poisson"<<std::endl;
    if (this->poissonMesh != nullptr) {
        // create poisson on this

        bool threeDim = true;

        poissonSampler = new PoissonSampler(*poissonMesh, scene, threeDim);
        poissonSampler->create();
    }
}

void MyGL::slot_loadPoissonObj() {
    std::cout<<"loading poisson obj"<<std::endl;

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
    poissonMesh->LoadOBJ(file_name, local_path, t);
    poissonMesh->create();

    scene.all_mesh.append(poissonMesh);
    this->update();

    std::cout<<"end: loading poisson obj"<<std::endl;
}
