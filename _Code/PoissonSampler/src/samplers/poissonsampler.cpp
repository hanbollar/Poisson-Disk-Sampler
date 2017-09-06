#include "poissonsampler.h"

std::vector<Sample*> PoissonSampler::poissonAlg(){
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
    // add it to active list & background grid

    // while activeValidSampleSize > 0
    // do:
    //      i = chosen random sample index id from active list
    //      generate k pts chosen uniformly between RADIUS and 2RADIUS around the current loc
    //      For each pt k {
    //          not valid sample if (1) within RADIUS of any existing samples (2) not within given mesh obj
    //          if valid then add to active list
    //      } if after all checked and no k were added --> remove x_i from active list
    // return completed list of samples
}

/**
 * @brief sampleWithinObj - checks if given sampled location is within the object being checked
 * @param p: the position of the given sample - so can check if within given obj to fill
 * @return true, if within; false, otherwise.
 */
bool sampleWithinObj(glm::vec3 p) {
    // TO DO
    return false;
}
