#include "vertex.h"

Vertex::Vertex() : pos(glm::vec3(0.0)), edge(nullptr), id(-1), halfEdgesOfCurrVector() {}

Vertex::Vertex(glm::vec3 p, HalfEdge* e, int i) : pos(p), edge(e), id(i), halfEdgesOfCurrVector() {
    QListWidgetItem::setText(QString::number(i));
}

Vertex::Vertex(glm::vec3 p, HalfEdge* e, int i, std::vector<HalfEdge*> hEdgesList)
    : pos(p), edge(e), id(i), halfEdgesOfCurrVector(hEdgesList) {
    QListWidgetItem::setText(QString::number(i));
}

void Vertex::setId(int i) {
    id = i;
    QListWidgetItem::setText(QString::number(i));
}

void Vertex::setHEdge(HalfEdge* h) {
   edge = h;
}

void Vertex::setPos(glm::vec3 p) {
    pos = p;
}

void Vertex::setHEdgeList(std::vector<HalfEdge*> hList) {
    halfEdgesOfCurrVector = hList;
}

void Vertex::addHEdgeToList(HalfEdge* h) {
    halfEdgesOfCurrVector.push_back(h);
}
