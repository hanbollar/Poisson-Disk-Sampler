#ifndef HALFEDGE_H
#define HALFEDGE_H

#include "halfedge.h"
#include "face.h"
#include "vertex.h"
#include <la.h>
#include <QListWidgetItem>

//bc cyclical declaration from HalfEdge to Vert and
//      Vert to HalfEdge the following line is included
//      to make sure previously declared
// we use this logic for both declarations below to make
//      sure all are prev declared
class Vertex;
class Face;

class HalfEdge : public QListWidgetItem {
public:
    //for no param constructor - everything set to nullptr
    HalfEdge();
    HalfEdge(Face* f, Vertex* v, HalfEdge* n, HalfEdge* s, int i);

    // Face to the left of this HalfEdge
    Face* face;
    // Vertex between this HalfEdge and the next HalfEdge in the ring
    Vertex* vert;
    // next HalfEdge in the loop of HalfEdges that make up this HalfEdge's Face
    HalfEdge* next;
    //HalfEdge that lies parallel to this HalfEdge and which travels in the opposite
    //  direction and is part of an adjacent Face
    HalfEdge* sym;
    // unique int to identify this HalfEdge for menus and the like
    int id;

    //Setters for the instance variables
    void setFace(Face* f);
    void setVert(Vertex* v);
    void setHEdgeNext(HalfEdge* n);
    void setHEdgeSym(HalfEdge* s);
    void setID(int i);
};

#endif // HALFEDGE_H
