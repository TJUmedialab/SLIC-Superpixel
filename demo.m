img  = imread('frame10.png');
mask = imread('mask.png');

% the basic usage of mexGenerateSuperPixel
sp  = mexGenerateSuperPixel(img, 50);
cntr=get_centers(sp);
size(cntr)
% sp starts from 0, segToImg process the labels number greater than 0
segToImg(sp+1);

%since the sp is integer value and continous, the totalNumOfSP
% can be calculated as follows
totalNumOfSP = max(sp(:)+1);

% this vector is used to demonstrate functionality 3 of
% mexGenerateSuperPixel, type 'help mexGenerateSuperPixel' for more help
nums = ones(totalNumOfSP, 1);
% we want to split region 0 into 10 subregions, 1 into 30 subregions and 10
% into 20 subregions.
nums(1) = 10;
nums(2) = 30;
nums(11) = 20;

[sp5, numsInEachSeg]= mexGenerateSuperPixel(img, [], sp, nums);
segToImg(sp5 + 1);title('sp5');


sp2 = mexGenerateSuperPixel(img, 200, 10, mask);
spPyramid  = mexGenerateSuperPixel(img, [20, 100, 500]);
spPyramid2 = mexGenerateSuperPixel(img, [20, 100, 500], [10, 20, 30]);


% the returned label starts from 0
sp = sp+1;
sp2 = sp2+1;
spPyramid = spPyramid +1;
spPyramid2 = spPyramid2 +1;

segToImg(sp);
segToImg(sp2);

segToImg(spPyramid(:,:,1));
segToImg(spPyramid(:,:,2));
segToImg(spPyramid(:,:,3));

segToImg(spPyramid2(:,:,1));
segToImg(spPyramid2(:,:,2));
segToImg(spPyramid2(:,:,3));