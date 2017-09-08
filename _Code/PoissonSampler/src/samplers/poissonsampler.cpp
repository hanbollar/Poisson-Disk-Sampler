#include "poissonsampler.h"

void PoissonSampler::poissonAlg(){
    // set up pointer list of valid samples
    std::vector<Sample*> activeValidSamples(0, nullptr);

    // choose random sample loc from grid - make sure valid in the obj & add to active list & background grid
    // implement this part by doing actual pos of object instead of rand location in obj and just having the alg
    //      breaking out from there [since can only do linear transformations on obj - must be valid pos in the obj]
    glm::vec3 randGridLoc = posToGridLoc(m->transform.position());
    Sample* start = new Sample(randGridLoc, m->transform.position(), 0);
    activeValidSamples.push_back(start);

    if (threeDim) {
        backgroundGrid3D[randGridLoc[0]][randGridLoc[1]][randGridLoc[2]] = start;
    } else {
        backgroundGrid2D[randGridLoc[0]][randGridLoc[1]] = start;
    }

    // number of samples tested at each x_i in the while loop [kept constant]
    int K = 10;

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
        Sample* x_i = activeValidSamples[(int)(std::rand() / (RAND_MAX) * activeValidSamples.size())];

        bool addedK = false;
        for (int i=0; i<K; i++) {
            glm::vec3 pos = glm::vec3(0.0f); /** ------------- TO IMPLEMENT THIS PART HERE: SAMPLING RANDOM LOC IN 3D SPACE ------------- **/;
            //note must make sure ^^ provides valid position that will be within the current grid area so sampling must check that first
            glm::vec3 gLoc = posToGridLoc(pos);

            // find collection of samples to test untested sample against so not testing against every single one
            // since testing within R to 2R radius and the boxes are bounded by that can just look for all 8 boxes
            //      around current gridP location
            glm::vec3 checkingMin = gLoc - glm::vec3(1.0);
            glm::vec3 checkingMax = gLoc + glm::vec3(1.0);

            bool valid = true;

            for (int j = checkingMin[0]; j < checkingMax[0]; i++) {
                for (int k = checkingMin[1]; k < checkingMax[1]; j++) {
                    for(int l = checkingMin[2]; l < checkingMax[2] && threeDim; k++) {

                        if (backgroundGrid3D[j][k][l] != nullptr) {
                            valid &= ! (glm::distance(pos, x_i->pos) <= RADIUS);
                        }
                    }

                    if (!threeDim && backgroundGrid2D[j][k] != nullptr) {
                        valid &= ! (glm::distance(pos, x_i->pos) <= RADIUS);
                    }

                }//end: for (int j = checkingMin[1]; j < checkingMax[1]; j++);
            }//end: for (int i = checkingMin[0]; i < checkingMax[0]; i++);

            if (valid && validLocWithinObj(pos)) {

                // valid then create and add to grid
                int id = activeValidSamples.size() + i;
                Sample* kPoint = new Sample(gLoc, pos, id);

                activeValidSamples.push_back(kPoint);

                if (threeDim) {
                    backgroundGrid3D[gLoc[0]][gLoc[1]][gLoc[2]] = kPoint;
                } else {
                    backgroundGrid2D[gLoc[0]][gLoc[1]] = kPoint;
                }

                addedK = true;
            }

        }// end: for (int i=0; i<K; i++);

        if (!addedK) {

            //none of x_i's newly created samples were added so x_i no longer a valid sample
            activeValidSamples.erase(std::remove(activeValidSamples.begin(), activeValidSamples.end(), x_i),
                                     activeValidSamples.end());

            glm::vec3 gLoc_x_i = x_i->gridLoc;
            if (threeDim) {
                backgroundGrid3D[gLoc_x_i[0]][gLoc_x_i[1]][gLoc_x_i[2]] = nullptr;
            } else {
                backgroundGrid2D[gLoc_x_i[0]][gLoc_x_i[1]] = nullptr;
            }
        }

    } //end: while(activeValidSamples.size() > 0)

}

/**
 * @brief PoissonSampler::initializeBackgroundGrids
 *
 * set up background grid
 * for now grid dimensions based entirely on const radius as declared.
 */
void PoissonSampler::initializeBackgroundGridsandBVH() {
    // grid dim: RADIUS/(dim_n)^1/2 --> bigger r is bigger voxels is fewer samples

    int nDim = 3;
    if (!threeDim) { nDim = 2; }

    float maxVoxelSize = RADIUS/(nDim*nDim);
    Point3f maxP = bbox->max;
    Point3f minP = bbox->min;

    this->voxelDim = glm::vec3((maxP[0] - minP[0])/maxVoxelSize,
                               (maxP[1] - minP[1])/maxVoxelSize,
                               (maxP[2] - minP[2])/maxVoxelSize);

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

    bvh = new PoissonBVH(m);
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
        return glm::vec3((int)((p[0] - min[0])/voxelDim[0]),
                         (int)((p[1] - min[1])/voxelDim[1]),
                         (int)((p[2] - min[2])/voxelDim[2]));
    }

    return glm::vec3((int)((p[0] - min[0])/voxelDim[0]),
                     (int)((p[1] - min[1])/voxelDim[1]),
                     0); //since grid is 2d version dont want to worry about unnecessary dividing.

}

/**
 * @brief sampleWithinObj - checks if given sampled location is within the object being checked
 * @param p: the position of the given sample - so can check if within given obj to fill
 * @return true, if within; false, otherwise.
 */
bool PoissonSampler::validLocWithinObj(glm::vec3 p) {
    /** ********** TO DO - do direct for loop for all triangles or use bvh for faster solutions ***************/

    if (threeDim) {
        // if hit odd num of triangles for 3d then within?
        // if hit even num of triangles for 3d then not within?
    } else {
        // to check if within for 2d then first check at proper z loc
        // and just check if x and y are within appropriate bounds2f for it
    }

    return false;
}
