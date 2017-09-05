#include "directlightingintegrator.h"

Color3f DirectLightingIntegrator::Li(const Ray &ray, const Scene &scene, std::shared_ptr<Sampler> sampler, int depth,
                                     Color3f compoundColor) const
{
    // Scene.intersect returns true and pointer parameter is closest intersection

   // return Color3f(0.0f);
    Intersection isx = Intersection();

    // if no intersection in the scene
    if (! (scene.Intersect(ray, &isx)) ) {
        return Color3f(0.0f);

    // otherwise there was an intersection
    } else {

//        isnan(-1);

        // if spot / point light then do proper accounting for it
        if (isx.objectHit != nullptr && dynamic_cast<IgnoreGeoLight*>(isx.objectHit->shape.get()) != NULL) {
            auto light = isx.objectHit;

            bool inter = true;

            while(isx.objectHit == light && inter) {
                isx.point += ray.direction * 0.5f;
                Ray passThrough = isx.SpawnRay(ray.direction);
                inter = scene.Intersect(passThrough, &isx);
            }
        }

        // emittance (ie if the material itself has its own light output)
        Color3f le = isx.Le(-ray.direction);
        /*** base case ***/
        if (depth == 0 || isx.objectHit->material == nullptr) { // dont do material check bc no Le
            // note: reflecting input ray over the normal so that oriented properly
            //return le; - dont need le since going over what coloring is going into surface
            return le;

        /*** else normal calculations ***/
        } else {

            // actually creating a bsdf attribute based on the intersected location
            isx.objectHit->ProduceBSDF(&isx);

            // randomly select which light to do sample_li with
            int index = glm::floor(sampler->Get1D() * scene.lights.size());

            Vector3f wiW; // filled in in Sample_f
            Vector3f woW = -1.0f*glm::normalize(ray.direction);
            float pdf = 0.0f;

            Color3f li = scene.lights[index]->Sample_Li(isx, sampler->Get2D(), &wiW, &pdf); // get this from the light

            // adjust pdf so divided by total number of lights
            pdf = pdf/(float)(scene.lights.size());

            // still need this even though we're not bouncing, because still need surface to show color
            // vv basically the current illuminated surface color [but not bouncing off of it so reusing same wi]
            Color3f surfaceColor = isx.bsdf.get()->f(woW, wiW, BSDF_ALL); // so f from Light Transport equation

            // checking for occlusion:
            // if the ray we send to the light hits an object before the light we are currently sampling from [thus this includes
            //      all other objects even including other lights] then counts as occluded bc will be sampled at other calls
            //      to this func [note no recursion remember bc direct light sampling]
            float visibility = 1.0f; // is there an object in the way
            Ray toLight = isx.SpawnRay(wiW);
            Intersection isxForLight = Intersection();

            bool isCorrectIntersection = (scene.Intersect(toLight, &isxForLight)
                                          && isxForLight.objectHit->areaLight.get() == scene.lights[index].get());

            // no intersection / not correct intersection then occluded
            if (!isCorrectIntersection) {
                visibility = 0.0f;
            }

            float absDot = glm::abs(glm::dot(wiW, isx.normalGeometric));

            if (pdf == 0) {
                //return le; - dont need le since going over what coloring is going into surface
                return le;
            }

            return  le + visibility * surfaceColor * (li * absDot) / pdf;
        }

    }//end: check for intersection
}
