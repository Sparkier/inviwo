#include <inviwo/core/datastructures/transferfunction.h>
#include <stdlib.h>
namespace inviwo {

    TransferFunction::TransferFunction() : Data() {
        alphaValues = new float[256];
        std::fill(alphaValues, alphaValues + 256, 0.5f);
    }
    TransferFunction::~TransferFunction() {
    }

    Data* TransferFunction::clone() {
        TransferFunction* newImage = new TransferFunction();
        copyRepresentations(newImage);
        return newImage;
    }

    void TransferFunction::setAlpha(float* alphaValues_){
        alphaValues = alphaValues_;
    }
    
    float* TransferFunction::getAlpha(){
        return alphaValues;
    }

}
