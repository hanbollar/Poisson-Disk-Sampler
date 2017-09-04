#include "mygl.h"
#include <la.h>

#include <iostream>
#include <stdio.h>
#include <fstream>
#include <string>

#include <QApplication>
#include <QKeyEvent>

#include <QFile>
#include <QFileDialog>
#include <QTextStream>

#include "face.h"
#include "vertex.h"
#include "halfedge.h"

class Face;
class Vertex;
class HalfEdge;

MyGL::MyGL(QWidget *parent)
    : GLWidget277(parent),
      geom_cylinder(this), geom_sphere(this),
      prog_lambert(this), prog_flat(this),
      gl_camera(),
      //-HB: below
      geom_halfMesh(this),
      selectedVert(nullptr), selectedFace(nullptr), selectedHEdge(nullptr),
      hVert(this), hFace(this), hHEdge(this),
      timeCount(0), renderMode(0), posMode(0)

{
    // Connect the timer to a function so that when the timer ticks the function is executed
    connect(&timer, SIGNAL(timeout()), this, SLOT(timerUpdate()));
    // Tell the timer to redraw 60 times per second
    timer.start(16);

    this->setFocusPolicy(Qt::StrongFocus);
}

MyGL::~MyGL()
{
    makeCurrent();
    glDeleteVertexArrays(1, &vao);
    //geom_cylinder.destroy();
    //geom_sphere.destroy();
    geom_halfMesh.destroy(); //-HB
    hVert.destroy(); //-HB
    hFace.destroy(); //-HB
    hHEdge.destroy(); //-HB
}

void MyGL::initializeGL()
{
    // Create an OpenGL context using Qt's QOpenGLFunctions_3_2_Core class
    // If you were programming in a non-Qt context you might use GLEW (GL Extension Wrangler)instead
    initializeOpenGLFunctions();
    // Print out some information about the current OpenGL context
    debugContextVersion();

    // Set a few settings/modes in OpenGL rendering
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POLYGON_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    // Set the size with which points should be rendered
    glPointSize(5);
    // Set the color with which the screen is filled at the start of each render call.
    glClearColor(0.5, 0.5, 0.5, 1);

    printGLErrorLog();

    // Create a Vertex Attribute Object
    glGenVertexArrays(1, &vao);

    //Create the instances of Cylinder and Sphere.
    //geom_cylinder.create();

    //geom_sphere.create();

    //-HB from here:
    geom_halfMesh.makeCube();
    for (HalfEdge* h: geom_halfMesh.halfEdges) {
        emit sig_addHalfEdge(h);
    }
    for (Vertex* v: geom_halfMesh.vertices) {
        emit sig_addVertex(v);
    }
    for (Face* f: geom_halfMesh.faces) {
        emit sig_addFace(f);
    }

    renderMode = 0;
    //-HB to here.

    // Create and set up the diffuse shader
    prog_lambert.create(":/glsl/lambert.vert.glsl", ":/glsl/lambert.frag.glsl");
    // Create and set up the flat lighting shader
    prog_flat.create(":/glsl/flat.vert.glsl", ":/glsl/flat.frag.glsl");

    // Set a color with which to draw geometry since you won't have one
    // defined until you implement the Node classes.
    // This makes your geometry render green.
    prog_lambert.setGeometryColor(glm::vec4(0,1,0,1));

    // We have to have a VAO bound in OpenGL 3.2 Core. But if we're not
    // using multiple VAOs, we can just bind one once.
//    vao.bind();
    glBindVertexArray(vao);
}

void MyGL::resizeGL(int w, int h)
{
    //This code sets the concatenated view and perspective projection matrices used for
    //our scene's camera view.
    gl_camera = Camera(w, h);
    glm::mat4 viewproj = gl_camera.getViewProj();

    // Upload the view-projection matrix to our shaders (i.e. onto the graphics card)

    prog_lambert.setViewProjMatrix(viewproj);
    prog_flat.setViewProjMatrix(viewproj);
    prog_lambert.setModelMatrix(glm::mat4(1.0f));
    prog_flat.setModelMatrix(glm::mat4(1.0f));

    printGLErrorLog();
}

//This function is called by Qt any time your GL window is supposed to update
//For example, when the function updateGL is called, paintGL is called implicitly.
//DO NOT CONSTRUCT YOUR SCENE GRAPH IN THIS FUNCTION!
void MyGL::paintGL()
{
    // Clear the screen so that we only see newly drawn images
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    prog_flat.setViewProjMatrix(gl_camera.getViewProj());
    prog_lambert.setViewProjMatrix(gl_camera.getViewProj());

#define NOPE
#ifdef NOPE
    //Create a model matrix. This one scales the sphere uniformly by 3, then translates it by <-2,0,0>.
    //Note that we have to transpose the model matrix before passing it to the shader
    //This is because OpenGL expects column-major matrices, but you've
    //implemented row-major matrices.
   /* glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(-2,0,0)) * glm::scale(glm::mat4(1.0f), glm::vec3(3,3,3));
    //Send the geometry's transformation matrix to the shader
    prog_lambert.setModelMatrix(model);
    //Draw the example sphere using our lambert shader
    prog_lambert.draw(geom_sphere);
*/
    //Now do the same to render the cylinder
    //We've rotated it -45 degrees on the Z axis, then translated it to the point <2,2,0>
  /*  model = glm::translate(glm::mat4(1.0f), glm::vec3(2,2,0)) * glm::rotate(glm::mat4(1.0f), glm::radians(-45.0f), glm::vec3(0,0,1));
    prog_lambert.setModelMatrix(model);
    prog_lambert.draw(geom_cylinder);
*/
//    prog_lambert.setModelMatrix(glm::mat4(1.0));

//    prog_lambert.draw(geom_cylinder);]
    prog_lambert.draw(geom_halfMesh);

    glDisable(GL_DEPTH_TEST);
    prog_flat.draw(hVert);
    prog_flat.draw(hFace);
    prog_flat.draw(hHEdge);
    glEnable(GL_DEPTH_TEST);
#endif
}


void MyGL::keyPressEvent(QKeyEvent *e)
{

    float amount = 2.0f;
    if(e->modifiers() & Qt::ShiftModifier){
        amount = 10.0f;
    }
    // http://doc.qt.io/qt-5/qt.html#Key-enum
    // This could all be much more efficient if a switch
    // statement were used, but I really dislike their
    // syntax so I chose to be lazy and use a long
    // chain of if statements instead
    if (e->key() == Qt::Key_Escape) {
        QApplication::quit();
    } else if (e->key() == Qt::Key_Right) {
        gl_camera.RotateAboutUp(-amount);
    } else if (e->key() == Qt::Key_Left) {
        gl_camera.RotateAboutUp(amount);
    } else if (e->key() == Qt::Key_Up) {
        gl_camera.RotateAboutRight(-amount);
    } else if (e->key() == Qt::Key_Down) {
        gl_camera.RotateAboutRight(amount);
    } else if (e->key() == Qt::Key_1) {
        gl_camera.fovy += amount;
    } else if (e->key() == Qt::Key_2) {
        gl_camera.fovy -= amount;
    } else if (e->key() == Qt::Key_W) {
        gl_camera.TranslateAlongLook(amount);
    } else if (e->key() == Qt::Key_S) {
        gl_camera.TranslateAlongLook(-amount);
    } else if (e->key() == Qt::Key_D) {
        gl_camera.TranslateAlongRight(amount);
    } else if (e->key() == Qt::Key_A) {
        gl_camera.TranslateAlongRight(-amount);
    } else if (e->key() == Qt::Key_Q) {
        gl_camera.TranslateAlongUp(-amount);
    } else if (e->key() == Qt::Key_E) {
        gl_camera.TranslateAlongUp(amount);
    } else if (e->key() == Qt::Key_R) {
//        gl_camera = Camera(this->width(), this->height()); //- old

        //-HB
//        if (renderMode == 0) {
//            renderMode = 1;
//            prog_lambert.setRenderMode(renderMode);
//        } else {
//            renderMode = 0;
//            prog_lambert.setRenderMode(renderMode);
//        }
        //-endHB
    }

    //--HB FROM HERE
    //N: currhalfedge: currhalfedge's next
//    if (e->key() == Qt::Key_N) {
//        if (selectedHEdge != nullptr) {
//            selectedHEdge = selectedHEdge->next;
//            emit sig_halfEdgeChanged(selectedHEdge);
//            hHEdge.setCurrentEdge(selectedHEdge);
//            hHEdge.create();
//        }
    //M: currhalfedge: currhalfedge's sym
//    } else if (e->key() == Qt::Key_M) {
//        if (selectedHEdge != nullptr) {
//            selectedHEdge = selectedHEdge->sym;
//            emit sig_halfEdgeChanged(selectedHEdge);
//            hHEdge.setCurrentEdge(selectedHEdge);
//            hHEdge.create();
//        }
    //F: currface: currhalfedge's face
//    } else if (e->key() == Qt::Key_F) {
//        if (selectedHEdge != nullptr) {
//            selectedFace = selectedHEdge->face;
//            emit sig_faceChanged(selectedFace);
//            hFace.setCurrentFace(selectedFace);
//            hFace.create();
//        }
    //V: currvertex:  currehalfedge's vert
//    } else if (e->key() == Qt::Key_V) {
//        if (selectedHEdge != nullptr) {
//            selectedVert = selectedHEdge->vert;
//            emit sig_vertexChanged(selectedHEdge);
//            hVert.setCurrentVertex(selectedVert);
//            hVert.create();
//        }
    //H: currhalfedge: currvert's half edge
//    } else if (e->key() == Qt::Key_H && amount == 2.0f) {
//        if (selectedVert != nullptr) {
//            selectedHEdge = selectedVert->edge;
//            emit sig_halfEdgeChanged(selectedHEdge);
//            hHEdge.setCurrentEdge(selectedHEdge);
//            hHEdge.create();
//        }
    //shftH: currhalfedge: currface's half edge
//    } else if (e->key() == Qt::Key_H && amount == 10.f) {
//        if (selectedFace != nullptr) {
//            selectedHEdge = selectedFace->start_edge;
//            emit sig_halfEdgeChanged(selectedHEdge);
//            hHEdge.setCurrentEdge(selectedHEdge);
//            hHEdge.create();
//        }
//    }

    //P: load up another object from the given objects folder
    if (e->key() == Qt::Key_P) {
        QString file = QFileDialog::getOpenFileName(
                    this,
                    tr("Opening one of Hannah's OBJ Files"),
                    "../../../../hw05/_objFiles/givenOBJ", tr("OBJ Files (*.obj)"));
        slot_loadFileData(file);
    //O: load up another object from the folder of my object data
    } else if (e->key() == Qt::Key_O) {
        QString file = QFileDialog::getOpenFileName(
                    this,
                    tr("Opening one of Hannah's OBJ Files"),
                    "../../../../hw05/_objFiles/myOBJ", tr("OBJ Files (*.obj)"));
        slot_loadFileData(file);
    }

    //Z: position mode turns on or off
//    if (e->key() == Qt::Key_Z) {
//        std::cout<<"z was pressed"<<std::endl;
//        if (posMode == 0) {
//            posMode = 1;
//            prog_lambert.setPosMode(posMode);
//        } else {
//            posMode = 0;
//            prog_lambert.setPosMode(posMode);
//        }
//    }
    //--HB TO HERE

    gl_camera.RecomputeAttributes();
    update();  // Calls paintGL, among other things
}

void MyGL::timerUpdate()
{
    // This function is called roughly 60 times per second.
    // Use it to perform any repeated actions you want to do,
    // such as BLAH BLAH AND BLAHDIBLAH

    timeCount++;
    prog_lambert.setPosTime(timeCount);
    prog_lambert.setColTime(timeCount);
    if (renderMode == 1) {
        this->update();
    }
    if (posMode == 1) {
        this->update();
    }
}

//void MyGL::slot_vertexItemClicked(QListWidgetItem *it) {
//    if (it != selectedVert) {
//        selectedVert = ((Vertex*)(it));
//        hVert.currentVertex = selectedVert;
//        hVert.create();
//        this->update();

//        emit sig_resetChangePosSpins(0.0);
//    }
//}

//void MyGL::slot_hEdgeItemClicked(QListWidgetItem *it) {
//    if (it != selectedHEdge) {
//        selectedHEdge = ((HalfEdge*)(it));
//        hHEdge.currentEdge = selectedHEdge;
//        hHEdge.create();
//        this->update();
//    }
//}

//void MyGL::slot_faceItemClicked(QListWidgetItem *it) {
//    if (it != selectedFace) {
//        selectedFace = ((Face*)(it));
//        hFace.currentFace = selectedFace;
//        hFace.create();
//        this->update();

//        emit sig_DisplayColorRed(selectedFace->color[0]);
//        emit sig_DisplayColorGreen(selectedFace->color[1]);
//        emit sig_DisplayColorBlue(selectedFace->color[2]);
//    }
//}

//void MyGL::slot_xChangedBy(double x) {
//    if (selectedVert == nullptr) {
//        selectedVert = geom_halfMesh.vertices[0];
//        hVert.currentVertex = selectedVert;
//        hVert.create();
//        this->update();
//    }
//    selectedVert->pos[0] += x;

//    geom_halfMesh.create();
//    hVert.currentVertex = selectedVert;
//    hVert.create();

//    this->update();
//}

//void MyGL::slot_yChangedBy(double y) {
//    if (selectedVert == nullptr) {
//        selectedVert = geom_halfMesh.vertices[0];
//        hVert.currentVertex = selectedVert;
//        hVert.create();
//        this->update();
//    }
//    selectedVert->pos[1] += y;

//    geom_halfMesh.create();
//    hVert.currentVertex = selectedVert;
//    hVert.create();

//    this->update();
//}

//void MyGL::slot_zChangedBy(double z) {
//    if (selectedVert == nullptr) {
//        selectedVert = geom_halfMesh.vertices[0];
//        hVert.currentVertex = selectedVert;
//        hVert.create();
//        this->update();
//    }
//    selectedVert->pos[2] += z;

//    geom_halfMesh.create();
//    hVert.currentVertex = selectedVert;
//    hVert.create();

//    this->update();
//}

//void MyGL::slot_redChangedTo(double x) {
//    if (selectedFace == nullptr) {
//        selectedFace = geom_halfMesh.faces[0];
//        hFace.currentFace = selectedFace;
//        hFace.create();
//        this->update();
//    }
//    if (x <= 255 and x >= 0) {
//        selectedFace->color[0] = x;

//        geom_halfMesh.create();
//        hFace.currentFace = selectedFace;
//        hFace.create();

//        this->update();
//    }
//}

//void MyGL::slot_greenChangedTo(double x) {
//    if (selectedFace == nullptr) {
//        selectedFace = geom_halfMesh.faces[0];
//        hFace.currentFace = selectedFace;
//        hFace.create();
//        this->update();
//    }
//    if (x <= 255 and x >= 0) {
//        selectedFace->color[1] = x;

//        geom_halfMesh.create();
//        hFace.currentFace = selectedFace;
//        hFace.create();
//        this->update();
//    }
//}

//void MyGL::slot_blueChangedTo(double x) {
//    if (selectedFace == nullptr) {
//        selectedFace = geom_halfMesh.faces[0];
//        hFace.currentFace = selectedFace;
//        hFace.create();
//        this->update();
//    }
//    if (x <= 255 and x >= 0) {
//        selectedFace->color[2] = x;

//        geom_halfMesh.create();
//        hFace.currentFace = selectedFace;
//        hFace.create();
//        this->update();
//    }
//}


void MyGL::slot_loadFileData(QString filePath) {
    //SETTING UP VARIABLES
    //filler variable used when checking if an item is a digit
    std::locale locale;
    //int id variables: note they all start at 1 bc obj files index starting from 1
    int vertexId = 1;
    int faceId = 1;
    int halfEdgeId = 1;
    //variables to read data from the files
    std::string fPath = filePath.toStdString();
    std::string line = "";

    //SETTING UP VECTORS
    //setting up vectors for vertices, faces, and halfedges
    std::vector<Vertex*> vertices;
    std::vector<Face*> faces;
    std::vector<HalfEdge*> hEdges;

    //SETTING UP MAPS
    //setting up map of face to all vertices for making halfEdges
    std::map<Face*, std::vector<Vertex*>> mapFaceToAllVertices;
    //setting up map of vertex to all half edges that point to it (for setting up syms later)
    std::map<Vertex*, std::vector<HalfEdge*>> mapVertexToAllHEdges;

    //SETTING UP STREAM TO PARSE FILE
    std::ifstream myFile (fPath); //opens file instream

    //iterating through all the lines in the file
    while (std::getline(myFile, line)) {
        //note: for the following code - c++ strings check equality with x.compare(y)
        //if line has at least 2 elements in it - then we can see if first let and second let are
        //as follows [as per the formatting]
        if (line.size() > 0) {
            std::string firstLet = line.substr(0, 1);
//            std::cout<<"firstLet is:"<<firstLet<<std::endl;

            if (line.size() > 1) {
                std::string secondLet = line.substr(1, 2);
//                std::cout<<"secondLet is:"<<secondLet<<std::endl;

                //setting up variables for parsing check for usable data

                //we trim spaces and tabs
                const char* t = " \t"; //the values we want to trim off
                firstLet = firstLet.erase(0, firstLet.find_first_not_of(t)); //trimming front
                firstLet = firstLet.erase(firstLet.find_last_not_of(t) + 1); //trimming back
                secondLet = secondLet.erase(0, secondLet.find_first_not_of(t)); //trimming front
                secondLet = secondLet.erase(secondLet.find_last_not_of(t) + 1); //trimming back

                //now parsing check: checking current line for usable data

                //VERTEX DATA
                //if first character in the line is v add it and following data to a vertex vector
                if ((firstLet.at(0) == 'v') && (std::isdigit(secondLet[0], locale) || secondLet.at(0) == ('-'))) {
//                    std::cout<<"FIRST: "<<line<<std::endl;

                    //PARSE for vertex
                    Vertex* v = parseLineForVertex(line);
                    v->setId(vertexId);

                    //ADD VERTEX to vector of vertices
                    vertices.push_back(v);

                    //INCREMENT vertexId so next vertex we see will have the next id num
                    vertexId += 1;

                //FACE DATA
                //if first character in the line is f add it and following data to a face vector
                //in below compare: starting for second let (vertex id for face) with 1 bc .obj files index from 1
                } else if (firstLet.at(0) == 'f' && std::isdigit(secondLet[0], locale) && secondLet.at(0) != '0') {
//                    std::cout<<"SECOND: "<<line<<std::endl;

                    //PARSE for face
                    Face* face = parseLineForFace(line, vertices, mapFaceToAllVertices);
                    face->setId(faceId);
                    if (face->id <= 0) {
                        std::cout<<"FACE ID WAS:"<<face->id<<" BUT SHOULD HAVE BEEN FACEIDNUM:"<<faceId<<std::endl;
                    }

                    //ADD FACE to vector of vertices
                    faces.push_back(face);

                    //INCREMENT faceId so next face we see will have the next id num
                    faceId += 1;

                //just for checking later to see if specific lines were skipped
                } else {
//                    std::cout<<"line not printed: was skipped"<<std::endl;
//                    std::cout<<line<<std::endl;

                }//END of: FACE DATA AND VERTEX DATA (not syms)
            }//END of: if (line.size() > 1)
        }//END of: if (line.size() > 0) {
    }//END of: while (std::getline(myFile, line))

    myFile.close(); //closes file instream

    buildHEdges(faces, hEdges,  mapFaceToAllVertices, mapVertexToAllHEdges, halfEdgeId);
    linkSymsForHEdges(hEdges, mapVertexToAllHEdges);

    //if all vectors have data then load up new mesh
    //otherwise dont update to any new mesh
    if (vertices.size() != 0 && faces.size() != 0 && hEdges.size() != 0) {
        //give data to mesh, send to vbos, and update
        geom_halfMesh.vertices = vertices;
        geom_halfMesh.faces = faces;
        geom_halfMesh.halfEdges = hEdges;

        geom_halfMesh.create();
        emit sig_clearList();
        for (HalfEdge* h: geom_halfMesh.halfEdges) {
            emit sig_addHalfEdge(h);
        }
        for (Vertex* v: geom_halfMesh.vertices) {
            emit sig_addVertex(v);
        }
        for (Face* f: geom_halfMesh.faces) {
            emit sig_addFace(f);
        }

        this->update();

        //as an additional check to make sure set up vertices half edges properly
        for (Vertex* v : vertices) {
            if ((v->halfEdgesOfCurrVector).size()==0) {
                std::cout<<"v:"<<v->id<<" has no halfEdges pointing to it"<<std::endl;
            }
        }


        //to make sure highlights follow the new mesh
//        slot_vertexItemClicked(geom_halfMesh.vertices[0]);
//        slot_faceItemClicked(geom_halfMesh.faces[0]);
//        slot_hEdgeItemClicked(geom_halfMesh.halfEdges[0]);
    }
}

//simple helper method for making a vertex from a line of position data
//returns the vertex of the line that was parsed
//note: sets the vertex's id to -1, bc in this method: do not have access to current idNum
Vertex* MyGL::parseLineForVertex(std::string line) {
    //cut of the first char (v) and the space
    std::string dataOfLine = line.substr(2);

    //the values we want to trim off between tokens
    const char* space = " ";

    //firstToken
    int spaceLoc = dataOfLine.find_first_of(space);
    std::string xPos = dataOfLine.substr(0, spaceLoc);

    //shift string for next token
    dataOfLine = dataOfLine.substr(spaceLoc+1);
    //secondToken
    spaceLoc = dataOfLine.find_first_of(space);
    std::string yPos = dataOfLine.substr(0, spaceLoc);

    //shift string for next token
    dataOfLine = dataOfLine.substr(spaceLoc+1);
    //thirdToken
    spaceLoc = dataOfLine.find_first_of(space);
    std::string zPos = dataOfLine.substr(0, spaceLoc);

    return new Vertex(glm::vec3(std::stod(xPos),
                                std::stod(yPos),
                                std::stod(zPos)),
                                nullptr,
                                -1);
}

//simple helper method for making the face from a line of vertex/uv/normals data
//  currently updates map of face to the vertices pointing to that face
//  returns the face of the line that was parsed
//note: sets the face's id to -1, bc in this method: do not have access to current idNum
Face* MyGL::parseLineForFace(std::string line,
                             std::vector<Vertex*>& vertices,
                             std::map<Face*, std::vector<Vertex*>>& mapFaceToAllVertices) {
    //cut of the first char (v) and the space
    //add "~" to end of it bc our obj file will not be taking this character in
    //  used to delineate when a line finishes [as per obj file format]
    std::string dataOfLineFace = line.substr(2) + "~";

    //current face: note: putting max of 255, 120, 255 bc that's the color range i chose (didnt want too much green)
    //note: i can use floats from 0 to 255 bc in my create method for my mesh class i divide all the colorsRGBs by 255 already
    float r = std::rand() % 255;
    float g = std::rand() % 120;
    float b = std::rand() % 255;
    glm::vec4 faceColor = glm::vec4(r, g, b, 1.0f);
    Face* face = new Face(nullptr, faceColor, -1);

    //the values we want to trim off between tokens and or search for
    const char* space = " ";
    const char* slash = "/";
    //to search at each looping for vertex/uvofvert/normalofvert in a line want to be able
    //      to cut dataOfLine so only includes info that hasn't been reached yet.
    //      includes " " between each full loop and ~ to tell us we finished the line
    const char* spaceSquig = " ~";

    //currentVertex(includes: vertex/uvofvert/normalofvert)
    int spaceLoc = dataOfLineFace.find_first_of(space);
    std::string currentVertexInfo;
    while(!dataOfLineFace.empty()) {
        currentVertexInfo = dataOfLineFace.substr(0, spaceLoc);
        //vertex Id
        int slashLoc = currentVertexInfo.find_first_of(slash);
        std::string vId = currentVertexInfo.substr(0, slashLoc);

        //uv Id - CURRENTLY DONT NEED FOR HW05
        //shift string for next token past "/"
        currentVertexInfo = currentVertexInfo.substr(slashLoc+1);
        slashLoc = currentVertexInfo.find_first_of(slash);
        std::string uvId = currentVertexInfo.substr(0, slashLoc);

        //normal Id - CURRENTLY DONT NEED FOR HW05
        //shift string for next token past "/"
        currentVertexInfo = currentVertexInfo.substr(slashLoc+1);
        slashLoc = currentVertexInfo.find_first_of(slash);
        std::string norId = currentVertexInfo.substr(0, slashLoc);

        //map current face to vertex with that id
        int vIdInt = std::stoi(vId);
        for (Vertex* vertexHere: vertices) {
            if (vIdInt == vertexHere->id) {
                mapFaceToAllVertices[face].push_back(vertexHere);
            }
        }

        //shift full line string for next token past " " or make it empty if finished
        //  last for the current face
        int spaceSquigLoc = dataOfLineFace.find_first_of(spaceSquig);
        //if not last element
        if (spaceSquigLoc < (int)(dataOfLineFace.size())-1) {
            dataOfLineFace = dataOfLineFace.substr(spaceSquigLoc+1);
        //otherwise this current looping was the last element so make dataOfLine empty
        } else {
            dataOfLineFace = "";
        }
    }

    return face;
}

//HALFEDGES (all data but syms)
void MyGL::buildHEdges(
        std::vector<Face*>& faces,
        std::vector<HalfEdge*>& hEdges,
        std::map<Face*, std::vector<Vertex*>>& mapFaceToAllVertices,
        std::map<Vertex*, std::vector<HalfEdge*>>& mapVertexToAllHEdges,
        int& halfEdgeId) {

    for (Face* f: faces) {
        //loop through all vertices to make halfEdges that point to them and that are associated with
        //   this face
        //the first half edge you make - add it as the start edge for this face
        //for each half edge that points to a vertex
        //          add it to the vertex's vector of half edges
        //          if the vertex does not yet have a half edge member variable (points to nullptr) - add it there as well
        std::vector<Vertex*> faceToVert = mapFaceToAllVertices[f];

        HalfEdge* prev = nullptr;
        HalfEdge* first = nullptr;
        int faceVertVectorSize = (int)faceToVert.size();
        for (int i=0; i < faceVertVectorSize; i++) {
            Vertex* vert = faceToVert[i];

            //CREATE HEDGE
            HalfEdge* half = new HalfEdge(f, vert, nullptr, nullptr, halfEdgeId);
            //if this is the first halfedge created - store it so can use as next for the last one
            if (i == 0) {
                first = half;
            }

            //FILL IN VERT HALFEDGE VAL IF NONE THERE
            //if vertex.halfedge (for one of the ones pointing to it is not set yet so nullptr)
            //  set it to current half edge
            if (vert->edge == nullptr) {
                vert->edge = half;
            }
            //FILL IN FACE HALFEDGE VAL IF NONE THERE
            //if vertex.halfedge (for one of the ones pointing to it is not set yet so nullptr)
            //  set it to current half edge
            if (f->start_edge == nullptr) {
                f->start_edge = half;
            }

            //SET HEDGE NEXT
            //if prev has been initialized in the prev loop of this for each then can add
            if (i != 0) {
                prev->setHEdgeNext(half);
            }
            //update prev for next halfedge in next iteration through this for loop
            prev = half;

            //SET FIRST HEDGE NEXT
            //if on last vertex - update
            if (i == faceVertVectorSize-1) {
                half->setHEdgeNext(first);
            }

            //ADD HALFEDGE TO VECTOR OF HALFEDGES
            hEdges.push_back(half);

            //add current half edge to map of vertex to all halfedges that point to it
            mapVertexToAllHEdges[vert].push_back(half);

           //INCREMENT hId so next face we see will have the next id num
           halfEdgeId += 1;
        }
    }//END of: HALFEDGES (but not syms)
}

//HALFEDGES SYMS
//IN THIS METHOD I ALSO ADD THE VERTTOHEDGE VECTOR TO THE APPROPRIATE VERTEX (bc filling all params of a vertex now)
void MyGL::linkSymsForHEdges(std::vector<HalfEdge*>& hEdges, std::map<Vertex*, std::vector<HalfEdge*>>& mapVertexToAllHEdges) {
    for(HalfEdge* h: hEdges) {
        Vertex* v = h->vert;
        std::vector<HalfEdge*> vertToHEdge = mapVertexToAllHEdges[v];

        v->setHEdgeList(vertToHEdge);

        //if the half edge's sym has not been matched yet then:
        if (h->sym == nullptr) {
            //to find the sym for our current half edge, we do the following:
            //iterate through each halfEdge in the vector vertToHEdge
            //  if our halfedge(h)'s vertex(v), is the vertex of the prev HEdge (prevOfChecked) for the (checkedHalfEdge)
            //  AND if the (checkedHalfEdge)'s vertex is the vertex of the prev HEdge (prevOfH) for (h)
            //      then they are syms
            //  otherwise they are not.

            for (HalfEdge* checkedHalfEdge: vertToHEdge) {
                //find prev HalfEdge of (h)
                HalfEdge* prevOfH = getPrevHEdge(h);
                //find prev HalfEdge of (checkedHalfEdge)
                HalfEdge* nextOfChecked = checkedHalfEdge->next;

                //if the above described condition holds, then set both of them as each other's sym
                int Hp = prevOfH->vert->id;
                int H = h->vert->id;
                int Cp = checkedHalfEdge->vert->id;
                int C = nextOfChecked->vert->id;
                if ((Hp == C) && (H == Cp)) {
                    nextOfChecked->setHEdgeSym(h);
                    h->setHEdgeSym(checkedHalfEdge->next);
                }
            }
        } //END of: if h->sym == nullptr
    }//END of: HALFEDGES SYMS
}

//simple helper method to get the previous half edge (on a face) of a specified half edge
//  so just going in a circle
HalfEdge* MyGL::getPrevHEdge(HalfEdge* h) {
    bool first = true;
    HalfEdge* curr = h;
    while (first || curr->next != h) {
        first = false;
        curr = curr->next;
    }
    return curr;
}


//Adds a vertex at the position in the center of the current highlighted edge
//creates halfedges accordingly and updates the vertex, syms, and next of all of the halfedges
//      associated with the line
//also updates the halfedges vector of the created vertex so it knows that the two newly
//      created half edges point to it
// the variables used are:
//  h: the current half edge
//  p: the prev of the current half edge
//  hadd: the added half edge that points to h
//  v0: the vertex h points to
//  hs: the current half edge's sym
//  ps: the prev of hs
//  hsadd: the added half edge that points to hs
//  v1: the vertex hs points to
void MyGL::slot_addVertexToCurrentHighlightedEdge() {
    //no half edge selected yet then select the first half edge in the mesh's vector of hEdges
    if (hHEdge.currentEdge == nullptr) {
//        slot_hEdgeItemClicked(geom_halfMesh.halfEdges[0]);

//        if (geom_halfMesh.halfEdges[0] != selectedHEdge) {
                selectedHEdge = (geom_halfMesh.halfEdges[0]);
                hHEdge.currentEdge = selectedHEdge;
        //        hHEdge.create();
        //        this->update();
//            }
    }

    HalfEdge* h = hHEdge.currentEdge; //current half edge to be split
    HalfEdge* p = getPrevHEdge(h);    //prev of h
    HalfEdge* hs = nullptr;           //sym of h
    HalfEdge* ps = nullptr;           //prev of sym

    //find sym hedge to the current hedge
    std::vector<HalfEdge*> hEdgeList = h->vert->halfEdgesOfCurrVector;
    for (HalfEdge* checkedHalfEdge: hEdgeList) {
        //find prev HalfEdge of (checkedHalfEdge)
        HalfEdge* nextOfChecked = checkedHalfEdge->next;

        int pId = p->vert->id;
        int hId = h->vert->id;
        int cId = checkedHalfEdge->vert->id;
        int cnId = nextOfChecked->vert->id;
        if ((pId == cnId) && (hId == cId)) {
            hs = nextOfChecked;
        }
    }
    if (hs == nullptr) {
        std::cout<<"ERROR in addVertexToCurrentHighlightedEdge: current half edge does not have a sym";
    }

    //find prev of sym hedge
    ps = getPrevHEdge(hs);

    //set up faces
    Face* f = h->face;    //find face associated with current hedge
    Face* fs = hs->face;  //find face associated with sym hedge

    //set up vertices
    Vertex* v0 = h->vert; //vertex associated with current hedge
    Vertex* v1 = hs->vert; //vertex associated with sym hedge
    //create new vertex (vId = vertices.size())
    Vertex* v2 = new Vertex((v0->pos + v1->pos)/2.0f,
                            nullptr,
                            geom_halfMesh.vertices.size());

    //create 2 new half edges (hId = hEdges.size(), hEdges.size()+1)
    HalfEdge* hAdd = new HalfEdge(f, v2, nullptr, nullptr, geom_halfMesh.halfEdges.size());
    HalfEdge* hsAdd = new HalfEdge(fs, v2, nullptr, nullptr, (hAdd->id)+1);
    //update1: next pointers
    p->next = hAdd;
    hsAdd->next = hs;
    ps->next = hsAdd;
    hAdd->next = h;
    //update2: vertex pointers (to the added vertex and that the added vertex points to)
    hAdd->setVert(v2);
    hsAdd->setVert(v2);
    v2->edge = hAdd;
    //update3: sym pointers
    h->sym = hsAdd;
    hsAdd->sym = h;
    hs->sym = hAdd;
    hAdd->sym = hs;
    v2->addHEdgeToList(hAdd);
    v2->addHEdgeToList(hsAdd);

    //add new vertex to list of vertices
    geom_halfMesh.vertices.push_back(v2);
    //add two new halfedges to list of half edges
    geom_halfMesh.halfEdges.push_back(hAdd);
    geom_halfMesh.halfEdges.push_back(hsAdd);

    emit sig_addHalfEdge(hAdd);
    emit sig_addHalfEdge(hsAdd);
    emit sig_addVertex(v2);

    //set highlighted edge to current hedge
//    slot_hEdgeItemClicked(h);
    //set highlighted face to face of current hedge
//    slot_faceItemClicked(f);
    //set highlighted vertex to vertex that current hedge points to
//    slot_vertexItemClicked(v0);

    geom_halfMesh.create();
//    hFace.create();
//    hHEdge.create();
//    hVert.create(); --- since dont need to see highlights for this project
    this->update();
}
