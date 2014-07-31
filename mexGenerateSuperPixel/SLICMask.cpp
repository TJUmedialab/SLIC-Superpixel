#include "SLICMask.h"
#include <assert.h>
#include <set>


void SLICMask::EnforceLabelConnectivity(
		const int*					labels,
		const int					width,
		const int					height,
		int*						nlabels,//input labels that need to be corrected to remove stray labels
		int&						numlabels,//the number of labels changes in the end if segments are removed
		const int&					K) {
    
    //assert(m_mask.width() > 0 && m_mask.height() > 0);

	const int dx4[4] = {-1,  0,  1,  0};
	const int dy4[4] = { 0, -1,  0,  1};

	const int sz = width*height;
	const int SUPSZ = sz/K;
	//nlabels.resize(sz, -1);
	for( int i = 0; i < sz; i++ ) nlabels[i] = -1;
	int label(0);
	int* xvec = new int[sz];
	int* yvec = new int[sz];
	int oindex(0);
	int adjlabel(0);//adjacent label
    int labeledCount(0);
    int whileCount(0);

    while( (labeledCount < sz) && (whileCount < 20)) {
        oindex = 0;
        whileCount++;
	    for( int j = 0; j < height; j++ )
	    {
		    for( int k = 0; k < width; k++ )
		    {
			    if( 0 > nlabels[oindex] )
			    {
                    adjlabel = -1;
				    nlabels[oindex] = label;
				    //--------------------
				    // Start a new segment
				    //--------------------
				    xvec[0] = k;
				    yvec[0] = j;
				    //-------------------------------------------------------
				    // Quickly find an adjacent label for use later if needed
				    //-------------------------------------------------------
				    {for( int n = 0; n < 4; n++ )
				    {
					    int x = xvec[0] + dx4[n];
					    int y = yvec[0] + dy4[n];
					    if( (x >= 0 && x < width) && (y >= 0 && y < height) )
					    {
						    int nindex = y*width + x;
                        
                            // Edited by Li Liang
                            if (m_mask.width() > 0 && m_mask.height() > 0) {
                                if(m_mask(nindex)==m_mask(oindex) && nlabels[nindex] >= 0) {
                                    adjlabel = nlabels[nindex];
                                }
                            } else {
                                if( nlabels[nindex] >= 0) {
                                    adjlabel = nlabels[nindex];
                                }
                            }
					    }
				    }}

				    int count(1);
				    for( int c = 0; c < count; c++ )
				    {
					    for( int n = 0; n < 4; n++ )
					    {
						    int x = xvec[c] + dx4[n];
						    int y = yvec[c] + dy4[n];

						    if( (x >= 0 && x < width) && (y >= 0 && y < height) )
						    {
							    int nindex = y*width + x;

							    if (m_mask.width() > 0 && m_mask.height() > 0) {
							        if( 0 > nlabels[nindex] && labels[oindex] == labels[nindex] && m_mask(oindex)==m_mask(nindex) ) {
								        xvec[count] = x;
								        yvec[count] = y;
								        nlabels[nindex] = label;
								        count++;
							        }
                                }
                                else {
                                    if(0 > nlabels[nindex] && labels[oindex] == labels[nindex]) {
                                        xvec[count] = x;
                                        yvec[count] = y;
                                        nlabels[nindex] = label;
                                        count++;
                                    }
                                }

						    }

					    }
				    }
                    
				    //-------------------------------------------------------
				    // If segment size is less then a limit, assign an
				    // adjacent label found before, and decrement label count.
				    //-------------------------------------------------------
				    if((count <= SUPSZ >> 2))
				    {
					    for( int c = 0; c < count; c++ )
					    {
						    int ind = yvec[c]*width+xvec[c];
						    nlabels[ind] = adjlabel;
					    }
                        //if (adjlabel >= 0 ) 
					        label--;
				    }

                    if ((count > SUPSZ >>2) || adjlabel >=0)
                        labeledCount += count;

				    label++;
			    }
			    oindex++;
		    }
	    }

    }

    // not labeled region
    if (whileCount == 20) {
        assert(m_mask.width() > 0 && m_mask.height() > 0);

        oindex = 0;
        for( int j = 0; j < height; j++ ) {
            for( int k = 0; k < width; k++ ) {
                
                if ( 0 > nlabels[oindex]) {

                    int count(1);
                    for( int c = 0; c < count; c++ ) {
                        for( int n = 0; n < 4; n++ ) {
                            int x = xvec[c] + dx4[n];
                            int y = yvec[c] + dy4[n];

                            if( (x >= 0 && x < width) && (y >= 0 && y < height) ) {
                                int nindex = y*width + x;

                                if (m_mask.width() > 0 && m_mask.height() > 0) {
                                    if( 0 > nlabels[nindex] && m_mask(oindex)==m_mask(nindex) ) {
                                        xvec[count] = x;
                                        yvec[count] = y;
                                        nlabels[nindex] = label;
                                        count++;
                                    }
                                } else {
                                    throw exception("Something wrong!");
                                }
                            }

                        }
                    }
                    label++;
                }
                oindex++;
            }
        }
    }
	numlabels = label;

	if(xvec) delete [] xvec;
	if(yvec) delete [] yvec;
}
void SLICMask::GenerateSuperpixelPyramid (
    const unsigned int*         ubuff,
    const int					width,
    const int					height,
    int*						klabels,
    int&						numlabels,
    const double*				K,//required number of superpixels for each level
    const double*               compactness,
    const int                   numOfPy
    ) {

    for (int py = 0; py < numOfPy; py++) {
        if (py == 0) {
            SetMask(CImg<uint>());
        }
        else{
            CImg<uint> mask(klabels+(py-1)*width*height, width, height, 1, 1);
            SetMask(mask);
        }

        DoSuperpixelSegmentation_ForGivenNumberOfSuperpixels(ubuff, width, height, klabels+py*width*height, numlabels, int(K[py]), compactness[py]);
    }
}

void SLICMask::GetLabelNumsInEachSeg(const int * const labels, __out int *nums) {
    int numOfSegs = int(m_mask.max() + 1);
    std::set<int>* aux = new std::set<int>[numOfSegs];
    
    int pixNum = m_mask.width() * m_mask.height();
    for (int i = 0; i < pixNum; i++) {
        aux[m_mask(i)].insert(labels[i]);
    }

    for (int i = 0; i < numOfSegs; i++) {
        nums[i] = int(aux[i].size());
    }

    delete[] aux;
}