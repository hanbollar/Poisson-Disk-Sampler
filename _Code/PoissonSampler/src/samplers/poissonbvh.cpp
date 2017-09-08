#include "poissonsampler.h"

void PoissonBVH::buildBVH(Mesh* m){

    //TO-DO
    //using root somewhere

    int minNumOfTris = 15;

    //build bbox of root
    root->bbox = root->buildBoundingBox(m->faces);
    // building the whole tree
    root->buildSelfAsChild(m->faces, minNumOfTris);
}

void P_BVHNode::buildSelfAsChild(QList<std::shared_ptr<Triangle>> t, int minNum) {

    // notes:
    // if current object has fewer than 10 objects then this is a leaf node
    // do the split along the longest axis
    // create bbox for left and right
    // use surface area heuristic to determine which should be in left and which should be in right
    // build left and right children
    // done

    // base case
    if (t.length() < minNum) {
        // leaf node
        this->tris = t;

        // checking for construction errors
        if (this->bbox == nullptr) { std::cout<<"error in P_BVHNode buildSelfASChild"<<std::endl; throw; }
        if (this->l != nullptr) { std::cout<<"error in P_BVHNode buildSelfASChild"<<std::endl; throw; }
        if (this->r != nullptr) { std::cout<<"error in P_BVHNode buildSelfASChild"<<std::endl; throw; }

        return;
    }

    // finding longest axis
    glm::vec3 diag = this->bbox->Diagonal();
    float longest = glm::max(diag[0], glm::max(diag[1], diag[2]));
    int longestAxis = (longest == diag[0]) ? 0 : (longest == diag[1] ? 1 : diag[2]);

    // SA heuristic: calculated at each split
//    float SAdiff = fabs(rightSA * rightCount + leftSA * leftCount) / outerB;
//    if (SAdiff < minSAH) {
//        minSAH = SAdiff;
//        splitVal = atLoc;
//        set0 = temp_set0;
//        set1 = temp_set1;
//    }\

    /******** TO COMPLEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEETE **/

}

Bounds3f* P_BVHNode::buildBoundingBox(QList<std::shared_ptr<Triangle>> t) {
    // TO DO

    return nullptr;
}
