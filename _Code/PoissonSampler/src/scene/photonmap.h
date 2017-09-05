#pragma once

#ifndef PHOTONMAP_H
#define PHOTONMAP_H

#endif // PHOTONMAP_H

#include "globals.h"
#include "scene.h"
#include "raytracing/ray.h"
#include "lights/light.h"
#include "samplers/sampler.h"

#include "scene/geometry/primitive.h"
#include "bounds.h"

#include "samplers/sampler.h"

#include "globals.h"

class Photon;

class PhotonMap
{
public:
    PhotonMap(int n_photons, float voxelR, Scene* s, std::shared_ptr<Sampler> samp, int recursiveDepth)
        : numPhotons(n_photons), caustPhotonCount(0), indirPhotonCount(0), voxelSquareRad(voxelR), currS(s),
    depth(recursiveDepth){

       bool success = setUpMapObject();
       buildMapStructure(samp);
    }

    // destructor
    ~PhotonMap() { for(std::vector<Photon*> k: indirMap) { k.clear(); }
                   for(std::vector<Photon*> m: caustMap) { m.clear(); }}

    float dimx;
    float dimy;
    float dimz;
    Point3f min;
    Point3f max;
    int depth;

    float voxelDiv = 5.0f;

    int caustPhotonCount;
    int indirPhotonCount;

    int numPhotons;
    int voxelSquareRad;
    Scene* currS;

    std::vector<std::vector<Photon*>> indirMap;
    std::vector<std::vector<Photon*>> caustMap;

    bool setUpMapObject();

    //3d point into to 1d index coordinate for our voxel map structure
    int worldToMapIndex(Point3f p);

    void addToMap(Photon* p, bool trueForCaustFalseForIndir); // true for CAUSTIC false for INDIRECT

    //given an intersection point return the accumulated
    Color3f accumAllPhotonsWithinRadius(Point3f p);

    // to set up the 1d array with the proper length for the map's dimensions
    void buildMapStructure(std::shared_ptr<Sampler> sample);
};

class Photon
{
public:
    Photon() : Photon(-1) {}
    Photon(int f) : Photon(Point3f(0.0f), Vector3f(0.0f), new Vector3f(0.0f), Vector3f(0.0f), f) {}
    Photon(Point3f p, Vector3f w, Vector3f* a, Color3f c, int f)
        : pos(p), dirIncid(w), alpha(a), col(c), flag(f) {}

    Point3f pos;
    Vector3f dirIncid;
    Vector3f* alpha;
    Color3f col;
    int flag;
};
