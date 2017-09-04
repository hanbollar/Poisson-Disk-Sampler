#ifndef VERTEX_H
#define VERTEX_H

#include "halfedge.h"
#include <la.h>
#include <QListWidgetItem>

//bc cyclical declaration from HalfEdge to Vert and
//  Vert to HalfEdge the following line is included
//  to make sure previously declared
// we use this logic for both declarations below to make
//      sure all are prev declared
class HalfEdge;
class Face;

class Vertex : public QListWidgetItem {
public:
    //for no param constructor - everything set to nullptr
    Vertex();
    Vertex(glm::vec3 p, HalfEdge* e, int i);
    Vertex(glm::vec3 p, HalfEdge* e, int i, std::vector<HalfEdge*>);

    //position of the vertex
    glm::vec3 pos;
    //ONE of the half edges that point to this vertex
    HalfEdge* edge;
    //unique int to identify this vert in menus etc
    int id;
    //vector of all half edges that point to this vertex
    std::vector<HalfEdge*> halfEdgesOfCurrVector;

    void setId(int i);

    void setHEdge(HalfEdge* h);

    //note: we take in a copy of the p vector bc it might be
    //used elsewhere and we want to be sure that when it changes
    //it doesnt affect the current vertex's position
    void setPos(glm::vec3 p);

    //TO PUT IN THESE VALUES - MUST BE SET MANUALLY, NOT PART OF
    //THE CONSTRUCTOR
    void setHEdgeList(std::vector<HalfEdge*>);
    void addHEdgeToList(HalfEdge* h);
};

#endif // VERTEX_H
