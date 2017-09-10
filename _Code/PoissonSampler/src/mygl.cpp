#include "mygl.h"
#include <la.h>

#include <iostream>
#include <QApplication>
#include <QKeyEvent>
#include <QXmlStreamReader>
#include <QFileDialog>
#include <QThreadPool>
#include <QOpenGLTexture>


#include <QDateTime>

#include "samplers/poissonsampler.h"

#include <openGL/drawable.h>

#include <vector>

constexpr float screen_quad_pos[] = {
    1.0f, -1.0f, 0.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
    -1.0f, -1.0f, 0.0f, 0.0f, 1.0f,
    -1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
};

MyGL::MyGL(QWidget *parent)
    : GLWidget277(parent),
      sampler(new Sampler(100, 0)),
      completeSFX(":/include/complete.wav")
{
    setFocusPolicy(Qt::ClickFocus);
    move_rubberband = false;
    rubberBand = new QRubberBand(QRubberBand::Rectangle, this);
    rubberBand->setGeometry(0,0,0,0);
    rubberBand->show();
    origin = QPoint(0, 0);
    rubberband_offset = QPoint(0, 0);

    //-------------------------------------------
    poissonSampler = nullptr;
    poissonMesh = nullptr;
}

void MyGL::mousePressEvent(QMouseEvent *e)
{
    origin = e->pos();
    if (!rubberBand)
        rubberBand = new QRubberBand(QRubberBand::Rectangle, this);
    else rubberBand->hide();
    rubberBand->setGeometry(QRect(origin, QSize()));
    rubberBand->show();
}

void MyGL::mouseMoveEvent(QMouseEvent *e)
{
    rubberBand->setGeometry(QRect(origin, e->pos()).normalized());
}

void MyGL::mouseReleaseEvent(QMouseEvent *e)
{
    rubberband_offset = e->pos();
    reorderRect();
}

void MyGL::reorderRect() {
    int tmp_originx = (std::min(origin.x(), rubberband_offset.x()));
    int tmp_originy = (std::min(origin.y(), rubberband_offset.y()));

    int tmp_offsetx = (std::max(origin.x(), rubberband_offset.x()));
    int tmp_offsety = (std::max(origin.y(), rubberband_offset.y()));

    origin.setX(std::max(tmp_originx, 0));
    origin.setY(std::max(tmp_originy,0));
    rubberband_offset.setX(std::min(tmp_offsetx, (int)scene.camera.width));
    rubberband_offset.setY(std::min(tmp_offsety, (int)scene.camera.height));
}

MyGL::~MyGL()
{
    makeCurrent();

    vao.destroy();
}

void MyGL::initializeGL()
{
    // Create an OpenGL context
    initializeOpenGLFunctions();
    // Print out some information about the current OpenGL context
    debugContextVersion();

    // Set a few settings/modes in OpenGL rendering
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POLYGON_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);

    // Enable backface culling
    //    glEnable(GL_CULL_FACE);
    //    glCullFace(GL_BACK);
    //    glFrontFace(GL_CCW);

    // Set the size with which points should be rendered
    glPointSize(5);
    // Set the color with which the screen is filled at the start of each render call.
    glClearColor(0.5, 0.5, 0.5, 1);

    printGLErrorLog();

    // Create a Vertex Attribute Object
    vao.create();

    // Create and set up the diffuse shader
    prog_lambert.create(":/glsl/lambert.vert.glsl", ":/glsl/lambert.frag.glsl");
    // Create and set up the flat-color shader
    prog_flat.create(":/glsl/flat.vert.glsl", ":/glsl/flat.frag.glsl");

    // We have to have a VAO bound in OpenGL 3.2 Core. But if we're not
    // using multiple VAOs, we can just bind one once.
    vao.bind();

    // Create shader program for progressive render view
    prog_progressive.addShaderFromSourceFile(QOpenGLShader::Vertex  , ":/glsl/renderview.vert.glsl");
    prog_progressive.addShaderFromSourceFile(QOpenGLShader::Fragment,  ":/glsl/renderview.frag.glsl");
    prog_progressive_attribute_position = 0;
    prog_progressive_attribute_texcoord = 1;
    prog_progressive.bindAttributeLocation("position", prog_progressive_attribute_position);
    prog_progressive.bindAttributeLocation("uv", prog_progressive_attribute_texcoord);
    prog_progressive.link();
    // create full screen quad for progressive rendering
    glGenBuffers(1, &progressive_position_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, progressive_position_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(screen_quad_pos), screen_quad_pos, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    //Test scene data initialization
    scene.CreateTestScene();
    ResizeToSceneCamera();
    gl_camera.CopyAttributes(scene.camera);
    rubberband_offset.setX(scene.camera.width);
    rubberband_offset.setY(scene.camera.height);
}

void MyGL::resizeGL(int w, int h)
{
    gl_camera = Camera(w, h);

    glm::mat4 viewproj = gl_camera.GetViewProj();

    // Upload the projection matrix
    prog_lambert.setViewProjMatrix(viewproj);
    prog_flat.setViewProjMatrix(viewproj);

    printGLErrorLog();
    gl_camera.CopyAttributes(scene.camera);
}

// This function is called by Qt any time your GL window is supposed to update
// For example, when the function updateGL is called, paintGL is called implicitly.
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
//    for(std::shared_ptr<Drawable> d : scene.drawables)
//    {
//        if (poissonSampler != nullptr) {
//            std::cout<<"in GLDrawScene: poissonSampler is not nullptr"<<std::endl;

//            prog_lambert.setModelMatrix(glm::mat4(1.0f));

//            Drawable* pSampler_drawable = dynamic_cast<Drawable*>(poissonSampler);



//            prog_lambert.draw(*this, *pSampler_drawable);


//        } /*else if (poissonMesh != nullptr) {
//            std::cout<<"in GLDrawScene: poissonMesh is not nullptr"<<std::endl;

//            prog_lambert.setModelMatrix(glm::mat4(1.0f));
//            prog_lambert.draw(*this, *poissonMesh);
//        }*/

//        if(d->drawMode() == GL_TRIANGLES)
//        {
////            if(g->areaLight != nullptr)
////            {
////                prog_flat.setModelMatrix(g->shape->transform.T());
////                prog_flat.draw(*this, *g->shape);
////            }
////            else
////            {


//            // only draw poissonMesh if sampler is nullptr
//            if ((poissonSampler != nullptr) && &(*d.get()) != &(*poissonMesh)) {
//               prog_lambert.setModelMatrix(d->transform.T());
//                prog_lambert.draw(*this, *d);
//            } else if (poissonSampler == nullptr) {
//                prog_lambert.setModelMatrix(d->transform.T());
//                prog_lambert.draw(*this, *d);
//            }


////            }
//        }
//        else if(d->drawMode() == GL_LINES)
//        {
//            prog_flat.setModelMatrix(d->transform.T());
//            prog_flat.draw(*this, *d);
//        } else if (d->drawMode() == GL_POINTS) {
//            prog_flat.setModelMatrix(d->transform.T());
//            prog_flat.draw(*this, *d);
//        }



//    }

    prog_flat.setModelMatrix(glm::mat4(1.0f));
    prog_flat.draw(*this, scene.camera);
}

void MyGL::ResizeToSceneCamera()
{
    this->setFixedWidth(scene.camera.width);
    this->setFixedHeight(scene.camera.height);
    rubberband_offset.setX(scene.camera.width);
    rubberband_offset.setY(scene.camera.height);
    //    if(scene.camera.aspect <= 618/432)
    //    {
    //        this->setFixedWidth(432 / scene.camera.aspect);
    //        this->setFixedHeight(432);
    //    }
    //    else
    //    {
    //        this->setFixedWidth(618);
    //        this->setFixedHeight(618 * scene.camera.aspect);
    //    }
//    gl_camera = Camera(scene.camera);
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

    std::cout<<"NEED TO REWRITE SLOT POISSON CLICKED BECAUSE SCENE.DRAWABLES DOES NOT EXIST ATM"<<std::endl;
//    if (this->scene.drawables.size() > 0) {
//        // create poisson on this

//        bool threeDim = true;

//        poissonSampler = new PoissonSampler(*poissonMesh, scene, threeDim);

////        Drawable* draw =
////        Drawable draw_poisson = *draw;
////        auto poisson_draw = std::make_shared<Mesh>();
////        *(scene.drawables).append(/*dynamic_cast<Drawable*>(poissonSampler)*/ (poisson_draw));
//    }

    std::cout<<"poisson finished being created"<<std::endl;
}

void MyGL::slot_loadPoissonObj() {
    std::cout<<"loading poisson obj"<<std::endl;

//    QString filepath = QFileDialog::getOpenFileName(0, QString("Load Obj"), QString("../scene_files"), tr("*.obj"));
//    std::cout<<"filepath:"<<filepath.toStdString()<<std::endl;

//    if(filepath.length() == 0)
//    {
//        return;
//    }

//    QFile file(filepath);
//    int i = filepath.length() - 1;
//    while(QString::compare(filepath.at(i), QChar('/')) != 0)
//    {
//        i--;
//    }
//    QStringRef local_path = filepath.leftRef(i+1);
////    std::cout<<"local_path:"<<local_path.toString().toStdString()<<std::endl;
//    QStringRef file_name = filepath.rightRef(filepath.length() - 1 - (i));
////    std::cout<<"file name:"<<file_name.toString().toStdString()<<std::endl;

////    QFileInfo fileInfo(filepath.fileName());
////    QStringRef filename(&(fileInfo.fileName()));

////    QString str = QString("");
////    QStringRef filename(&str);

//    Transform t = Transform();
//    poissonMesh = new Mesh();
//    poissonMesh->LoadOBJ(file_name, local_path, t);

    std::cout<<"NEED TO REWRITE SLOT_LOAD POISSON OBJ SINCE SCENE NO LONGER HAS DRAWABLES"<<std::endl;

//    if (scene.drawables.length() > 0) {
//         if (dynamic_cast<Mesh*>(scene.drawables[0].get()) != nullptr) { // first obj is a mesh
//             poissonMesh = (Mesh*)(scene.drawables[0].get());
//         }
//    }

    std::cout<<"end: loading poisson obj"<<std::endl;
}
