function [ images ] = readInAllImages(PathToImages)
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here

d = dir(PathToImages);

isub = [d(:).isdir]; %# returns logical vector
nameFolds = {d(isub).name}';

d(ismember(nameFolds,{'.','..'})) = [];

for i=1:length(d)
    filename = [PathToImages d(i).name];
    images(i) = struct('id',i,'name',d(i).name,'image',imread(filename));
end


end

