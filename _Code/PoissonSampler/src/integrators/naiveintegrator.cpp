#include "naiveintegrator.h"

Color3f NaiveIntegrator::Li(const Ray &ray, const Scene &scene, std::shared_ptr<Sampler> sampler, int depth,
                            Color3f compoundColor) const
{
    // Scene.intersect returns true and pointer parameter is closest intersection
    Intersection isx = Intersection();

    // if no intersection in the scene
    if (! (scene.Intersect(ray, &isx))) {
        return Color3f(0.0f);

    // otherwise there was an intersection
    } else {

        Color3f le = isx.Le(-ray.direction);
        /*** base case ***/
        if (depth == 0 || isx.objectHit->material == nullptr) {
            // note: reflecting input ray over the normal so that oriented properly
            return le;

        /*** else normal recursion ***/
        } else {

            // if spot / point light then do proper accounting for it
            if (isx.objectHit != nullptr && dynamic_cast<IgnoreGeoLight*>(isx.objectHit->shape.get()) != NULL) {
                auto light = isx.objectHit;

                bool inter = true;

                while(isx.objectHit == light && inter) {
                    isx.point += ray.direction * 0.1f;
                    Ray passThrough = isx.SpawnRay(ray.direction);
                    inter = scene.Intersect(passThrough, &isx);
                }
            }

            // emittance (ie if the material itself has its own light output)

            isx.objectHit->ProduceBSDF(&isx);
            //BSDF* usingBSDF = isx.bsdf.get();

            // doing sample_f to get color val for f bc it fills in the missing vars and its final
            //      output is the color returned by f
            Vector3f woW = -1.0f*glm::normalize(ray.direction);
            Vector3f wiW; // filled in in Sample_f
            float pdf = 0.0f;
            BxDFType type = BSDF_ALL;
            BxDFType sampledType; // filled in in Sample_f
            Color3f f_val = isx.bsdf.get()->Sample_f(woW, &wiW, sampler->Get2D(), &pdf, type, &sampledType);
            if (std::isnan(f_val[0]) || std::isnan(f_val[1]) || std::isnan(f_val[2])) {
                std::cout<<"f_val IS NAN"<<std::endl;
            }
            if (std::isnan(wiW.x) || std::isnan(wiW.y) || std::isnan(wiW.z)) {
                std::cout<<"wiW IS NAN"<<std::endl;
            }


            Color3f li = this->Li(isx.SpawnRay(wiW), scene, sampler, depth-1, compoundColor);
            if (std::isnan(li[0]) || std::isnan(li[1]) || std::isnan(li[2])) {
                std::cout<<"li IS NAN"<<std::endl;
            }

            float absDot = glm::abs(glm::dot(wiW, isx.normalGeometric));
            if (std::isnan(absDot) || std::isnan(absDot) || std::isnan(absDot)) {
                std::cout<<"li IS NAN"<<std::endl;
            }

            if (pdf == 0) {
                return le;
            }

            Color3f returning = le + (f_val * li * absDot) / pdf;
            if (std::isnan(returning[0]) || std::isnan(returning[1]) || std::isnan(returning[2])) {
                std::cout<<"RETURNING IS NAN"<<std::endl;
            }

            return  returning;
        }

    }//end: check for intersection
}
