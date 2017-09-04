#ifndef MYGL_H
#define MYGL_H

#include <glwidget277.h>
#include <utils.h>
#include <shaderprogram.h>
#include <scene/cylinder.h>
#include <scene/sphere.h>
#include "camera.h"

#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>

#include "halfedgemesh.h"
#include "mylistwidget.h"

#include "highlightface.h"
#include "highlightvertex.h"
#include "highlighthedge.h"

class MyGL
    : public GLWidget277
{
    Q_OBJECT
private:
    Cylinder geom_cylinder;// The instance of a unit cylinder we can use to render any cylinder
    Sphere geom_sphere;// The instance of a unit sphere we can use to render any sphere
    ShaderProgram prog_lambert;// A shader program that uses lambertian reflection
    ShaderProgram prog_flat;// A shader program that uses "flat" reflection (no shadowing at all)

    GLuint vao; // A handle for our vertex array object. This will store the VBOs created in our geometry classes.
                // Don't worry too much about this. Just know it is necessary in order to render geometry.

    Camera gl_camera;

    /// Timer linked to timerUpdate(). Fires approx. 60 times per second
    QTimer timer;

    //mesh data variables
    HalfEdgeMesh geom_halfMesh; //-HB
    Vertex* selectedVert; //-HB
    Face* selectedFace; //-HB
    HalfEdge* selectedHEdge; //-HB

    //highlighting variables
    HighlightVertex hVert; //-HB
    HighlightFace hFace; //-HB
    HighlightHEdge hHEdge; //-HB

    //openGL Fun variables
    int timeCount; //-HB
    int renderMode; //-HB
    int posMode; //-HB

public:
    explicit MyGL(QWidget *parent = 0);
    ~MyGL();

    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();

    HalfEdge* getPrevHEdge(HalfEdge* h); //-HB

protected:
    void keyPressEvent(QKeyEvent *e);

    //used when parsing the file for data
    Vertex* parseLineForVertex(std::string); //-HB
    Face* parseLineForFace(std::string,
                           std::vector<Vertex*>&,
                           std::map<Face*,
                           std::vector<Vertex*>>&); //-HB
    void buildHEdges(std::vector<Face*> &,
                     std::vector<HalfEdge*>&,
                     std::map<Face*, std::vector<Vertex*>>&,
                     std::map<Vertex*, std::vector<HalfEdge*>>&,
                     int&);
    void linkSymsForHEdges(std::vector<HalfEdge*>&,
                           std::map<Vertex*, std::vector<HalfEdge*>>&);


private slots:
    /// Slot that gets called ~60 times per second
    void timerUpdate();

public slots:
//    void slot_vertexItemClicked(QListWidgetItem*);
//    void slot_faceItemClicked(QListWidgetItem*);
//    void slot_hEdgeItemClicked(QListWidgetItem*);
//    void slot_xChangedBy(double);
//    void slot_yChangedBy(double);
//    void slot_zChangedBy(double);
//    void slot_redChangedTo(double);
//    void slot_blueChangedTo(double);
//    void slot_greenChangedTo(double);

    void slot_loadFileData(QString);

    void slot_addVertexToCurrentHighlightedEdge();

signals:
    void sig_addHalfEdge(QListWidgetItem*);
    void sig_addVertex(QListWidgetItem*);
    void sig_addFace(QListWidgetItem*);

//    void sig_resetChangePosSpins(double);
//    void sig_resetChangeColSpins(double);
//    void sig_DisplayColorRed(double);
//    void sig_DisplayColorGreen(double);
//    void sig_DisplayColorBlue(double);

//    void sig_halfEdgeChanged(QListWidgetItem*);
//    void sig_faceChanged(QListWidgetItem*);
//    void sig_vertexChanged(QListWidgetItem*);

    void sig_clearList();
};


#endif // MYGL_H
