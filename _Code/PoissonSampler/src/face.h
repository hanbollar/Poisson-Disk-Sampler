#ifndef FACE_H
#define FACE_H

#include <la.h>
#include "halfedge.h"
#include <QListWidgetItem>

//bc cyclical declaration from HalfEdge to Face and
//  Face to HalfEdge the following line is included
//  to make sure previously declared
// we use this logic for both declarations below to make
//      sure all are prev declared
class HalfEdge;
class Vertex;

class Face : public QListWidgetItem {
public:
    //for no param constructor
    //- start_edge set to nullptr
    //- vec3 is 0vector
    //- id is -1
    Face();
    Face(HalfEdge* s, glm::vec4 c, int i);

    // the (arbitrary) starting HalfEdge of this Face
    HalfEdge* start_edge;
    // stores the red, green, and blue components of the Face's color
    glm::vec4 color;
    //unique int to identify this face in menus etc
    int id;

    //Setters for the instance variables
    void setStartEdge(HalfEdge* s);
    void setColor(glm::vec4 c);
    void setId(int i);


};

#endif // FACE_H
