#include "halfedgemesh.h"
#include <iostream>

HalfEdgeMesh::HalfEdgeMesh(GLWidget277* context)
    : Drawable(context),
      faces(), halfEdges(), vertices()
{}

HalfEdgeMesh::HalfEdgeMesh(GLWidget277 *context,
                           const std::vector<Face*>& f,
                           const std::vector<HalfEdge*>& h,
                           const std::vector<Vertex*>& v)
    : Drawable(context),
      faces(f), halfEdges(h), vertices(v)
{}

GLenum HalfEdgeMesh::drawMode() {
    return GL_TRIANGLES;
}

void HalfEdgeMesh::makeCube() {

    //Setting up Faces:
    Face* fBase = new Face();
    fBase->setColor(glm::vec4(255.0f, 0.0f, 0.0f, 1.0f)); //RED
    fBase->setId(0);

    Face* fRight = new Face();
    fRight->setColor(glm::vec4(230.0f, 0.0f, 255.0f, 1.0f)); //PURPLE
    fRight->setId(1);

    Face* fLeft = new Face();
    fLeft->setColor(glm::vec4(0.0f, 255.0f, 0.0f, 1.0f)); //GREEN
    fLeft->setId(2);

    Face* fFront = new Face();
    fFront->setColor(glm::vec4(0.0f, 0.0f, 255.0f, 1.0f)); //BLUE
    fFront->setId(3);

    Face* fTop = new Face();
    fTop->setColor(glm::vec4(200.0f, 160.0f, 215.0f, 1.0f)); //PINK
    fTop->setId(4);

    Face* fBack = new Face();
    fBack->setColor(glm::vec4(240.0f, 255.0f, 0.0f, 1.0f)); //YELLOW
    fBack->setId(5);

    //Setting up vertices
    Vertex* v0 = new Vertex(glm::vec3(0.5f, -0.5f, -0.5f), nullptr, 0);
    Vertex* v1 = new Vertex(glm::vec3(0.5f, -0.5f, 0.5f), nullptr, 1);
    Vertex* v2 = new Vertex(glm::vec3(-0.5f, -0.5f, 0.5f), nullptr, 2);
    Vertex* v3 = new Vertex(glm::vec3(-0.5f, -0.5f, -0.5f), nullptr, 3);
    Vertex* v4 = new Vertex(glm::vec3(0.5f, 0.5f, -0.5f), nullptr, 4);
    Vertex* v5 = new Vertex(glm::vec3(0.5f, 0.5f, 0.5f), nullptr, 5);
    Vertex* v6 = new Vertex(glm::vec3(-0.5f, 0.5f, 0.5f), nullptr, 6);
    Vertex* v7 = new Vertex(glm::vec3(-0.5f, 0.5f, -0.5f), nullptr, 7);

    //BASE
    HalfEdge* h0 = new HalfEdge(fBase, v0, nullptr, nullptr, 0);
    HalfEdge* h1 = new HalfEdge(fBase, v1, nullptr, nullptr, 1);
    HalfEdge* h2 = new HalfEdge(fBase, v2, nullptr, nullptr, 2);
    HalfEdge* h3 = new HalfEdge(fBase, v3, nullptr, nullptr, 3);
    //RIGHT
    HalfEdge* h4 = new HalfEdge(fRight, v4, nullptr, nullptr, 4);
    HalfEdge* h5 = new HalfEdge(fRight, v5, nullptr, nullptr, 5);
    HalfEdge* h6 = new HalfEdge(fRight, v1, nullptr, nullptr, 6);
    HalfEdge* h7 = new HalfEdge(fRight, v0, nullptr, nullptr, 7);
    //LEFT
    HalfEdge* h8 = new HalfEdge(fLeft, v2, nullptr, nullptr, 8);
    HalfEdge* h9 = new HalfEdge(fLeft, v3, nullptr, nullptr, 9);
    HalfEdge* h10 = new HalfEdge(fLeft, v7, nullptr, nullptr, 10);
    HalfEdge* h11 = new HalfEdge(fLeft, v6, nullptr, nullptr, 11);
    //FRONT
    HalfEdge* h12 = new HalfEdge(fFront, v1, nullptr, nullptr, 12);
    HalfEdge* h13 = new HalfEdge(fFront, v5, nullptr, nullptr, 13);
    HalfEdge* h14 = new HalfEdge(fFront, v6, nullptr, nullptr, 14);
    HalfEdge* h15 = new HalfEdge(fFront, v2, nullptr, nullptr, 15);
    //TOP
    HalfEdge* h16 = new HalfEdge(fTop, v5, nullptr, nullptr, 16);
    HalfEdge* h17 = new HalfEdge(fTop, v4, nullptr, nullptr, 17);
    HalfEdge* h18 = new HalfEdge(fTop, v7, nullptr, nullptr, 18);
    HalfEdge* h19 = new HalfEdge(fTop, v6, nullptr, nullptr, 19);
    //BACK
    HalfEdge* h20 = new HalfEdge(fBack, v4, nullptr, nullptr, 20);
    HalfEdge* h21 = new HalfEdge(fBack, v0, nullptr, nullptr, 21);
    HalfEdge* h22 = new HalfEdge(fBack, v3, nullptr, nullptr, 22);
    HalfEdge* h23 = new HalfEdge(fBack, v7, nullptr, nullptr, 23);

    //adding all half edges that point to a specific vertex to that
    //  vertex's vector of them
    v0->addHEdgeToList(h0);
    v0->addHEdgeToList(h7);
    v0->addHEdgeToList(h21);
    v1->addHEdgeToList(h1);
    v1->addHEdgeToList(h6);
    v1->addHEdgeToList(h12);
    v2->addHEdgeToList(h2);
    v2->addHEdgeToList(h8);
    v2->addHEdgeToList(h15);
    v3->addHEdgeToList(h3);
    v3->addHEdgeToList(h9);
    v3->addHEdgeToList(h22);
    v4->addHEdgeToList(h4);
    v4->addHEdgeToList(h17);
    v4->addHEdgeToList(h20);
    v5->addHEdgeToList(h5);
    v5->addHEdgeToList(h13);
    v5->addHEdgeToList(h16);
    v6->addHEdgeToList(h11);
    v6->addHEdgeToList(h19);
    v6->addHEdgeToList(h14);
    v7->addHEdgeToList(h10);
    v7->addHEdgeToList(h18);
    v7->addHEdgeToList(h23);

    //Add Vertices to HalfEdges (the vertex the half edge points to)
    //BASE
    h0->setHEdgeNext(h1);
    h1->setHEdgeNext(h2);
    h2->setHEdgeNext(h3);
    h3->setHEdgeNext(h0);
    //RIGHT
    h4->setHEdgeNext(h5);
    h5->setHEdgeNext(h6);
    h6->setHEdgeNext(h7);
    h7->setHEdgeNext(h4);
    //LEFT
    h8->setHEdgeNext(h11);
    h9->setHEdgeNext(h8);
    h10->setHEdgeNext(h9);
    h11->setHEdgeNext(h10);
    //FRONT
    h12->setHEdgeNext(h13);
    h13->setHEdgeNext(h14);
    h14->setHEdgeNext(h15);
    h15->setHEdgeNext(h12);
    //TOP
    h16->setHEdgeNext(h17);
    h17->setHEdgeNext(h18);
    h18->setHEdgeNext(h19);
    h19->setHEdgeNext(h16);
    //BACK
    h20->setHEdgeNext(h21);
    h21->setHEdgeNext(h22);
    h22->setHEdgeNext(h23);
    h23->setHEdgeNext(h20);

    //setting syms
    //BOT
    h0->setHEdgeSym(h22);
    h1->setHEdgeSym(h7);
    h2->setHEdgeSym(h12);
    h3->setHEdgeSym(h8);
    //RIGHT
    h4->setHEdgeSym(h21);
    h5->setHEdgeSym(h17);
    h6->setHEdgeSym(h13);
    h7->setHEdgeSym(h1);
    //LEFT
    h8->setHEdgeSym(h3);
    h9->setHEdgeSym(h15);
    h10->setHEdgeSym(h19);
    h11->setHEdgeSym(h23);
    //FRONT
    h12->setHEdgeSym(h2);
    h13->setHEdgeSym(h6);
    h14->setHEdgeSym(h16);
    h15->setHEdgeSym(h9);
    //TOP
    h16->setHEdgeSym(h14);
    h17->setHEdgeSym(h5);
    h18->setHEdgeSym(h20);
    h19->setHEdgeSym(h10);
    //BACK
    h20->setHEdgeSym(h18);
    h21->setHEdgeSym(h4);
    h22->setHEdgeSym(h0);
    h23->setHEdgeSym(h11);

    //Add Faces to HalfEdges (the face this half edge is part of) - already done

    //Add HalfEdges to Faces (1 as starter for each face)
    fBase->setStartEdge(h0);
    fRight->setStartEdge(h4);
    fLeft->setStartEdge(h8);
    fFront->setStartEdge(h12);
    fTop->setStartEdge(h16);
    fBack->setStartEdge(h20);

    //Add HalfEdges to Vertices (1 of the half edges that points to this vertex)
    v0->setHEdge(h0);
    v1->setHEdge(h1);
    v2->setHEdge(h2);
    v3->setHEdge(h3);
    v4->setHEdge(h4);
    v5->setHEdge(h7);
    v6->setHEdge(h9);
    v7->setHEdge(h10);

    //ADDING VALUES TO CURRENT MESH
    //faces
    faces.push_back(fBase);
    faces.push_back(fRight);
    faces.push_back(fLeft);
    faces.push_back(fFront);
    faces.push_back(fTop);
    faces.push_back(fBack);
    //halfEdges
    halfEdges.push_back(h0);
    halfEdges.push_back(h1);
    halfEdges.push_back(h2);
    halfEdges.push_back(h3);
    halfEdges.push_back(h4);
    halfEdges.push_back(h5);
    halfEdges.push_back(h6);
    halfEdges.push_back(h7);
    halfEdges.push_back(h8);
    halfEdges.push_back(h9);
    halfEdges.push_back(h10);
    halfEdges.push_back(h11);
    halfEdges.push_back(h12);
    halfEdges.push_back(h13);
    halfEdges.push_back(h14);
    halfEdges.push_back(h15);
    halfEdges.push_back(h16);
    halfEdges.push_back(h17);
    halfEdges.push_back(h18);
    halfEdges.push_back(h19);
    halfEdges.push_back(h20);
    halfEdges.push_back(h21);
    halfEdges.push_back(h22);
    halfEdges.push_back(h23);
    //vertices
    vertices.push_back(v0);
    vertices.push_back(v1);
    vertices.push_back(v2);
    vertices.push_back(v3);
    vertices.push_back(v4);
    vertices.push_back(v5);
    vertices.push_back(v6);
    vertices.push_back(v7);

    //NOW CALLING CREATE TO ACTUALLY DRAW THE CUBE
    create();
}

void HalfEdgeMesh::create() {
    //set up all these std::vectors
    //  triangle indices
    std::vector<GLuint> triIndices;
    //  pos vec4s
    std::vector<glm::vec4> posVector;
    //  normal vec4s
    std::vector<glm::vec4> normVector;
    //  color vec4s
    std::vector<glm::vec4> colVector;
    //note: pos, normal, and color vectors need to be 1:1 to eachother

    int indexOffSet = 0;

    for (Face* f : faces) {
        HalfEdge* firstEdge = f->start_edge;

        //POSITIONS, NORMALS, and COLORS
        //setting up variables to begin making position vec3s and normal vec3s for current face
        HalfEdge *currEdge = firstEdge;
        Vertex *currVert = (currEdge->vert);
        HalfEdge *prevEdge = currEdge;
        while (prevEdge->next != firstEdge) { //looping around to find prev edge for firstEdge
            prevEdge = (prevEdge->next);
        }
        Vertex *prevVert = prevEdge->vert;
        Vertex *nextVert = currEdge->next->vert;

        glm::vec3 v1 = currVert->pos - prevVert->pos;
        glm::vec3 v2 = currVert->pos - nextVert->pos;
        glm::vec3 norm = glm::cross(v1, v2);
        glm::vec4 normVec = glm::vec4(norm[0],
                                      norm[1],
                                      norm[2],
                                      0);

        bool firstLoop = true;

        while(firstLoop || currEdge != firstEdge) {
            firstLoop = false;

            //converting vec3s into vec4s so correct for the vbos
            glm::vec4 posVec = glm::vec4(currVert->pos[0],
                                         currVert->pos[1],
                                         currVert->pos[2],
                                         1);

            //std::cout << "currentVertex id: " << currVert->id << "pos: " << glm::to_string(currVert->pos) << std::endl;


            //storing
            posVector.push_back(posVec);
            normVector.push_back(normVec);
            //color also stored here to maintain 1:1
            colVector.push_back((f->color)/255.0f);

            //now shifting all values for next pos and norm values
            prevEdge = currEdge;
            currEdge = currEdge->next;
            prevVert = prevEdge->vert;
            currVert = currEdge->vert;
            nextVert = currEdge->next->vert;
        }

        //TRIANGLES
        //setting up variables to begin triangulating current face

        int count = 1;
        HalfEdge* current = firstEdge->next;

        //counting number of vertices for this face
        while (current != firstEdge) { //while next is not the first edge
            count++;
            current = current->next; //step to next edge
        }

        //making triangles for current face
        int first = 0;
        int second = 1;
        int third = 2;

        while (third < count) {
            triIndices.push_back(first + indexOffSet);
            triIndices.push_back(second + indexOffSet);
            triIndices.push_back(third + indexOffSet);

            second = third;
            third++;
        }

        indexOffSet += count;
    }


    count = triIndices.size();

    //  handling triangle indices vbo
    generateIdx();
    context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufIdx);
    context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, triIndices.size() * sizeof(GLuint), triIndices.data(), GL_STATIC_DRAW);

    //  handling of pos vec3s vbo
    generatePos();
    context->glBindBuffer(GL_ARRAY_BUFFER, bufPos);
    context->glBufferData(GL_ARRAY_BUFFER, posVector.size() * sizeof(glm::vec4), posVector.data(), GL_STATIC_DRAW);

    //  handling of norm vec3s vbo
    generateNor();
    context->glBindBuffer(GL_ARRAY_BUFFER, bufNor);
    context->glBufferData(GL_ARRAY_BUFFER, normVector.size() * sizeof(glm::vec4), normVector.data(), GL_STATIC_DRAW);

    //  handling of color vbo
    generateCol();
    context->glBindBuffer(GL_ARRAY_BUFFER, bufCol);
    context->glBufferData(GL_ARRAY_BUFFER, colVector.size() * sizeof(glm::vec4), colVector.data(), GL_STATIC_DRAW);
}
