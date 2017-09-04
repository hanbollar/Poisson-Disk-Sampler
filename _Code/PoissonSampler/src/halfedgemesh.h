#ifndef HALFEDGEMESH_H
#define HALFEDGEMESH_H

#include <QOpenGLContext>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>

#include <drawable.h>
#include "face.h"
#include "halfedge.h"
#include "vertex.h"


class HalfEdgeMesh : public Drawable {
public:
    HalfEdgeMesh(GLWidget277* context);
    HalfEdgeMesh(GLWidget277* context,
                 const std::vector<Face*>& f,
                 const std::vector<HalfEdge*>& h,
                 const std::vector<Vertex*>& v);

    //making cube
    void makeCube();

    virtual GLenum drawMode();

    //storing values
    std::vector<Face*> faces;
    std::vector<HalfEdge*> halfEdges;
    std::vector<Vertex*> vertices;

    //the inherited function
    virtual void create();

};

#endif // HALFEDGEMESH_H
