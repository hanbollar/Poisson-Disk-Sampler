#include "highlightface.h"

HighlightFace::HighlightFace(GLWidget277* context)
    : Drawable(context), currentFace(nullptr) {}

void HighlightFace::setCurrentFace(Face* h) {
    currentFace = h;
}

GLenum HighlightFace::drawMode() {
    return GL_LINES;
}

void HighlightFace::create() {

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

    HalfEdge* tempCurr = currentFace->start_edge;
    glm::vec4 oppColOfFace = glm::vec4(255.0f - currentFace->color[0],
                                       255.0f - currentFace->color[1],
                                       255.0f - currentFace->color[2],
                                       1.0f);

    HalfEdge* iterEdge = tempCurr->next;
    int vCount = 1;
    while (iterEdge != tempCurr) {
        iterEdge = iterEdge->next;
        vCount += 1;
    }

    //making the lines
    for (int i=0; i < vCount*2-1; i++) {
        //FINDING VALUES FOR CURRENT LINE:
        HalfEdge* curr = tempCurr;
        Vertex* currVert = curr->vert;
        //to find the prevVert
        bool first = true;
        while (first || curr->next != tempCurr) {
            first = false;
            curr = curr->next;
        }
        Vertex* prevVert = curr->vert;

        //MAKING ITEMS TO PUSH
        glm::vec4 currVertPos = glm::vec4(currVert->pos[0], currVert->pos[1], currVert->pos[2], 1.0f);
        glm::vec4 prevVertPos = glm::vec4(prevVert->pos[0], prevVert->pos[1], prevVert->pos[2], 1.0f);

        lineIndices.push_back(i);
        lineIndices.push_back(i+1);
        posVector.push_back(prevVertPos);
        posVector.push_back(currVertPos);

        colVector.push_back(oppColOfFace); //col for for prev vert
        colVector.push_back(oppColOfFace); //col for current vert

        //for next line:
        tempCurr = tempCurr->next;
    }

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
