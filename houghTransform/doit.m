clear all;clc;

% read in all images
images = readInAllImages('Muscle_VascusLateralis/');

% Groundtruth
GT = [13.7; 16.07; 11.7; 12; 13.55; 11.6; 17.2; 10.63; 11.67; 16.5; 12.1; 13.35; 13.57; 19.5; 24.9; 13.5; 14.2; 13.75; 17.4; 18; 10.3; 12.2];

angles = zeros(length(images),4);

% find angles
for i=1:length(images)
    angle = findAngle(images(i).image,0);
    %angles(i) = struct('id',images(i).id,'name',images(i).name,'angle',angle);
    angles(i,1) = images(i).id;
    angles(i,2) = angle;
    angles(i,3) = GT(i);
    angles(i,4) = abs(GT(i)-angle);
    %ImageName = angles(i).name
    %Angle = angles(i).angle
end

average_error = sum(angles(:,4))/length(images)