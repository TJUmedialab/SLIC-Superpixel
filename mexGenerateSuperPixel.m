%%
%Usage:
%
% Functionality 1: Generate the super pixels for the given image. 
%
%     [ superPixel ] = mexGenerateSuperPixel(img, superPixelNum, [ compactness ], [ mask ]): 
%     
%     Input parameter(s):
%
%         img            : An RGB image of size R x C x 3
%         superpixelNum  : The desired number of super pixels
%         compactness    : The compactness for generating the super pixels. Default value, 10
%         mask           : If this parameter is given, the generated super pixels is strictly obey the boundaries
%                          of it; otherwise, no boundaries constraint is imposed on the generated super pixel
%
%     Output parameter(s):
%
%         superPixel     : The super pixels of the given image. The super pixel label starts from 0
%
% Functionality 2: Given the super pixel number and/or the compactness of each level of the pyramid, build the super
%          pixel pyramid
% 
%     [ spPyramid ] = mexGenerateSuperPixel(img, superPixelNumArray, [ compactnessArray ] );
%
%     Input parameter(s):
% 
%         img                  : An RGB image of size R x C x 3
%         superPixelNumArray   : A row or col vector of size N indicating number of super pixels in each level
%                                of the pyramid. The number in the vector must be increasing order.
%         compactnessArray     : A row or col vector of the same size of superPixelNumArray, indicating the 
%                                compactness for each level of the pyramid. Default value for each level: 10
%
%     Output parameter(s):
%
%         spPyramid            : A matrix of size R x C x N represents the  super pixel pyramid of img. Each
%                                slice of the matrix is one level of the pyramid. The number of super pixels in 
%                                level i(1<=i<=N) is close to superPixelNumArray(i). In each level of the pyramid,
%                                the label of the super pixel starts from 0.
%
%
%
% Functionality 3: Split one or more super pixels (regions) in an image into multiple ones.
%
%     [superPixel, [numOfSubregions] ] = mexGenerateSuperPixel(img, [], regions, numForEachLabel, [ compactnessArray ])
%
%     Input parameter(s):
%
%         img                  : An RGB image of size R x C x 3
%         []                   : An empty matrix
%         regions              : An integer-valued mask, different values represents different regions of $img$.
%                                The labels of the regions must start from 0 and be continuous.
%         numForEachRegion     : An vector whose length is number of regions defined in $regions$. The i-th element
%                                in this parameter is the desired number of subregions that you want to split (i-1)-th
%                                region into. If the i-th element is less than 2, we do not split the corresponding region.
%         compactnessArray     : An vector whose length is the same as $numForEachRegion$. It indicates the compactness
%                                you want to use when splitting each regions. Default value: 10
%
%     Output parameter(s):
%
%         superPixel           : The original regions and the split regions. Note the region number in $regions$ may 
%                                be re-labeled but it is still continuous labeled and starts from 0.
%         numOfSubregions      : An vector whose length is the same as $numForEachRegion$. The i-th element in this parameter
%                                is the true number of subregions that the corresponding region is split into.
%
