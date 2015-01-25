function [ angle ] = findAngle(images,plotting_option)
% calculates the angle between the aponeurosis and the fibers via hough
% transform
% use plotting_option=1 for getting the result plotted
%

% cropping image + convert to grayscale
im = imcrop(images.image,[258 130 438 439]);
img = rgb2gray(im);

if plotting_option
    close all; figure; imshow(img);title(images.name);
end

% apply gamma correction in order to enhance white pixels/lines
img = imadjust(img,stretchlim(img),[],2);

 % convert image to binary image by thresholding
img_bin = im2bw(img,0.3);
%img_bin = edge(img,'canny');
%img_bin = edge(img_bin,'canny');

if plotting_option
    figure; imshow(img);
    figure; imshow(img_bin);
end

% detect APONEUROSES

%[H,theta,rho] = hough(img_bin,'RhoResolution',0.5,'Theta',-90:-80);
[H,theta,rho] = hough(img_bin,'RhoResolution',0.5,'Theta',[-90:-80 70:89]);
%[H,theta,rho] = hough(img_bin,'RhoResolution',0.5,'Theta',-84:-20);
%[H,theta,rho] = hough(img_bin);
P = houghpeaks(H,1,'threshold',ceil(0.3*max(H(:))));
lines = houghlines(img_bin, theta, rho,P);

if plotting_option
    figure, imshow(img), hold on
end

max_len = 0;
for k = 1:length(lines)
   xy = [lines(k).point1; lines(k).point2];
   if plotting_option
        plot(xy(:,1),xy(:,2),'LineWidth',2,'Color','green');
   

        % Plot beginnings and ends of lines
        plot(xy(1,1),xy(1,2),'x','LineWidth',2,'Color','yellow');
        plot(xy(2,1),xy(2,2),'x','LineWidth',2,'Color','red');
   end

   % Determine the endpoints of the longest line segment
   len = norm(lines(k).point1 - lines(k).point2);
   if ( len > max_len)
      max_len = len;
      xy_long = xy;
   end
end

aponeuroses_line = lines(1);
% angles = 0;
% for i=1:length(lines)
%    angles = angles + lines(i).theta;
% end
% aponeuroses_angle = angles/length(lines);
% aponeuroses_angle = lines(1).theta;

% highlight the longest line segment
if plotting_option
    plot(xy_long(:,1),xy_long(:,2),'LineWidth',2,'Color','red');
    myTitle = ['Detected Aponeuroses with Angle Theta = ' num2str(lines(1).theta)];
    title(myTitle);
end


% detect MUSCLE FIBERS
img_bin_edge = edge(img_bin,'canny');

%[H,theta,rho] = hough(img_bin_edge,'RhoResolution',0.5,'Theta',-84:-20);
[H,theta,rho] = hough(img_bin_edge,'RhoResolution',0.5,'Theta',-84:-60);
P = houghpeaks(H,25,'threshold',ceil(0.3*max(H(:))));
lines = houghlines(img_bin_edge, theta, rho,P);

if plotting_option
    figure, imshow(img), hold on
end

max_len = 0;
for k = 1:length(lines)
   xy = [lines(k).point1; lines(k).point2];
   if plotting_option
       plot(xy(:,1),xy(:,2),'LineWidth',2,'Color','green');

       % Plot beginnings and ends of lines
       plot(xy(1,1),xy(1,2),'x','LineWidth',2,'Color','yellow');
       plot(xy(2,1),xy(2,2),'x','LineWidth',2,'Color','red');
   end

   % Determine the endpoints of the longest line segment
   len = norm(lines(k).point1 - lines(k).point2);
   if ( len > max_len)
      max_len = len;
      xy_long = xy;
   end
end

% highlight the longest line segment
if plotting_option
    plot(xy_long(:,1),xy_long(:,2),'LineWidth',2,'Color','red');
    myTitle = ['Found Muscle Fibers'];
    title(myTitle);
end

k=1;
for i=1:length(lines)
    if aponeuroses_line.point1(2)<(size(img,1)/2) % if aponeuroses was in the upper half of the image (than the relevant fibers should be below)
        if lines(i).point1(2) > aponeuroses_line.point1(2) % possible relevant muscle fiber
            relevant_lines(k) = lines(i);
            k = k + 1;
        end
    else % aponeuroses was detected in the lower half of the image
        if lines(i).point1(2) < aponeuroses_line.point1(2) % possible relevant muscle fiber
            relevant_lines(k) = lines(i);
            k = k + 1;
        end
    end
end

if plotting_option
    figure, imshow(img), hold on
end

max_len = 0;
for k = 1:length(relevant_lines)
   xy = [relevant_lines(k).point1; relevant_lines(k).point2];
   if plotting_option
       plot(xy(:,1),xy(:,2),'LineWidth',2,'Color','green');

       % Plot beginnings and ends of relevant_lines
       plot(xy(1,1),xy(1,2),'x','LineWidth',2,'Color','yellow');
       plot(xy(2,1),xy(2,2),'x','LineWidth',2,'Color','red');
   end

   % Determine the endpoints of the longest line segment
   len = norm(relevant_lines(k).point1 - relevant_lines(k).point2);
   if ( len > max_len)
      max_len = len;
      xy_long = xy;
   end
end

% highlight the longest line segment
if plotting_option
    plot(xy_long(:,1),xy_long(:,2),'LineWidth',2,'Color','red');
    myTitle = ['Relevant Muscle Fibers'];
    title(myTitle);
end


% calculate the median of the relevant lines (muscle fibers)
relevant_thetas = zeros(length(relevant_lines),1);
for i=1:length(relevant_lines)
    relevant_thetas(i) = relevant_lines(i).theta;
end

theta_mean = mean(relevant_thetas);
calculated_angle = abs(aponeuroses_line.theta - theta_mean);
%calculated_angle = abs(aponeuroses_angle - theta_mean);



angle = calculated_angle;

end

