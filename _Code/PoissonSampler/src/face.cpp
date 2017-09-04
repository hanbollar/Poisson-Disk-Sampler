#include "face.h"

//Constructors

Face::Face()
    : start_edge(nullptr), color(glm::vec4(0.0)), id(-1) {}

Face::Face(HalfEdge *s, glm::vec4 c, int i)
    : start_edge(s), color(c), id(i) {
    QListWidgetItem::setText(QString::number(i));
}

//Setters for the instance variables

void Face::setStartEdge(HalfEdge* s) {
    start_edge = s;
}

void Face::setColor(glm::vec4 c) {
    color = c;
}

void Face::setId(int i) {
    id = i;
    QListWidgetItem::setText(QString::number(i));
}
