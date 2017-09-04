#ifndef HIGHLIGHTHEDGE_H
#define HIGHLIGHTHEDGE_H

#include <drawable.h>

#include "halfedge.h"
#include "vertex.h"
#include "face.h"

#include <QOpenGLContext>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>

class HighlightHEdge : public Drawable
{
public:
    HighlightHEdge(GLWidget277* context);

    HalfEdge* currentEdge;

    void setCurrentEdge(HalfEdge* h);

    virtual GLenum drawMode();
    virtual void create();
};

#endif // HIGHLIGHTHEDGE_H
