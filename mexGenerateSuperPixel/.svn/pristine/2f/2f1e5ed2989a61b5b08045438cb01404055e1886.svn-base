#pragma once
#pragma warning(disable:4996)
#include "../common/mexutils.h"
#include <map>
#define cimg_plugin "cimgmatlab.h"
#include "../common/CImg.h"
#include <process.h>  // for new thread
#include <sstream> // for stringstream
using namespace cimg_library;
using namespace std;
#include "SLIC.h"
//
CImg<unsigned> *segmentImageSP(CImg<uchar> *im, 
                                  float sigma, 
                                  float c, 
                                  int min_size,
                                  int *num_ccs);

// img1: input image 1
// mask: the mask for the region in im1
// img2: input image 2
// return value: the output super pixel indices
vector<int> regionMatch(const CImg<uchar>& img1, const CImg<uchar>& mask, const CImg<uchar>& img2, const CImg<uint>& );

struct rgb_color{
    uchar r;
    uchar g;
    uchar b;
};


template<typename T>
void  showLabelsUsingRandomColor(CImg<T>& img) {
    HANDLE hThread;
    unsigned threadID;
    hThread = (HANDLE)_beginthreadex( NULL, 0, &_showLabelsUsingRandomColor<T>, reinterpret_cast<void*>(&img), 0, &threadID );
    WaitForSingleObject( hThread, 100 );
}

// work horse
template<typename T>
unsigned __stdcall _showLabelsUsingRandomColor(void* p) {

    struct statisticInfo {
        statisticInfo():totalX(0),totalY(0),count(0),meanX(0), meanY(0){}
        uint totalX;
        uint totalY;
        uint meanX;
        uint meanY;
        uint count;
    };

    CImg<T>* ptr = reinterpret_cast<CImg<T>* >(p);
    CImg<T> img(*ptr, false);

    

    assert(img.spectrum() == 1); 
    assert(img.depth() == 1);

    //srand(time(NULL));
    CImg<uchar> result(img.width(), img.height(), 1, 3);
    CImg<uchar> font(img.width(), img.height(), 1, 3, 255);
    T min = img.min(); T max = img.max();
    map<T, statisticInfo> colorIndice;

    cimg_forXY(img, x, y) {
        statisticInfo m;
        pair<map<T, statisticInfo>::iterator, bool> pr = colorIndice.insert(make_pair(img(x, y), m));
        if (!pr.second) { // insert not successfully
            ((pr.first)->second).totalX += x;
            ((pr.first)->second).totalY += y;
            ((pr.first)->second).count += 1;
        }
    }

    const unsigned char black[] = { 0,0,0};
    stringstream s;
    string str;
    int meanX, meanY;
    for (auto it=colorIndice.begin(); it != colorIndice.end(); it++) {
        meanX = int(static_cast<float>((it->second).totalX)/(it->second).count);
        meanY = int(static_cast<float>((it->second).totalY)/(it->second).count);
        (it->second).meanX = meanX;
        (it->second).meanY = meanY;

        s << it->first;
        s >> str;
        s.clear();
        font.draw_text(meanX, meanY, str.c_str(), black, 0);
    }

    
    cimg_forXY(img, x, y){
        auto pr = colorIndice.find(img(x, y));
        int meanX, meanY;
        statisticInfo m = pr->second;
        meanX = (pr->second).meanX;
        meanY = (pr->second).meanY;
        result(x, y, 0) = static_cast<uchar>(meanX*19+meanY*17);
        result(x, y, 1) = static_cast<uchar>(meanX*11+meanY*23);
        result(x, y, 2) = static_cast<uchar>(meanX*13+meanY*47);
    }

    result &= font;
    result.display();

    return 0;
}