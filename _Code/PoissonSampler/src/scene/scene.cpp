#include <scene/scene.h>

#include <scene/geometry/mesh.h>


Scene::Scene() {}

Scene::~Scene()
{
//    delete bvh;
}

void Scene::SetCamera(const Camera &c)
{   
    camera = Camera(c);
    camera.create();

//    if (usingLensCamera) {
//        SetCamera(LensCamera());
//    }

//    film.SetDimensions(c.width, c.height);
}

bool Scene::Intersect(const Ray &ray, Intersection *isect) const
{
    glm::mat4 viewProj = camera.GetViewProj();

    bool result = false;
    for(Mesh* m : all_mesh){
        for (std::shared_ptr<Triangle> t : m->faces) {
            Intersection testIsect;
            if(t->Intersect(ray, &testIsect, viewProj))
            {
                if(testIsect.t < isect->t || isect->t < 0)
                {
                    *isect = testIsect;
                    result = true;
                }
            }
        }
    }
    return result;
}

void Scene::CreateTestScene()
{
    // test

    camera = Camera(400, 400, Point3f(5, 8, -5), Point3f(0,0,0), Vector3f(0,1,0));
    camera.near_clip = 0.1f;
    camera.far_clip = 100.0f;

    camera.create();

//    film = Film(400, 400);
}

void Scene::Clear()
{
    // These lists contain shared_ptrs
    // so the pointers will be freed
    // if appropriate when we clear the lists.

//    drawables.clear();
    camera = Camera();
//    film = Film();
}
