#include "photonmap.h"

#define 	M_PI   3.14159265358979323846	/* pi */

bool PhotonMap::setUpMapObject() {
    // build the bounding box for all objects in the scene
    Bounds3f bbox = Bounds3f();
    QList<std::shared_ptr<Primitive>> prim = currS->primitives;
    QList<std::shared_ptr<Light>> lit = currS->lights;

    // include lights in the bounding box
    for (int i=0; i<lit.size(); i++) {
        bbox = Union(bbox, lit[i].get()->getTransformPos());
    }
    // include primitives in the bounding box
    for (int i=0; i<prim.size(); i++) {
        Bounds3f wB = prim[i].get()->WorldBound();
        bbox = Union(bbox, wB);
    }

    min = bbox.min;
    max = bbox.max;

    dimx = (max.x - min.x) / this->voxelDiv; // div by voxelDim to make voxels bigger
    dimy = (max.y - min.y) / this->voxelDiv;
    dimz = (max.z - min.z) / this->voxelDiv;

    int numVoxels = (int) ( (glm::ceil(dimx) * glm::ceil(dimy) * glm::ceil(dimz)) + 1); // written here to check val when testing

    //std::cout<<"numVoxels: "<<numVoxels<<std::endl;

    indirMap = std::vector<std::vector<Photon*>>((int)this->voxelDiv * (glm::ceil(dimx) * glm::ceil(dimy) * glm::ceil(dimz)) + 1);
    caustMap = std::vector<std::vector<Photon*>>((int)this->voxelDiv * (glm::ceil(dimx) * glm::ceil(dimy) * glm::ceil(dimz)) + 1);

    return true; //set up properly - used for debugging purposes
}

int PhotonMap::worldToMapIndex(Point3f p) {
    float threshold = 0.001;
    if (p.x < min.x - threshold || p.y < min.y - threshold || p.z < min.z - threshold) {
        std::cout<<"ERROR:\nERROR:\nERROR:\ngiven world to map pos of pos outside of bounded range"<<std::endl;;
        throw;
    }

    // indexing with z as the right most digit
    int loc = glm::floor(fabs(p.x - min.x) / this->voxelDiv) * glm::floor(dimy)* glm::floor(dimz)
            + glm::floor(fabs(p.y - min.y) / this->voxelDiv) * glm::floor(dimz)
            + glm::floor(fabs(p.z - min.z) / this->voxelDiv);

    return loc;
}


/*Vector3f to3D(unsigned int idx) {
    int z = idx / (bounds.max[0] * bounds.max[1]);
    idx -= (z * bounds.max[0] * bounds.max[1]);
    int y = idx / bounds.max[0];
    int x = idx % int(bounds.max[0]);
    return Vector3f(x, y, z);
}
*/

/*int to1D(Vector3f& p) {
    return (p.z * bounds.max[0] * bounds.max[1]) + (p.y * bounds.max[1]) + p.x;
} */

void PhotonMap::addToMap(Photon* p, bool trueForCaustFalseForIndir) {
    int loc = worldToMapIndex(p->pos);
   // std::cout<<"added at loc: " <<loc<<std::endl;
    std::vector<Photon*> fill;
    fill.push_back(p);

    if (trueForCaustFalseForIndir) {
        if (caustMap[loc].size() == 0) {
            caustMap[loc] = fill;
        } else {
            caustMap[loc].push_back(p);
        }
        caustPhotonCount += 1;
    } else {
        if (indirMap[loc].size() == 0) {
            indirMap[loc] = fill;
        } else {
            indirMap[loc].push_back(p);
        }
        indirPhotonCount += 1;
    }
}

Color3f PhotonMap::accumAllPhotonsWithinRadius(Point3f p) {
    int locCenter = worldToMapIndex(p);

    int min = 0;
    int max = (int)(glm::ceil(dimx) * glm::ceil(dimy) * glm::ceil(dimz) );

    float calcRad = 0.1f;

    //glm::floor(fabs(p.y - min.y) / this->voxelDiv) * glm::ceil(dimz)

    int zeroCount = 0;
    int totalNumPhotons = 0;

    Color3f colIndir = Color3f(0.0f);
    Color3f colCaust = Color3f(0.0f);

    float minRad = 0.0f;

    while (totalNumPhotons < 4 && calcRad <= 0.5f) {
        int minXBox = glm::floor( (p.x - calcRad) / this->voxelDiv ) * 2.0f * glm::floor(dimy) * glm::floor(dimz) - 1;
        int maxXBox = glm::ceil((p.x + calcRad) / this->voxelDiv ) * 2.0f * glm::ceil(dimy) * glm::ceil(dimz) + 1;
        int minYBox = glm::floor( (p.y - calcRad) / this->voxelDiv ) * 2.0f * glm::floor(dimz) - 1;
        int maxYBox = glm::ceil( (p.y + calcRad) / this->voxelDiv ) * 2.0f * glm::ceil(dimz) + 1;
        int minZBox = glm::floor( (p.z - calcRad) / this->voxelDiv ) * 2.0f - 1;
        int maxZBox = glm::ceil( (p.z + calcRad) / this->voxelDiv ) * 2.0f + 1;

        // bound the bounds to the voxel grid's dimensions
        minXBox = glm::max(min, minXBox);
        minYBox = glm::max(min, minYBox);
        minZBox = glm::max(min, minZBox);
        maxXBox = glm::min(max, maxXBox);
        maxYBox = glm::min(max, maxYBox);
        maxZBox = glm::min(max, maxZBox);

         // loop through the map locations within the squareRad
         for (int i = minXBox; i <= maxXBox; i++) {
             for (int j = minYBox; j <= maxYBox; j++) {
                 for (int k = minZBox; k <= maxZBox; k++) {
                     int loc = glm::min(i + j + k, max);

                     for (Photon* phot: indirMap.at(loc)) {
                         float len = glm::length(phot->pos - p);
                         if (len < calcRad && len > minRad) {
                            colIndir += (*phot->alpha) * phot->col;

                            if (phot->col == Color3f(0.0f)) { zeroCount += 1; }

                            totalNumPhotons += 1;
                         }
                     }

                     for (Photon* phot: caustMap.at(loc)) {
                         float len = glm::length(phot->pos - p);
                         if (len < calcRad && len > minRad) {
                            colCaust += (*phot->alpha) * phot->col;

                            totalNumPhotons += 1;
                         }
                     }
                 }
             }
        }

        if (totalNumPhotons < 10) {
            minRad = calcRad;
            calcRad += 0.1f;
        }
    }

//    float worldRad = (float) voxelSquareRad * glm::max(dimx, glm::max(dimy, dimz));
    float area =  M_PI * calcRad * calcRad;

    Color3f finCol (0.0f);
    if (totalNumPhotons != 0) {
        finCol = (colIndir + colCaust) / area; /* (float)(totalNumPhotons));*/ //(float)(totalNumPhotons); //(float)numPhotons;//area;  //----------- TOTAL NUM OF PHOTONS WITHIN THIS AREA
    }

    //std::cout<<"finColor: " << finCol.r <<","<< finCol.g<<","<<finCol.b<<std::endl;

    if (finCol == Color3f(0.0f) && (zeroCount != 0)) {
       // std::cout<<std::endl;
    }

    return finCol;
}

void PhotonMap::buildMapStructure(std::shared_ptr<Sampler> sample) {
    QList<std::shared_ptr<Light>> allLights = currS->lights;

    int numBounces = this->depth;
    int photonFlag = 0;

    // for each light in the scene shoot the proper number of photons from it into the scene to build the
    //      requested map
    for (std::shared_ptr<Light> l: allLights) {

        Vector3f a = Vector3f(1.0f, 1.0f, 1.0f);
        Vector3f *aCaust = new Vector3f(1.0f, 1.0f, 1.0f);
        Vector3f *aIndir = new Vector3f(1.0f, 1.0f, 1.0f);
        *aCaust *= l.get()->powerVal(); // / 20.0f;
        *aIndir *= l.get()->powerVal(); // / 20.0f;  // FILL THESE IN PROPERLY AT END BY DIVIDING BY TOTAL NUM PHOTONS

        float numPhotonsForCaust = 0.0f;
        float numPhotonsForIndir = 0.0f;

        for (int i = 0; i < numPhotons; i++) {

            // set up photon
            Photon* p = new Photon(photonFlag);
            photonFlag += 1;

            // set up ray information
            Intersection isxTest = Intersection();
            Vector3f d = l.get()->getDir(sample, &isxTest);
            Point3f o = isxTest.point;

            Intersection temp = Intersection();
            temp.point = o;
            Ray r = temp.SpawnRay(d);

            BxDFType prevSampled;

            bool stop = false;

            Color3f prevFVal = Color3f(0.0f);
            float prevAbsDot = 0.0f;
            float prevPdf = 0.0f;

            for (int b = 0; b < numBounces && !stop; b++) {
               bool firstBounce = (b==0);

               Intersection isx = Intersection();
               bool hitScene = currS->Intersect(r, &isx);

               if (hitScene && firstBounce) {
                   bool sameLight = (isx.objectHit->areaLight.get() == l.get());
                   if (sameLight) {
                       r.origin = isx.point + Vector3f(0.1f, 0.1f, 0.1f) * r.direction;
                       isx = Intersection();
                       hitScene = currS->Intersect(r, &isx);
                   }

                   p->col = l.get()->Sample_Li(Intersection(), sample->Get2D(), new Vector3f(0.0f), new Float(0.0f)); //this-> //((AreaLight*) l.get())->L(isx, r.direction);

//                   p->col = l.get()->L(isx, r.direction); //Vector3f(1.0f, 1.0f, 1.0f) * l.get()->powerVal() / 500.0f;
               }

               bool doingPhotonBounce = hitScene && isx.objectHit->material != nullptr;

               // hit valid obj in scene [ie not another light]
               if (doingPhotonBounce) {

                   // check material at the scene
                   isx.objectHit->ProduceBSDF(&isx);

                   Vector3f woW = -1.0f*glm::normalize(r.direction);
                   Vector3f wiW; // filled in in Sample_f
                   float pdf = 0.0f;
                   BxDFType type = BSDF_ALL;
                   BxDFType sampledType; // filled in in Sample_f
                   Color3f f_val = isx.bsdf.get()->Sample_f(woW, &wiW, sample->Get2D(), &pdf, type, &sampledType);
                   float absDot = glm::abs(glm::dot(wiW, isx.normalGeometric));

                   Color3f photonCol = Color3f(0.0f);
                   if (pdf != 0.0f && !firstBounce) { //-----------------------------------------------------
                       photonCol = p->col * (f_val * absDot); // pdf;
//                       photonCol = p->col * prevFVal * prevAbsDot; // / prevPdf; //--------------------------
                   }

                   bool storePhoton = (photonCol != Color3f(0.0f)); ///////-----------------

                   if (storePhoton) {
                       // store the photon properly
                       Photon* ph = new Photon(photonFlag);
                       photonFlag += 1;

                       ph->pos = isx.point;
                       ph->dirIncid = wiW;
                       ph->col = photonCol;

                       if (firstBounce) {
                           // if first intersection then just bounce
                       } else {
                           // if prev specular & !curr specu = put in caust map
                           if ( ((prevSampled & BSDF_SPECULAR) > 0) && !((sampledType & BSDF_SPECULAR) > 0) ) {
                               ph->alpha = aCaust;

                               addToMap(ph, true); // TRUE for caust

                               numPhotonsForCaust += 1.0f;

                           // if prev nonspecular & !curr spec = put in indir map
                           } else if ( ( !(prevSampled & BSDF_SPECULAR) > 0) && ( !(sampledType & BSDF_SPECULAR) > 0) ) {
                               ph->alpha = aIndir;

                               addToMap(ph, false); // false for indir

                                numPhotonsForIndir += 1.0f;

                           }// if prev nonspecular & curr spec OR if prev specular & curr specu = just bounce
                       }

                       // adjust curr photons color for next iteration
                       *p = *ph;
                       // bounce the ray
                       r = isx.SpawnRay(wiW);
                       // update prev type
                       prevSampled = sampledType;

                   } // end: storing the photon

                   /** **************** **/
                   /** RUSSIAN ROULETTE **/
                   /** **************** **/
                   bool russian = false;
                   // check for russian roulette termination to break/exit the loop
                   if (numBounces - b >= 3) {
                       float q = std::max(0.05f, (1.0f - std::max(a.r, std::max(a.g, a.b) ) ));

                       russian = (sample->Get1D() < q);
                       a /= (1.0f - q);
                   }

                   // update prev Photon bounce vals --------------------------------------------
                   prevFVal = f_val;
                   prevAbsDot = absDot;
                   prevPdf = pdf;

                   stop = (!doingPhotonBounce || russian || (!storePhoton && !firstBounce));

                }// end: bounced this photon

            } // end: bouncing current photon
        } // end: creating all the photons

        *aCaust /= numPhotonsForCaust; // FINISHING FILLING IN ALPHA VALUES PROPERLY
        *aIndir /= numPhotonsForIndir;

        std::cout<<std::endl;

    } // end: for each light

    //std::cout<<"numPhotons: caust: " << this->caustPhotonCount << " indir: " << this->indirPhotonCount<<std::endl;
}
