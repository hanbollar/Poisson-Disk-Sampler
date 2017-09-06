#include "poissonsampler.h"

std::vector<std::vector<std::vector<Sample*>>> PoissonSampler::poissonAlg(){
    // set up background grid
    //      for now grid dimensions based entirely on const radius as declared.

    // grid dim: RADIUS/(dim_n)^1/2 --> bigger r is bigger voxels is fewer samples

    // setting up 3D background grid for the algorithm
    float maxVoxelSize = RADIUS/((nDimensions)*(nDimensions));
    Point3f maxP = bbox->max;
    Point3f minP = bbox->min;

    glm::vec3 voxelDim = glm::vec3((maxP[0] - minP[0])/maxVoxelSize,
                                   (maxP[1] - minP[1])/maxVoxelSize,
                                   (maxP[2] - minP[2])/maxVoxelSize);


    std::vector<std::vector<std::vector<Sample*>>> backgroundGrid3D
            (voxelDim[0],
             std::vector<std::vector<Sample*>>(voxelDim[1],
                                              std::vector<Sample*>(voxelDim[2], nullptr)
                                              )
            );

    //    std::vector<std::vector<Sample*>> backgroundGrid2D;
    // for now only doing 3D for simplicity

    // set up pointer list of valid samples
    std::vector<Sample*> activeValidSamples(0, nullptr);

    // THE POISSON ALGORITHM

    // choose random sample loc from grid - make sure valid in the obj
    glm::vec3 randGridLoc = glm::vec3((int)((float) std::rand() / (RAND_MAX) * voxelDim[0]),
                                      (int)((float) std::rand() / (RAND_MAX) * voxelDim[1]),
                                      (int)((float) std::rand() / (RAND_MAX) * voxelDim[2]));
    // add it to active list & background grid
    while(!validLocWithinObj(randGridLoc)) {
        randGridLoc = glm::vec3((int)((float) std::rand() / (RAND_MAX) * voxelDim[0]),
                                (int)((float) std::rand() / (RAND_MAX) * voxelDim[1]),
                                (int)((float) std::rand() / (RAND_MAX) * voxelDim[2]));
    }
    activeValidSamples.push_back(new Sample(posToGridLoc(randGridLoc, voxelDim), randGridLoc, 0));

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
            glm::vec3 pos = /** ------------- TO IMPLEMENT THIS PART HERE ------------- **/ glm::vec3(0.0f);

            for (Sample* samp: activeValidSamples) {
                if (glm::distance(pos, samp->pos) > RADIUS) {
                    // valid loc - only want to do within check if valid rad so checked fewer times

                    if (validLocWithinObj(pos)) {

                        // valid then create and add to grid
                        glm::vec3 gLoc = posToGridLoc(pos, voxelDim);
                        int id = activeValidSamples.size() + i;
                        Sample* kPoint = new Sample(gLoc, pos, id);

                        activeValidSamples.push_back(kPoint);
                        backgroundGrid3D[gLoc[0]][gLoc[1]][gLoc[2]];

                        addedK = true;
                    }
                }
            }// end: for (Sample* samp: activeValidSamples);
        }// end: for (int i=0; i<K; i++);

        if (!addedK) {

            //x_i no longer a valid sample
            activeValidSamples.erase(std::remove(activeValidSamples.begin(), activeValidSamples.end(), x_i),
                                     activeValidSamples.end());
        }
    } //end: while(activeValidSamples.size() > 0)

    return backgroundGrid3D;
}

/**
 * @brief posToGridLoc - helper method for poisson alg
 * @param p - position to be put into grid
 * @param voxelDim - dimensions of each voxel loc
 * @return glm::vec3 - valid grid location of this position
 */
glm::vec3 PoissonSampler::posToGridLoc(glm::vec3 p, glm::vec3 voxelDim) {
    // precondition: given position is already valid location inside the given object

    // TO DO
    return glm::vec3(0.0f);
}

glm::vec3 PoissonSampler::gridLocToPos(glm::vec3 gLoc, glm::vec3 voxelDim) {
    // note: just returning exact center of grid since that is all the info we have

    // TO DO
    return glm::vec3(0.0f);
}

/**
 * @brief sampleWithinObj - checks if given sampled location is within the object being checked
 * @param p: the position of the given sample - so can check if within given obj to fill
 * @return true, if within; false, otherwise.
 */
bool PoissonSampler::validLocWithinObj(glm::vec3 p) {
    // TO DO
    return false;
}
