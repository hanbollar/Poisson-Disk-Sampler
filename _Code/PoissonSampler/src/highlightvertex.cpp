#include "highlightvertex.h"

HighlightVertex::HighlightVertex(GLWidget277* context)
    : Drawable(context), currentVertex(nullptr) {}

void HighlightVertex::setCurrentVertex(Vertex* v) {
    currentVertex = v;
}

GLenum HighlightVertex::drawMode() {
    return GL_POINTS;
}

void HighlightVertex::create() {
    //create new vbo
    //set drawmode to GL_POINTS
    //only drawing for one vertex
    //  line indices. first in line is prevVert, second in line is currentVert
    std::vector<GLuint> pointIndex;
    //  pos vec4s
    std::vector<glm::vec4> posVector;
    //  color vec4s
    std::vector<glm::vec4> colVector;
    //note: pos, and color vectors need to be 1:1 to eachother

    glm::vec4 currVertPos = glm::vec4(currentVertex->pos[0], currentVertex->pos[1], currentVertex->pos[2], 1.0f);

    pointIndex.push_back(0);

    colVector.push_back(glm::vec4(255.0f, 255.0f, 255.0f, 1.0f) / 255.0f); //white for current vert
    posVector.push_back(currVertPos);

    count = pointIndex.size();

    //  handling line indices vbo
    generateIdx();
    context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufIdx);
    context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, pointIndex.size() * sizeof(GLuint), pointIndex.data(), GL_STATIC_DRAW);

    //  handling of pos vec3s vbo
    generatePos();
    context->glBindBuffer(GL_ARRAY_BUFFER, bufPos);
    context->glBufferData(GL_ARRAY_BUFFER, posVector.size() * sizeof(glm::vec4), posVector.data(), GL_STATIC_DRAW);

    //  handling of color vbo
    generateCol();
    context->glBindBuffer(GL_ARRAY_BUFFER, bufCol);
    context->glBufferData(GL_ARRAY_BUFFER, colVector.size() * sizeof(glm::vec4), colVector.data(), GL_STATIC_DRAW);
}
