clear all;clc;

% read in all images
images = readInAllImages('Muscle_VascusLateralis/')';

% read in groundtruth
GT = readInGroundTruth('GroundTruth/groundTruth.txt');

% check if GT and images order is consistent
for i=1:length(images)
    if( ~strcmp(GT{1}(i), images(i).name) )
        i
        error('Image order of cell array GT and struct images is not consistent!');
    end
end

% read in results from the template matching approach (from Thomas)
results_template = dlmread('TestResults.csv', ';',1,1);
% where lower aponeurosis was not found, take the upper one:
index = find(results_template(:,1)==-666);
results_template(index,1) = results_template(index,2);

% find angles
error = zeros(length(images),1);
for i=1:length(images)
    angle = findAngle(images(i),0);
    images(i).angle_hough = angle;
    images(i).angle_template = results_template(i,1);
    images(i).gt = GT{2}(i);
    error_hough(i,1) = abs(images(i).gt-images(i).angle_hough);
    error_template(i,1) = abs(images(i).gt-images(i).angle_template);
end

% JUST TO MAKE THE AVERAGING FAIR, SINCE THE TEMPLATE APPROACH CANNOT FIND
% THE APO FOR THIS IMAGE:
error_template(6,1) = error_hough(6,1);

average_error_hough = sum(error_hough(:,1))/length(images)
average_error_template = sum(error_template(:,1))/length(images)

error_both_approaches = [error_hough error_template];

plot(error_both_approaches,'x-');
legend('Hough Transform','Template Matching');
title('Error plot of both approaches');