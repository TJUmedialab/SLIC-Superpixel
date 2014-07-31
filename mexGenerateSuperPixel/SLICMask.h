#include "SLIC.h"
#include "mexutils.h"

class SLICMask: public SLIC{
public:
    CImg<uint> GetMask() const {return m_mask;}
    void       SetMask(const CImg<uint> &mask) {m_mask = mask;}
    //
    //
    virtual void EnforceLabelConnectivity(
		const int*					labels,
		const int					width,
		const int					height,
		int*						nlabels,//input labels that need to be corrected to remove stray labels
		int&						numlabels,//the number of labels changes in the end if segments are removed
		const int&					K);
    void GenerateSuperpixelPyramid (
        const unsigned int*         ubuff,
        const int					width,
        const int					height,
        int*						klabels,
        int&						numlabels,
        const double*				K,//required number of superpixels
        const double*               compactness,
        const int                   numOfPy
        );
    void GetLabelNumsInEachSeg(const int *const labels, __out int *nums);
private:
    CImg<uint>    m_mask;
};