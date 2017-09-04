#ifndef HIGHLIGHTVERTEX_H
#define HIGHLIGHTVERTEX_H

#include <drawable.h>

#include "halfedge.h"
#include "vertex.h"
#include "face.h"

#include <QOpenGLContext>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>


class HighlightVertex : public Drawable
{
public:
    HighlightVertex(GLWidget277* context);

    Vertex* currentVertex;

    void setCurrentVertex(Vertex* h);

    virtual GLenum drawMode();
    virtual void create();

};

#endif // HIGHLIGHTVERTEX_H
