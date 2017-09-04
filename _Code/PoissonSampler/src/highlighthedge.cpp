#include "highlighthedge.h"

HighlightHEdge::HighlightHEdge(GLWidget277* context)
    : Drawable(context), currentEdge(nullptr) {}

void HighlightHEdge::setCurrentEdge(HalfEdge* h) {
    currentEdge = h;
}

GLenum HighlightHEdge::drawMode() {
    return GL_LINES;
}

void HighlightHEdge::create() {
    //create new vbo
    //set drawmode to GL_Line
    //should interpolate from one vertex col value to other
    //only need to put in starting vertex and ending vertex so ya

    //  line indices. first in line is prevVert, second in line is currentVert
    std::vector<GLuint> lineIndices;
    //  pos vec4s
    std::vector<glm::vec4> posVector;
    //  color vec4s
    std::vector<glm::vec4> colVector;
    //note: pos, and color vectors need to be 1:1 to eachother

    HalfEdge* curr = currentEdge;
    Vertex* currVert = curr->vert;
    //to find the prevVert
    bool first = true;
    while (first || curr->next != currentEdge) {
        first = false;
        curr = curr->next;
    }
    Vertex* prevVert = curr->vert;

    glm::vec4 currVertPos = glm::vec4(currVert->pos[0], currVert->pos[1], currVert->pos[2], 1.0f);
    glm::vec4 prevVertPos = glm::vec4(prevVert->pos[0], prevVert->pos[1], prevVert->pos[2], 1.0f);

    lineIndices.push_back(0);
    lineIndices.push_back(1);
    posVector.push_back(prevVertPos);
    posVector.push_back(currVertPos);

    colVector.push_back(glm::vec4(255.0f, 0.0f, 0.0f, 1.0f) / 255.0f); //red for prev vert
    colVector.push_back(glm::vec4(255.0f, 255.0f, 0.0f, 1.0f) / 255.0f); //yellow for current vert

    count = lineIndices.size();

    //  handling line indices vbo
    generateIdx();
    context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufIdx);
    context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, lineIndices.size() * sizeof(GLuint), lineIndices.data(), GL_STATIC_DRAW);

    //  handling of pos vec3s vbo
    generatePos();
    context->glBindBuffer(GL_ARRAY_BUFFER, bufPos);
    context->glBufferData(GL_ARRAY_BUFFER, posVector.size() * sizeof(glm::vec4), posVector.data(), GL_STATIC_DRAW);

    //  handling of color vbo
    generateCol();
    context->glBindBuffer(GL_ARRAY_BUFFER, bufCol);
    context->glBufferData(GL_ARRAY_BUFFER, colVector.size() * sizeof(glm::vec4), colVector.data(), GL_STATIC_DRAW);
}
