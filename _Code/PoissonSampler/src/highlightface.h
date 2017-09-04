#ifndef HIGHLIGHTFACE_H
#define HIGHLIGHTFACE_H

#include <drawable.h>

#include "halfedge.h"
#include "vertex.h"
#include "face.h"

#include <QOpenGLContext>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>


class HighlightFace : public Drawable
{
public:
    HighlightFace(GLWidget277* context);

    Face* currentFace;

    void setCurrentFace(Face* f);

    virtual GLenum drawMode();
    virtual void create();
};

#endif // HIGHLIGHTFACE_H
