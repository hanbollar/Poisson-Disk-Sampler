#include "poissonsampler.h"

PoissonSampler::PoissonSampler(Mesh &mesh, Scene &scene, bool isThreeDim)
    : m(mesh), s(scene), bvh(nullptr), bbox(nullptr), finalSamples(0, nullptr),
        threeDim(isThreeDim), voxelDim(glm::vec3(0.0f)), numPoints(0), samp(5, 5) {

    initializeVarsAndBVH();
    poissonAlg();
}

GLenum PoissonSampler::drawMode() const {
    return GL_POINTS;
}

void PoissonSampler::create() {
    //create new vbo
    //set drawmode to GL_POINTS

    //  pos vec3s
    std::vector<glm::vec3> posVector;

    for (Sample* s: finalSamples ){
        posVector.push_back(s->pos);
    }

    std::cout<<"-----------------numPoints: "<<numPoints<<std::endl;

    GLuint points_idx[numPoints];
    glm::vec3 points_vert_pos[numPoints];
    glm::vec3 points_vert_nor[numPoints];
    glm::vec3 points_vert_col[numPoints];

    glm::vec3 color2 = glm::vec3(200.0f, 230.0f, 255.0f) / 255.0f; //yellow
    glm::vec3 color3 = glm::vec3(0.0f, 0.0f, 1.0f); //blue
    for (int p = 0; numPoints!=0 && p<numPoints; p++) {
        float lerpWeight = p/(1.0f*numPoints);

        points_vert_col[p] = (color2*lerpWeight + (1-lerpWeight)*color3);
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

    std::vector<std::vector<std::vector<Sample*>>> backgroundGrid
            = std::vector<std::vector<std::vector<Sample*>>>(
                                           voxelDim[0],
                                           std::vector<std::vector<Sample*>>(voxelDim[1],
                                                                             std::vector<Sample*>(voxelDim[2], nullptr)
                                                                             )
                                    );

    // choose random sample loc from grid - make sure valid in the obj & add to active list & background grid
    // implement this part by doing actual pos of object instead of rand location in obj and just having the alg
    //      breaking out from there [since can only do linear transformations on obj - must be valid pos in the obj]

    glm::vec3 randGridLoc = posToGridLoc(m.faces.first().get()->points[0]);
    Sample* start = new Sample(randGridLoc, m.transform.position(), 0);
    activeValidSamples.push_back(start);

    backgroundGrid[randGridLoc[0]][randGridLoc[1]][(threeDim) ? randGridLoc[2] : 0] = start;

    // number of samples tested at each x_i in the while loop [kept constant]
    int K = 100;

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

        Sample* x_i = activeValidSamples[(int)( samp.Get2D().x * activeValidSamples.size())];
//        std::cout<<"x_i gridpos: "<<x_i->gridLoc[0]<<","<<x_i->gridLoc[1]<<","<<x_i->gridLoc[2]<<std::endl;
        std::cout<</*"sample x_i pos: "<<x_i->pos[0]<<","<<x_i->pos[1]<<","<<x_i->pos[2]<<*/": active: "<<activeValidSamples.size()<<": finished: "<<finalSamples.size()<<std::endl;

        bool addedK = false;
        for (int i=0; i<K; i++) {
            glm::vec3 pos = randomLocAround(x_i->pos);

            //note must make sure ^^ provides valid position that will be within the current grid area so sampling must check that first
            glm::vec3 gLoc = posToGridLoc(pos);

            // find collection of samples to test untested sample against so not testing against every single one
            // since testing within R to 2R radius and the boxes are bounded by that can just look for all 8 boxes
            //      around current gridP location

            glm::vec3 min = bbox->min;

            glm::vec3 div = glm::vec3(voxelDim)/RADIUS;
            float factor = 4;
            glm::vec3 checkingMin(0.0f);
            glm::vec3 checkingMax(0.0f);
            for (int j=0; j<3; j++) {
                checkingMin[j] = pos[j] - factor*div[j];
                checkingMax[j] = pos[j] + factor*div[j];
            }
            checkingMin = posToGridLoc(checkingMin);
            checkingMax = posToGridLoc(checkingMax);

            bool valid = true;
            for (int j = checkingMin[0]; j <= checkingMax[0]; j++) {
                for (int k = checkingMin[1]; k <= checkingMax[1]; k++) {
                    for(int l = checkingMin[2]; l <= checkingMax[2]; l++) {

                        if (backgroundGrid[j][k][l] != nullptr) {
                            if (j == gLoc[0] && k == gLoc[1] && l == gLoc[2]) {
                                valid = false;
//                                std::cout<<"not valid bc same loc as x_i"<<std::endl;
                            }

                            valid &= (glm::distance(pos, x_i->pos) >= RADIUS && (glm::distance(pos, x_i->pos) <= 2*RADIUS));
//                            std::cout<<"valid: "<<valid<<" because of grid distance check"<<std::endl;
                        }
                    }
                }//end: for (int j = checkingMin[1]; j < checkingMax[1]; j++);
            }//end: for (int i = checkingMin[0]; i < checkingMax[0]; i++);

//             std::cout<<"    valid in Grid at loc:"<<gLoc[0]<<","<<gLoc[1]<<","<<gLoc[2]<<": "<<valid<<" valid in Obj: "<<validLocWithinObj(pos)<<" at pos: "<<pos[0]<<","<<pos[1]<<","<<pos[2]<<std::endl;
            if (valid && validLocWithinObj(pos)) {
//                std::cout<<"        added"<<std::endl;
                // valid then create and add to grid
                Sample* kPoint = new Sample(gLoc, pos, activeValidSamples.size() + i);

                activeValidSamples.push_back(kPoint);
                backgroundGrid[gLoc[0]][gLoc[1]][gLoc[2]] = kPoint;

                addedK = true;
            }

        }// end: for (int i=0; i<K; i++);

        if (!addedK) {

            //none of x_i's newly created samples were added so x_i no longer a valid active sample - add to list of final samples
            finalSamples.push_back(new Sample(x_i));
            numPoints += 1;
            activeValidSamples.erase(std::remove(activeValidSamples.begin(), activeValidSamples.end(), x_i),
                                     activeValidSamples.end());

        }

    } //end: while(activeValidSamples.size() > 0)

    std::cout<<"poissonSampler::poissonAlg finished"<<std::endl;
}

/**
 * @brief PoissonSampler::initializeBackgroundGrids
 *
 * set up background grid
 * for now grid dimensions based entirely on const radius as declared.
 */
void PoissonSampler::initializeVarsAndBVH() {
    // grid dim: RADIUS/(dim_n)^1/2 --> bigger r is bigger voxels is fewer samples

    std::cout<<"poissonSampler::initializeBackgroundGridsandBVH"<<std::endl;

    int nDim = (threeDim) ? 3 : 2;

    maxVoxelSize = RADIUS/((nDim*nDim)/*^(1/2)*/);
    bvh = new PoissonBVH(m);
    bbox = bvh->root->bbox;

    Point3f maxP = bbox->max;
    Point3f minP = bbox->min;

    this->voxelDim = glm::vec3(glm::ceil((maxP[0] - minP[0])/maxVoxelSize),
                               glm::ceil((maxP[1] - minP[1])/maxVoxelSize),
                               glm::ceil((maxP[2] - minP[2])/maxVoxelSize) );

    std::cout<<"poissonSampler::initializeBackgroundGridsandBVH finished"<<std::endl;
}

/**
 * @brief PoissonSampler::randomLocAround
 * @param pos
 * @return
 */
glm::vec3 PoissonSampler::randomLocAround(glm::vec3 pos) {
    float val = 3.0f;
    float x = samp.Get2D().x * val * RADIUS;
    float y = samp.Get2D().x * val * RADIUS;
    float z = samp.Get2D().x * val * RADIUS;

    float factor = RADIUS * val/2.0f;
    x = (x > factor) ? pos[0] + x : pos[0] - x;
    y = (y > factor) ? pos[1] + y : pos[1] - y;
    z = (z > factor) ? pos[2] + z : pos[2] - z;

    return glm::vec3(x, y, z);
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

    int x = (int)(glm::clamp(((p[0] - min[0])/RADIUS), 0.0f, voxelDim[0] - 1));
    int y = (int)(glm::clamp(((p[1] - min[1])/RADIUS), 0.0f, voxelDim[1] - 1));
    int z = (threeDim) ? (int)(glm::clamp(((p[2] - min[2])/RADIUS), 0.0f, voxelDim[2] - 1)) : 0;

    return glm::vec3(x, y, z);
}

/**
 * @brief sampleWithinObj - checks if given sampled location is within the object being checked
 * @param p: the position of the given sample - so can check if within given obj to fill
 * @return true, if within; false, otherwise.
 */
bool PoissonSampler::validLocWithinObj(glm::vec3 p) {

//    return (p.x > bbox->min[0] && p.y > bbox->min[1] && p.z > bbox->min[2]
//            && p.x < bbox->max[0] && p.y < bbox->max[1] && p.z < bbox->max[2]);
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
