% Watershed Algorithm
% To view image at step, use command:
% imshow(%ImageName);

clc; clear all;

% Import image, perform grayscale
image = imread('cat.png');
grayImage = 0.299*image(:,:,1) + 0.587*image(:,:,2) + 0.114*image(:,:,3);

% Select a threshold value, create binary image (Using vectorization
% instead of if statement
threshold = 175;
binaryImage = 1*(grayImage>threshold) + 0*(grayImage<threshold);

% Inverse the image, such that white pixels are the foreground
inverseImage = 0*(binaryImage == 1) + 1*(binaryImage == 0);

% Erosion Transform

% Pad image
imageSize = size(inverseImage);
erosionImage = zeros(imageSize(1)+3, imageSize(2)+3);
erosionImage(2:imageSize(1)+1,2:imageSize(2)+1) = inverseImage;

% Apply kernel
kernel = ones(3);

erosionImage = conv2(erosionImage, kernel, 'same');
erosionImage = erosionImage == max(erosionImage(:));

%compareImage = imerode(erosionImage, kernel);


% % Distance Transform
% 
% for i = 1:erosion
% 
% eucliddist = bwdist(erosionImage, 'quasi-euclidean');
% distanceImage = repmat(rescale(eucliddist), [1 1 3]);
