#include "bsdf.h"
#include <warpfunctions.h>

BSDF::BSDF(const Intersection& isect, float eta /*= 1*/)
    //TODO: Properly set worldToTangent and tangentToWorld - DONE
    : worldToTangent(glm::transpose(Matrix3x3(isect.tangent, isect.bitangent, isect.normalGeometric))),
      tangentToWorld(Matrix3x3(isect.tangent, isect.bitangent, isect.normalGeometric)),
      normal(isect.normalGeometric),
      eta(eta),
      numBxDFs(0),
      bxdfs{nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}
{
//    if (isnan(isect.tangent.x) || isnan(isect.tangent.y) || isnan(isect.tangent.z) ) {
//        throw;
//    }
//    if (isnan(isect.bitangent.x) || isnan(isect.bitangent.y) || isnan(isect.bitangent.z) ) {
//        throw;
//    }
//    if (isnan(isect.normalGeometric.x) || isnan(isect.normalGeometric.y) || isnan(isect.normalGeometric.z) ) {
//        throw;
//    }
}

BSDF::~BSDF(){
    for (int i = 0; i < numBxDFs; i++) {
        delete bxdfs[i];
    }
}

void BSDF::UpdateTangentSpaceMatrices(const Normal3f& n, const Vector3f& t, const Vector3f b)
{
    //TODO: Update worldToTangent and tangentToWorld based on the normal, tangent, and bitangent - DONE
//    if (isnan(b.x) || isnan(b.y) || isnan(b.z)) {
//        throw;
//    }
//    if (isnan(t.x) || isnan(t.y) || isnan(t.z)) {
//        throw;
//    }
//    if (isnan(n.x) || isnan(n.y) || isnan(n.z)) {
//        throw;
//    }

    worldToTangent = glm::transpose(Matrix3x3(t,b,n));
    tangentToWorld = glm::inverse(worldToTangent);

//    tangentToWorld = Matrix3x3(t, b, n);
//     worldToTangent = glm::transpose(tangentToWorld);
}

Color3f BSDF::f(const Vector3f &woW, const Vector3f &wiW, BxDFType flags /*= BSDF_ALL*/) const
{
    //TODO - DONE

    Color3f sum = Color3f(0.0f);

    // currently woW and wiW are in world space but need to be in tangent space (wo and wi)
    Vector3f wo = (worldToTangent*woW);
    Vector3f wi = (worldToTangent*wiW);

    //return Color3f(0.f);
    for (int i=0; i<this->numBxDFs; i++) {
        BxDF *b = this->bxdfs[i];
        if (b->MatchesFlags(flags)) {
            sum += b->f(wo, wi);
        }
    }

    return sum;
}

// STEP1
// Use the input random number _xi_ to select
// one of our BxDFs that matches the _type_ flags.
//
// STEP2
// After selecting our random BxDF, rewrite the first uniform
// random number contained within _xi_ to another number within
// [0, 1) so that we don't bias the _wi_ sample generated from
// BxDF::Sample_f.
//
// STEP3
// Convert woW and wiW into tangent space and pass them to
// the chosen BxDF's Sample_f (along with pdf).
// Store the color returned by BxDF::Sample_f and convert
// the _wi_ obtained from this function back into world space.
//
// STEP4
// Iterate over all BxDFs that we DID NOT select above (so, all
// but the one sampled BxDF) and add their PDFs to the PDF we obtained
// from BxDF::Sample_f, then average them all together.
//
// STEP5
// Finally, iterate over all BxDFs and sum together the results of their
// f() for the chosen wo and wi, then return that sum. -- also ignore the randomly found BxDF

// additions for specific BxDFs:
// for specular: if randomly chosen is specular then return f val as random's f
//      and set pdf as random's pdf
//      but if random is not specular - skip over any specular bxdfs when calculating pdf and final
//      f that's returned

Color3f BSDF::Sample_f(const Vector3f &woW, Vector3f *wiW, const Point2f &xi,
                       float *pdf, BxDFType type, BxDFType *sampledType) const
{
    //TODO : implement - DONE

    /********* STEP 1 *********/ //----> to setup for step 3
    Point2f usingRand = xi;

    // selecting random BxDF that matches the given type
    //      note: assuming rand values in xi are predeclared in a prev func
    std::vector<BxDF*> matchingType;
    for (int i=0; i<this->numBxDFs; i++) {
        BxDF* b = bxdfs[i];
        if (b->MatchesFlags(type)) {
            matchingType.push_back(b);
        }
    }
    //int indexRand = glm::ceil( usingRand[0]*(matchingType.size() - 1) );
    int indexRand = std::min(std::floor(xi[0] * matchingType.size()), (float)(matchingType.size()) );
    BxDF* randBxDF = matchingType[indexRand];

    /********* STEP 2 *********/ //----> preventing biasing
    //rewrite the used rand so dont bias the _wi_ sample
    //can't use rand bc c++ - so making random number deterministically
    //usingRand[0] = glm::clamp( xi[0] * (indexRand + 1) / (this->numBxDFs * 1.0f), 0.0f, 1.0f);
    usingRand[0] = xi[0] * matchingType.size() - indexRand;

    /********* STEP 3 *********/ //----> to calculate wo and wi vectors in tangent space for future calcs
    // currently woW and wiW are in world space but need to be in tangent space
    //  when inputted into the BxDF Sample_f methods
    Vector3f wo = (worldToTangent * woW);
    Vector3f wi; //to be found in the Sample_f method of randBxDF
    float randCalcPDF = 0.0f;

    Color3f randColor = randBxDF->Sample_f(wo, &wi, usingRand, &randCalcPDF, sampledType);

//    if (isnan(wi.x) || isnan(wi.y) || isnan(wi.z)) {
//        std::cout<<std::endl;
//    }

    // ^ saving outputed color
    // make sure wow and wiw are now back in world space
    // orig wow still in world space bc given proper but need to convert tan found wiw to world space
    *wiW = (tangentToWorld * wi);
//    if (isnan(wiW->x) || isnan(wiW->y) || isnan(wiW->z)) {
//        std::cout<<std::endl;
//    }

    // if randomly chosen is specular
    if (randBxDF->MatchesFlags(BSDF_SPECULAR)) {
        *pdf = randCalcPDF / this->numBxDFs;
        return randColor;
    }

    /********* STEP 4 *********/ //----> to fill in the inputted pdf value
    // iterating over all BxDF's but the one we used and sum up to ultimately average the pdf's calculated
    float sum = randCalcPDF;
    float count = 1.0f;

    //debugging stuff
    float pdfs[8];

    for (int i=0; i<this->numBxDFs; i++) {
        BxDF* b = bxdfs[i];
        // skipping the rand because already calced his color so dont need to recalc
        // skipping specular vals bc specular pdf is 1 at wi of reflected ray but 0 otherwise so not accurate for
        //      our summation
        if (b != randBxDF && !b->MatchesFlags(BSDF_SPECULAR) /*&& b->type != BxDFType::BSDF_REFLECTION*/) {
            // calcing individual pdf's of prev non used BxDFs
            pdfs[i] = b->Pdf(wo, wi);
            sum += b->Pdf(wo, wi);
            count += 1.0f;
        }
    }

    // filling in the inputed pointer value properly for *pdf
    *pdf = (sum / count);

    /********* STEP 5 *********/ //----> doing the final summation for f's to calc the full output of colors
    // summing all the BxDF's f calculations and return the sum
    Color3f outputColor = randColor;
    for (int i=0; i<this->numBxDFs; i++) {
        BxDF* b = bxdfs[i];
        // skipping the rand because already calced his color so dont need to recalc
        // skipping specular vals bc specular color is 0 at not proper wi
        if (b != randBxDF && !b->MatchesFlags(BSDF_SPECULAR) /*&& b->type != BxDFType::BSDF_REFLECTION*/) {
            // calcing individual f's of prev non used BxDFs
            outputColor += b->f(wo, wi);
        }
    }

    if (std::isnan(*pdf)) {
        std::cout<<std::endl;
    }

    return outputColor;
}


float BSDF::Pdf(const Vector3f &woW, const Vector3f &wiW, BxDFType flags) const
{
    //TODO - DONE

    // currently woW and wiW are in world space but need to be in tangent space
    //  when inputted into the BxDF Sample_f methods
    Vector3f wo = worldToTangent * woW;
    Vector3f wi = worldToTangent * wiW;

    float pdfSum = 0.0f;
    float count = 0.0f;

    for (int i=0; i<this->numBxDFs; i++) {
        BxDF* b = bxdfs[i];
        if (b->MatchesFlags(flags)) {
            // calcing individual pdf's
            pdfSum += b->Pdf(wo, wi);
            count += 1.0f;
        }
    }
    // returning the average val
    if (count == 0.0f) {
        return 0.0f;
    }
    return (pdfSum / count);
}

/********************************************/
/********************************************/
/***************  BxDF  *********************/
/********************************************/
/********************************************/

Color3f BxDF::Sample_f(const Vector3f &wo, Vector3f *wi, const Point2f &xi,
                       Float *pdf, BxDFType *sampledType) const
{
    //TODO - DONE

    // filling in input value not prev declared
    *sampledType = this->type;

    // generate wi ray based on warp functions from hw3 for uniform distribution within the hemisphere
    *wi = WarpFunctions::squareToHemisphereUniform(xi);
    *pdf = this->Pdf(wo, *wi);

    return this->f(wo, *wi);
}

// The PDF for uniform hemisphere sampling
float BxDF::Pdf(const Vector3f &wo, const Vector3f &wi) const
{
    //    return SameHemisphere(wo, wi) ? Inv2Pi : 0; //--> what was orig here for base
    return WarpFunctions::squareToHemisphereUniformPDF(wi);
}
