#include "integrator.h"
#include "math.h"

void Integrator::run()
{
    Render();
}

void Integrator::Render()
{
    // Compute the bounds of our sample, clamping to screen's max bounds if necessary
    // Instantiate a FilmTile to store this thread's pixel colors
    std::vector<Point2i> tilePixels = bounds.GetPoints();
    // For every pixel in the FilmTile:
    for(Point2i pixel : tilePixels)
    {
        /// Make this false when not debugging
        bool debugging = false;

        Point2i pixelCheck = Point2i(103, 123);
        if((pixel.x != pixelCheck.x || pixel.y != pixelCheck.y) && debugging)
        {
            if (pixel.x == pixelCheck.x && pixel.y == pixelCheck.y) {
                std::cout<<std::endl;
            }
            continue;
        }
        Color3f pixelColor(0.f);
        // Ask our sampler for a collection of stratified samples, then raycast through each sample
        std::vector<Point2f> pixelSamples = sampler->GenerateStratifiedSamples();

        int sampleCount = 0;
        for(Point2f sample : pixelSamples)
        {
//            std::cout<<"rendering sample"<<std::endl;
            if (pixel.x == pixelCheck.x && pixel.y == pixelCheck.y && debugging) {
                std::cout<<std::endl;
            }
            sample = sample + Point2f(pixel); // _sample_ is [0, 1), but it needs to be translated to the pixel's origin.
            // Generate a ray from this pixel sample
            Ray ray = camera->Raycast(sample);
            // Get the L (energy) for the ray by calling Li(ray, scene, tileSampler, arena)
            // Li is implemented by Integrator subclasses, like DirectLightingIntegrator
            Color3f L = Li(ray, *scene, sampler, recursionLimit, Color3f(0.0f) /*compound color*/);
            // Accumulate color in the pixel

            if (pixel.x == pixelCheck.x && pixel.y == pixelCheck.y && debugging) {
                if (std::isnan(L.x) || std::isnan(L.y) || std::isnan(L.z)) {
                    throw;
                }
                if (L.x < 0 || L.y < 0 || L.z < 0) {
                    throw;
                }
            }

            pixelColor += L;
            sampleCount += 1;
        }

        // Average all samples' energies
        pixelColor /= pixelSamples.size();

//        if (isnan(pixelColor.x) || isnan(pixelColor.y) || isnan(pixelColor.z)) {
//            throw;
//        }
//        if ((pixelColor.x < 0) || pixelColor.y < 0 || pixelColor.z < 0) {
//            throw;
//        }


        film->SetPixelColor(pixel, glm::clamp(pixelColor, 0.f, 1.f));
    }
    //We're done here! All pixels have been given an averaged color.
}


void Integrator::ClampBounds()
{
    Point2i max = bounds.Max();
    max = Point2i(std::min(max.x, film->bounds.Max().x), std::min(max.y, film->bounds.Max().y));
    bounds = Bounds2i(bounds.Min(), max);
}
