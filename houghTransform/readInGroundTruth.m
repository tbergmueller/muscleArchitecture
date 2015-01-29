function [ GT ] = readInGroundTruth(path)
%READINGROUNDTRUTH Summary of this function goes here
%   Detailed explanation goes here

fileID = fopen(path);
text = textscan(fileID,'%s %f');
fclose(fileID);
[~,index] = sort(text{1});
text{1} = text{1}(index);
text{2} = text{2}(index);

patient_unique = unique(text{1});
GT = cell([1 2]);
GT{1} = patient_unique;
GT{2} = zeros(length(GT{1}),1);
for i=1:length(GT{1})
    angle_sum=0;
    angle_count=0;
    for k=1:length(text{1})
        if( strcmp(GT{1}(i),text{1}(k)) )
            angle_sum = angle_sum + text{2}(k);
            angle_count = angle_count + 1;
        end
    end
    GT{2}(i) = angle_sum/angle_count; % calculate mean of angle
end


end

