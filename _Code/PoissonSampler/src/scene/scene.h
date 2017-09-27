#pragma once
#include <QList>
#include <scene/camera.h>

#include <scene/geometry/mesh.h>

class Scene
{
public:
    Scene();
    ~Scene();
    QList<Mesh*> all_mesh;
    Camera camera;

//    QList<std::shared_ptr<Drawable>> drawables;

    void SetCamera(const Camera &c);

    void CreateTestScene();
    void Clear();

    bool Intersect(const Ray& ray, Intersection* isect) const;
};
