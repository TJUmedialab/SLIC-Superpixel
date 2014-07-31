#include <mex.h>
#include "SLICMask.h"
#include <assert.h>
#include <set>
#include <omp.h>

// corresponding usage 1
void mexGenerateSuperPixelUsage1(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
    

    enum INPUT  {IMG=0, SPNUM, COMPACTNESS, MASK};
    enum OUTPUT {LABELS=0};

    typedef double CommonCImgType;

    CImg<CommonCImgType> image;
    int          spNum;
    double       compactness = 10.0;
    CImg<unsigned> maskImg;

    // make sure the input image is in the range [0,255]
    if(mexCheckType<uint8>( prhs[IMG] )){
        image = CImg<CommonCImgType>(prhs[IMG], true);
    } else if (mexCheckType<double>( prhs[IMG] )) {
        image = CImg<CommonCImgType>(prhs[IMG], true) * 255;
    }

    spNum  = (int)mxGetScalar( prhs[SPNUM] );
    if ( nrhs >= 3 )
        compactness = mxGetScalar( prhs[COMPACTNESS] );
    if ( nrhs == 4 ) 
        maskImg     = CImg<unsigned>(prhs[MASK], true);

    int width  = image.width();
    int height = image.height();
    int* labels = new int[width*height];
    CImg<uint> imgBits = loadMatImgBits<CommonCImgType , uint>(image);

    // check the validility
    if (nrhs == 4) {
        if (maskImg.spectrum()!=1)
            mexErrMsgTxt("The input mask must be single channel!");
        if (maskImg.width() != width ||
            maskImg.height()!= height)
            mexErrMsgTxt("Size of mask must be agree with the original image!");
    }

    int numLabels(0);
    SLICMask slicMask;
    slicMask.SetMask(maskImg);

    slicMask.DoSuperpixelSegmentation_ForGivenNumberOfSuperpixels(imgBits._data, width, height, labels, numLabels, spNum, compactness);
    
    mxArray* lbs  = vec2MxArray(labels, height, width);
    plhs[LABELS] = lbs;

    delete[] labels;
}

// corresponding usage 2
void mexGenerateSuperPixelUsage2(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
    enum INPUT  {IMG=0, NUMARRAY, COMPACTARRAY};
    enum OUTPUT {LABELS=0};
    typedef double CommonCImgType;

    CImg<CommonCImgType> image;
    double*              pyNumArray;
    double*              compactnessArray;
    int                  numOfPy;

    // make sure the input image is in the range [0,255]
    if(mexCheckType<uint8>( prhs[IMG] )){
        image = CImg<CommonCImgType>(prhs[IMG], true);
    } else if (mexCheckType<double>( prhs[IMG] )) {
        image = CImg<CommonCImgType>(prhs[IMG], true) * 255;
    }

    pyNumArray  = mxGetPr( prhs[NUMARRAY] );
    numOfPy     = (int)mxGetNumberOfElements( prhs[NUMARRAY] );
    assert(numOfPy > 0);

    if ( nrhs == 3 )
        compactnessArray = mxGetPr( prhs[COMPACTARRAY] );
    else{

        // not provide the compactnessArray, initial it
        compactnessArray = new double[numOfPy];
        for (int i = 0; i < numOfPy; i++) {
            compactnessArray[i] = 10.0;
        }
    }

    int width  = image.width();
    int height = image.height();
    int* labels = new int[width*height*numOfPy];
    CImg<uint> imgBits = loadMatImgBits<CommonCImgType , uint>(image);

    int numLabels(0);
    SLICMask slicMask;

    slicMask.GenerateSuperpixelPyramid(imgBits._data, width, height, labels, numLabels, pyNumArray, compactnessArray, numOfPy);

    mxArray* lbs  = vec2MxArray(labels, height, width, numOfPy);
    plhs[LABELS] = lbs;

    delete[] labels;
    
    if (nrhs < 3) {
        delete[] compactnessArray;
    }
}


// corresponding usage 4
void mexGenerateSuperPixelUsage3(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
    enum INPUT  {IMG=0, DUMMY, MASK, NUMFOREACHLABEL, COMPATNESSARRAY};
    enum OUTPUT {LABELS=0, TRUENUMOFLABELS};

    typedef double CommonCImgType;
    CImg<CommonCImgType> image(prhs[IMG]);
    CImg<uint> maskImg(prhs[MASK]);
    CImg<CommonCImgType> spNumsForSeg(prhs[NUMFOREACHLABEL]);
    double *compactnessArray = 0;

    
    int totalSpNum = static_cast<int>(maskImg.max() + 1);
    int width  = image.width();
    int height = image.height();

    CImg<uint> imgBits = loadMatImgBits<CommonCImgType , uint>(image);

    if(nrhs == 5)
        compactnessArray = mxGetPr(prhs[COMPATNESSARRAY]);
    else {
        compactnessArray = new double[totalSpNum];
        for (int i = 0; i < totalSpNum; i++) compactnessArray[i]=10.0;
    }

    if (spNumsForSeg.width()*spNumsForSeg.height() != totalSpNum)
        mexErrMsgTxt("spNum != maskImg.max() + 1");
    if (maskImg.spectrum()!=1)
        mexErrMsgTxt("The input mask must be single channel!");
    if (maskImg.width() != width || maskImg.height()!= height)
        mexErrMsgTxt("Size of mask must be agree with the original image!");

    int numLabels(0);
    int labelNum = 0;


    int* finalLabels = new int[width*height];
    memset(finalLabels, 0xFF, sizeof(int)*width*height);

    int maxSize = width*height;
    
    
#pragma omp parallel for num_threads(8)
    for (int i = 0; i < totalSpNum; i++) {
        if (spNumsForSeg(i) > 1) {
            double spNumForThisSeg = spNumsForSeg(i);
            
            int* labels = new int[width*height];
            memset(labels, 0, sizeof(int)*width*height);

            // m is the binary valued mask than indicating the region we are interested in
            CImg<uint> m(width, height, 1, 1, 0);

            for (int t = 0; t < width*height; t++) {
                m(t) = maskImg(t) == i;
            }

            int* tmp = new int[maxSize];
            memset(tmp, 0xFF, sizeof(int)*maxSize);

            double area = m.sum();
            int spNumForWholeImg = int(spNumForThisSeg*width*height/area);
            SLICMask slicMask;
            slicMask.SetMask(m);
            slicMask.DoSuperpixelSegmentation_ForGivenNumberOfSuperpixels(imgBits._data, width, height, labels, numLabels, spNumForWholeImg, compactnessArray[i]);
            
            for (int s = 0; s < width*height; s++) {
                if (m[s]) {
                    if (tmp[labels[s]] == -1)
                        tmp[labels[s]] = labelNum++;
                    
                    //#pragma  omp atomic
                    finalLabels[s] = tmp[labels[s]];
                }
            }
            
            delete[] tmp;
            delete[] labels;
        }
    }

    int* tmp = new int[maxSize];
    memset(tmp, 0xFF, sizeof(int)*maxSize);
    for (int s = 0; s < width*height; s++)
        if(finalLabels[s] == -1) {
            if (tmp[maskImg(s)] == -1) {
                tmp[maskImg(s)] = labelNum++;
            }
            finalLabels[s] = tmp[maskImg(s)];
        }
    delete[] tmp;

    mxArray* outputLabel = vec2MxArray(finalLabels, height, width);

    plhs[LABELS] = outputLabel;
    if (nlhs == 2) {
        int *nums = new int[totalSpNum];
        
        SLICMask slicMask;
        slicMask.SetMask(maskImg);
        slicMask.GetLabelNumsInEachSeg(finalLabels, nums);
        mxArray* outputNums = vec2MxArray(nums, totalSpNum, 1);
        plhs[TRUENUMOFLABELS] = outputNums;
        delete[] nums;
    }

    if (nrhs < COMPATNESSARRAY + 1) delete[] compactnessArray;
}

// Functionality 4: Given the number of pyramid level and/or the compactness array for each level, build the super
//                  pixel pyramid
//     [ spPyramid ] = mexGenerateSuperPixel(img, [], superPixelPyLevels, [ compactnessArray ]): calc the super pixel pyramid
//     
//     Input parameter(s):
//
//         img                  : An RGB image of size R x C x 3
//         []                   : An empty matrix
//         superPixelPyLevels   : The desired level of the super pixel levels
//         compactnessArray     : An array of length $superPixelPyLevels$ indicating the compactness used in each level of pyramid
//
//     Output parameter(s):
//         spPyramid            : A matrix of size R x C x $superPixelPyLevels$ represents the  super pixel pyramid of 
//                                $img$. Each slice of the matrix is one level of the pyramid. In each level of the pyramid,
//                                the label of the super pixel starts from 0.
void mexGenerateSuperPixelUsage4(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
    enum INPUT  {IMG=0, DUMMY, PYNUM, COMPACTNESS};
    enum OUTPUT {LABELS=0};
}

//Usage:
//
// Functionality 1: Generate the super pixels for the given image. 
//
//     [ superPixel ] = mexGenerateSuperPixel(img, superPixelNum, [ compactness ], [ mask ]): 
//     
//     Input parameter(s):
//
//         img            : An RGB image of size R x C x 3
//         superpixelNum  : The desired number of super pixels
//         compactness    : The compactness for generating the super pixels. Default value, 10
//         mask           : If this parameter is given, the generated super pixels is strictly obey the boundaries
//                          of it; otherwise, no boundaries constraint is imposed on the generated super pixel
//
//     Output parameter(s):
//         superPixel     : The super pixels of the given image. The super pixel label starts from 0
//
// Functionality 2: Given the super pixel number and/or the compactness of each level of the pyramid, build the super
//          pixel pyramid
// 
//     [ spPyramid ] = mexGenerateSuperPixel(img, superPixelNumArray, [ compactnessArray ] );
//
//     Input parameter(s):
// 
//         img                  : An RGB image of size R x C x 3
//         superPixelNumArray   : A row or col vector of size N indicating number of super pixels in each level
//                                of the pyramid. The number in the vector must be increasing order.
//         compactnessArray     : A row or col vector of the same size of superPixelNumArray, indicating the 
//                                compactness for each level of the pyramid. Default value for each level: 10
//
//     Output parameter(s):
//
//         spPyramid            : A matrix of size R x C x N represents the  super pixel pyramid of img. Each
//                                slice of the matrix is one level of the pyramid. The number of super pixels in 
//                                level i(1<=i<=N) is close to superPixelNumArray(i). In each level of the pyramid,
//                                the label of the super pixel starts from 0.
//
//
//
// Functionality 3: Split one or more super pixels (regions) in an image into multiple ones.
//
//     [superPixel, [numOfSubregions] ] = mexGenerateSuperPixel(img, [], regions, numForEachLabel, [ compactnessArray ])
//
//     Input parameter(s):
//
//         img                  : An RGB image of size R x C x 3
//         []                   : An empty matrix
//         regions              : An integer-valued mask, different values represents different regions of $img$.
//                                The labels of the regions must start from 0 and be continuous.
//         numForEachRegion     : An vector whose length is number of regions defined in $regions$. The i-th element
//                                in this parameter is the desired number of subregions that you want to split (i-1)-th
//                                region into. If the i-th element is less than 2, we do not split the corresponding region.
//         compactnessArray     : An vector whose length is the same as $numForEachRegion$. It indicates the compactness
//                                you want to use when splitting each regions. Default value: 10
//
//     Output parameter(s):
//
//         superPixel           : The original regions and the split regions. Note the region number in $regions$ may 
//                                be re-labeled but it is still continuous labeled and starts from 0.
//         numOfSubregions      : An vector whose length is the same as $numForEachRegion$. The i-th element in this parameter
//                                is the true number of subregions that the corresponding region is split into.
//
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
        typedef unsigned char uchar;
        typedef unsigned int  uint;
        
        enum {SECONDINPUT = 1, THIRDINPUT};
        
        if (nrhs < 2 ) {
            
            mexErrMsgTxt("Not enough input!");

        } else if (nrhs > 5) {
            
            mexErrMsgTxt("Two much input!");

        } else if ( mxIsEmpty( prhs[SECONDINPUT] ) ) {  // belongs to the 3rd usage
            
            if (mxGetNumberOfElements(prhs[THIRDINPUT]) == 1)
                mexGenerateSuperPixelUsage4(nlhs, plhs, nrhs, prhs);
            else
                mexGenerateSuperPixelUsage3(nlhs, plhs, nrhs, prhs);

        } else if ( mxGetNumberOfElements(prhs[SECONDINPUT])!= 1 ) { // belongs to the 2nd usage
            
            mexGenerateSuperPixelUsage2(nlhs, plhs, nrhs, prhs);

        } else { // belongs to the 1st usage

            mexGenerateSuperPixelUsage1(nlhs, plhs, nrhs, prhs);

        }
}
