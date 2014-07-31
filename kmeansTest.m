img  = imread('Penguins.jpg');
[ht, wd, ~] = size(img);
sp   = mexSuperPixInitial(img, 500, 5);
sp   = sp + 1;
spNum = max(sp(:));
points = zeros(2, spNum);


for i=1:spNum
    m = sp==i;
    [r, c] = find(m);
    points(:,i) = [mean(r); mean(c)];
end
x = points(2,:);
y = points(1,:);
%plot(x, y, '.', 'MarkerFaceColor',[0,0,0]);


%yNumOfClustter = floor(sqrt(spNum*ht/wd));
yNumOfClustter = floor(sqrt(spNum*wd/ht));

[C, A] = vl_kmeans(x, yNumOfClustter, 'verbose', 'distance', 'l1', 'algorithm', 'elkan');
%[clustCent,point2cluster,clustter] = MeanShiftCluster(x,.75);

clustter = cell(yNumOfClustter, 1);
for m = 1:yNumOfClustter
    clustter{m} = find(A==m);
end

figure;hold on
for m = 1:yNumOfClustter
    plot(x(clustter{m}), y(clustter{m}), '.', 'MarkerEdgeColor',rand(1,3));
end

segToImg(sp, clustter);
