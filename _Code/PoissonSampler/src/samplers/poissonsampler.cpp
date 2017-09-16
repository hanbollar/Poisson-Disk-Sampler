#include "poissonsampler.h"

PoissonSampler::PoissonSampler(Mesh &mesh, Scene &scene, bool isThreeDim)
    : m(mesh), s(scene), bvh(nullptr), bbox(nullptr),
        threeDim(isThreeDim), voxelDim(glm::vec3(0.0f)), numPoints(0), samp(5, 5) {

    initializeBackgroundGridsandBVH();
    poissonAlg();
}

GLenum PoissonSampler::drawMode() {
    return GL_POINTS;
}

void PoissonSampler::create() {
    //create new vbo
    //set drawmode to GL_POINTS

    //  pos vec3s
    std::vector<glm::vec3> posVector;

    int i=0;
    int j=0;
    int k=0;

    this->numPoints = 0;

    if (threeDim) {
        while (i<backgroundGrid3D.size()) {
            while (j<(backgroundGrid3D[i]).size()) {
                while (k<(backgroundGrid3D[i][j]).size()) {

                    if (backgroundGrid3D[i][j][k]!= nullptr) {
                        posVector.push_back((backgroundGrid3D[i][j][k])->pos);

                        numPoints += 1;
                    }

                    k++;
                }
                j++;
            }
            i++;
        }
    } else {
        while (i<backgroundGrid3D.size()) {
            while (j<backgroundGrid3D[i].size()) {

                if (backgroundGrid2D[i][j]!= nullptr) {
                    posVector.push_back((backgroundGrid2D[i][j])->pos);
                    numPoints += 1;
                }



                j++;
            }
            i++;
        }
    }

    std::cout<<"-----------------numPoints: "<<numPoints<<std::endl;

    GLuint points_idx[numPoints];
    glm::vec3 points_vert_pos[numPoints];
    glm::vec3 points_vert_nor[numPoints];
    glm::vec3 points_vert_col[numPoints];

    glm::vec3 color2 = glm::vec3(200.0f, 230.0f, 255.0f) / 255.0f; //yellow
    glm::vec3 color3 = glm::vec3(1.0f, 0.0f, 0.0f); //red
    for (int p = 0; numPoints!=0 && p<numPoints; p++) {
        float lerpWeight = p/(1.0f*numPoints);

        points_vert_col[p] = color3; /*(color2*lerpWeight + (1-lerpWeight)*color3); */
        points_vert_nor[p] = glm::vec3(0, 0, 1);
        points_idx[p] = p;
        points_vert_pos[p] = posVector[p];
    }

    count = numPoints;

    //  handling line indices vbo
    bufIdx.create();
    bufIdx.bind();
    bufIdx.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufIdx.allocate(points_idx, numPoints * sizeof(GLuint));

    bufPos.create();
    bufPos.bind();
    bufPos.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufPos.allocate(points_vert_pos, numPoints * sizeof(glm::vec3));

    bufNor.create();
    bufNor.bind();
    bufNor.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufNor.allocate(points_vert_nor, numPoints * sizeof(glm::vec3));

    bufCol.create();
    bufCol.bind();
    bufCol.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufCol.allocate(points_vert_col, numPoints * sizeof(glm::vec3));
}

void PoissonSampler::poissonAlg(){
    std::cout<<"poissonSampler::poissonAlg"<<std::endl;

    // set up pointer list of valid samples
    std::vector<Sample*> activeValidSamples(0, nullptr);

    // choose random sample loc from grid - make sure valid in the obj & add to active list & background grid
    // implement this part by doing actual pos of object instead of rand location in obj and just having the alg
    //      breaking out from there [since can only do linear transformations on obj - must be valid pos in the obj]

    std::cout<<"here1"<<std::endl;
    glm::vec3 tloc = m.faces.first().get()->points[0];
    std::cout<<"here2"<<std::endl;
    std::cout<<"position of first vertex:"<<tloc.x<<","<<tloc.y<<","<<tloc.z<<","<<std::endl;
    std::cout<<"here3"<<std::endl;
    glm::vec3 randGridLoc = posToGridLoc(m.faces.first().get()->points[0]);
    Sample* start = new Sample(randGridLoc, m.transform.position(), 0);
    activeValidSamples.push_back(start);

    if (threeDim) {
        std::cout<<"randGridLoc:"<<randGridLoc[0]<<","<<randGridLoc[1]<<","<<randGridLoc[2]<<std::endl;
        backgroundGrid3D[randGridLoc[0]][randGridLoc[1]][randGridLoc[2]] = start;
    } else {
        backgroundGrid2D[randGridLoc[0]][randGridLoc[1]] = start;
    }

    // number of samples tested at each x_i in the while loop [kept constant]
    int K = 10;

    std::cout<<"poissonSampler::poissonAlg beginning the while loop"<<std::endl;

    // while activeValidSampleSize > 0
    // do:
    //      i = chosen random sample index id from active list
    //      generate k pts chosen uniformly between RADIUS and 2RADIUS around the current loc
    //      For each pt k {
    //          not valid sample if (1) within RADIUS of any existing samples (2) not within given mesh obj
    //          if valid then add to active list
    //      } if after all checked and no k were added --> remove x_i from active list
    // return completed list of samples
    while(activeValidSamples.size() > 0) {
        float abTest = samp.Get2D().x;

        Sample* x_i = activeValidSamples[(int)(abTest * activeValidSamples.size())];
        std::cout<<"sample x_i pos: "<<x_i->pos[0]<<","<<x_i->pos[1]<<","<<x_i->pos[2]<<std::endl;
        bool addedK = false;
        for (int i=0; i<K; i++) {
            glm::vec3 pos = randomLocAround(x_i->pos);
            std::cout<<"---------------------------"<<std::endl;
            std::cout<<"on item: i:"<<i<<" <K:"<<K<<" loc:"<<pos[0]<<","<<pos[1]<<","<<pos[2]<<std::endl;

            //note must make sure ^^ provides valid position that will be within the current grid area so sampling must check that first
            glm::vec3 gLoc = posToGridLoc(pos);
            std::cout<<"gridLoc: "<<gLoc[0]<<","<<gLoc[1]<<","<<gLoc[2]<<std::endl;

            // find collection of samples to test untested sample against so not testing against every single one
            // since testing within R to 2R radius and the boxes are bounded by that can just look for all 8 boxes
            //      around current gridP location
            glm::vec3 checkingMin = gLoc - 2 * RADIUS; //voxel checks
            glm::vec3 checkingMax = gLoc + 2 * RADIUS; //voxel checks

            checkingMin = glm::vec3(glm::clamp(checkingMin[0], 0.0f, voxelDim[0] - 1),
                                    glm::clamp(checkingMin[1], 0.0f, voxelDim[1] - 1),
                                    glm::clamp(checkingMin[2], 0.0f, voxelDim[2] - 1) );
            checkingMax = glm::vec3(glm::clamp(checkingMax[0], 0.0f, voxelDim[0] - 1),
                                    glm::clamp(checkingMax[1], 0.0f, voxelDim[1] - 1),
                                    glm::clamp(checkingMax[2], 0.0f, voxelDim[2] - 1) );


            bool valid = true;

            for (int j = checkingMin[0]; j <= checkingMax[0]; j++) {
                for (int k = checkingMin[1]; k <= checkingMax[1]; k++) {
                    for(int l = checkingMin[2]; l <= checkingMax[2] && threeDim; l++) {



                        if (backgroundGrid3D[j][k][l] != nullptr) {
                            if (j == gLoc[0] && k == gLoc[1] && l == gLoc[2]) {
                                valid = false;
                                std::cout<<"isValid: "<<false<<std::endl;
                            }

                            bool isValid = (glm::distance(gLoc, x_i->gridLoc) >= RADIUS && (glm::distance(gLoc, x_i->gridLoc) <= 2*RADIUS));
//                            std::cout<<"valid before isValid: "<<valid<<std::endl;
//                            std::cout<<"isValid: "<<isValid<<std::endl;
                            valid &= isValid;
//                            std::cout<<"valid after isValid: "<<valid<<std::endl;
                        } else {
//                            std::cout<<"skipped gloc: "<<j<<","<<k<<","<<l<<std::endl;
                        }
                    }

//                    if (!threeDim && backgroundGrid2D[j][k] != nullptr) {
//                        bool isValid = (glm::distance(gLoc, x_i->gridLoc) >= RADIUS && (glm::distance(gLoc, x_i->gridLoc) <= 2*RADIUS));
//                        std::cout<<"isValid: "<<isValid<<std::endl;
//                        valid &= isValid;
//                    } else if (!threeDim && (j == gLoc[0] && k == gLoc[1])) {
//                        valid = false;
//                        std::cout<<"isValid: "<<false<<std::endl;
//                    }
                    // TO COMPLETE TO COMPLET TO COMPLETE FOR 2d

                }//end: for (int j = checkingMin[1]; j < checkingMax[1]; j++);
            }//end: for (int i = checkingMin[0]; i < checkingMax[0]; i++);

            // have all valid locations but not valid within obj -- NEED TO RESOLVE

//            std::cout<<std::endl;
//            std::cout<<"valid based on grid around: "<<valid<<std::endl;
//            std::cout<<"validLocWithinObj(pos): "<<validLocWithinObj(pos)<<": for pos:"<<pos[0]<<","<<pos[1]<<","<<pos[2]<<std::endl;
//            std::cout<<std::endl;

            if (valid && validLocWithinObj(pos)) {

                // valid then create and add to grid
                int id = activeValidSamples.size() + i;
                Sample* kPoint = new Sample(gLoc, pos, id);

                activeValidSamples.push_back(kPoint);

                if (threeDim) {
                    if (backgroundGrid3D[gLoc[0]][gLoc[1]][gLoc[2]] != nullptr) {
                        throw;
                    }
                    backgroundGrid3D[gLoc[0]][gLoc[1]][gLoc[2]] = kPoint;
                } else {
                    backgroundGrid2D[gLoc[0]][gLoc[1]] = kPoint;
                }

                addedK = true;

                std::cout<<"added location from this sample"<<std::endl;
            }

        }// end: for (int i=0; i<K; i++);

        if (!addedK) {

            //none of x_i's newly created samples were added so x_i no longer a valid sample
            activeValidSamples.erase(std::remove(activeValidSamples.begin(), activeValidSamples.end(), x_i),
                                     activeValidSamples.end());

            std::cout<<"did not add any from this sample"<<std::endl;
        }

        std::cout<<"poissonSampler::poissonAlg while(activeValidSamples.size() > 0 -- the sample size: "<<activeValidSamples.size()<<std::endl;

    } //end: while(activeValidSamples.size() > 0)

    std::cout<<"poissonSampler::poissonAlg finished"<<std::endl;

}

glm::vec3 PoissonSampler::randomLocAround(glm::vec3 pos) {
    float x = samp.Get2D().x * 10 * maxVoxelSize;
    float y = samp.Get2D().x * 10 * maxVoxelSize;
    float z = samp.Get2D().x * 10 * maxVoxelSize;

    x = (x > maxVoxelSize/5.0f) ? pos[0] + x : pos[0] - x;
    y = (y > maxVoxelSize/5.0f) ? pos[1] + y : pos[1] - y;
    z = (z > maxVoxelSize/5.0f) ? pos[2] + z : pos[2] - z;

    return glm::vec3(x, y, z);
}

/**
 * @brief PoissonSampler::initializeBackgroundGrids
 *
 * set up background grid
 * for now grid dimensions based entirely on const radius as declared.
 */
void PoissonSampler::initializeBackgroundGridsandBVH() {
    // grid dim: RADIUS/(dim_n)^1/2 --> bigger r is bigger voxels is fewer samples

    std::cout<<"poissonSampler::initializeBackgroundGridsandBVH"<<std::endl;

    int nDim = 3;
    if (!threeDim) { nDim = 2; }

    maxVoxelSize = RADIUS/(nDim*nDim);
    bvh = new PoissonBVH(m);
    bbox = bvh->root->bbox;

    Point3f maxP = bbox->max;
    Point3f minP = bbox->min;

    this->voxelDim = glm::vec3(glm::ceil((maxP[0] - minP[0])/maxVoxelSize),
                               glm::ceil((maxP[1] - minP[1])/maxVoxelSize),
                               glm::ceil((maxP[2] - minP[2])/maxVoxelSize) );

    this->backgroundGrid3D = std::vector<std::vector<std::vector<Sample*>>>(
                                   voxelDim[0],
                                   std::vector<std::vector<Sample*>>(voxelDim[1],
                                                                     std::vector<Sample*>(voxelDim[2], nullptr)
                                                                     )
                             );
    this->backgroundGrid2D = std::vector<std::vector<Sample*>>(
                                   voxelDim[0],
                                   std::vector<Sample*>(voxelDim[1])
                             );

    std::cout<<"poissonSampler::initializeBackgroundGridsandBVH finished"<<std::endl;
}

/**
 * @brief posToGridLoc - helper method for poisson alg
 * @param p - position to be put into grid
 * @param voxelDim - dimensions of each voxel loc
 * @return glm::vec3 - valid grid location of this position
 */
glm::vec3 PoissonSampler::posToGridLoc(glm::vec3 p) {
    // precondition: given position is already valid location inside the given bounding box so fits in
    //      the background grid
    // grid is based around bounds3f of bbox for the given object

    glm::vec3 min = bbox->min;

    if (threeDim) {
        return glm::vec3((int)(glm::clamp(((p[0] - min[0])/maxVoxelSize), 0.0f, voxelDim[0] - 1)),
                         (int)(glm::clamp(((p[1] - min[1])/maxVoxelSize), 0.0f, voxelDim[1] - 1)),
                         (int)(glm::clamp(((p[2] - min[2])/maxVoxelSize), 0.0f, voxelDim[2] - 1)) );
    }

    return glm::vec3((int)(glm::clamp(((p[0] - min[0])/maxVoxelSize), 0.0f, voxelDim[0] - 1)),
                     (int)(glm::clamp(((p[1] - min[1])/maxVoxelSize), 0.0f, voxelDim[1] - 1)),
                     0.0f ); //since grid is 2d version dont want to worry about unnecessary dividing.

}

/**
 * @brief sampleWithinObj - checks if given sampled location is within the object being checked
 * @param p: the position of the given sample - so can check if within given obj to fill
 * @return true, if within; false, otherwise.
 */
bool PoissonSampler::validLocWithinObj(glm::vec3 p) {

    if (threeDim) {

        int numIsx = 0;
        Intersection isx = Intersection();
        Ray r = Ray(s.camera.eye, glm::normalize(s.camera.eye - p));

        if (bvh->intersect(r, &isx, &numIsx, s.camera.GetViewProj())) {
            return (numIsx %2 == 0) ? false : true;
        }
    }

    // else: 2dim

//         to check if within for 2d then first check at proper z loc
//         and just check if x and y are within appropriate bounds2f for it

    /** TO COMPLETE TO COMPLETE TO COMPLETE TO COMPLETE TO COMPLETE **/
    /** TO COMPLETE TO COMPLETE TO COMPLETE TO COMPLETE TO COMPLETE **/
    /** TO COMPLETE TO COMPLETE TO COMPLETE TO COMPLETE TO COMPLETE **/
    /** TO COMPLETE TO COMPLETE TO COMPLETE TO COMPLETE TO COMPLETE **/
    /** TO COMPLETE TO COMPLETE TO COMPLETE TO COMPLETE TO COMPLETE **/
    /** TO COMPLETE TO COMPLETE TO COMPLETE TO COMPLETE TO COMPLETE **/
    /** TO COMPLETE TO COMPLETE TO COMPLETE TO COMPLETE TO COMPLETE **/
    /** TO COMPLETE TO COMPLETE TO COMPLETE TO COMPLETE TO COMPLETE **/
    /** TO COMPLETE TO COMPLETE TO COMPLETE TO COMPLETE TO COMPLETE **/
    /** TO COMPLETE TO COMPLETE TO COMPLETE TO COMPLETE TO COMPLETE **/
    /** TO COMPLETE TO COMPLETE TO COMPLETE TO COMPLETE TO COMPLETE **/

    return false;
}
