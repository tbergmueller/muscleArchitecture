//============================================================================
// Name        : templateApproach.cpp
// Author      : tbergmueller
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;

using namespace std;

#define PROBE_WIDTH		50
#define PROBE_HEIGHT	100

#define MAX_SEARCH_X	PROBE_WIDTH
#define MAX_SEARCH_Y	PROBE_HEIGHT/2

int main()
{
	string path ="/home/tbergmueller/DEV/muscle/01_MuscularArchitecture/Muscle_VascusLateralis/FISCHERM_1.bmp";


	Mat img = imread(path, CV_LOAD_IMAGE_GRAYSCALE);

	Rect ultrasoundROI(258,130,694-258,515-130);

	Mat ultraSound = img(ultrasoundROI);


	Point probePoint(130,161);

	Mat probe = ultraSound(Rect(probePoint.x, probePoint.y, PROBE_WIDTH, PROBE_HEIGHT));

	Mat differenceAccu(MAX_SEARCH_Y, MAX_SEARCH_X, CV_32FC1);


	Point startPoint = Point(probePoint.x+PROBE_WIDTH/2, probePoint.y); // we assume they go downwards...



	for(int dx = startPoint.x; dx < startPoint.x+MAX_SEARCH_X; dx++)
	{
		for(int dy = startPoint.y; dy < startPoint.y+MAX_SEARCH_Y; dy++)
		{
			Rect compareROI(dx,dy, PROBE_WIDTH,PROBE_HEIGHT);

			Mat toCompare = ultraSound(compareROI);

			Mat diff;
			absdiff(toCompare, probe, diff);

			differenceAccu.at<float>(dy-startPoint.y, dx - startPoint.x) = sum(diff)[0] ;
		}
	}

	double minVal, maxVal;
	Point minLoc, maxLoc;

	minMaxLoc(differenceAccu,&minVal, &maxVal, &minLoc, &maxLoc );

	cout << "minimum is at " << minLoc << endl;


	//minLoc.x = 19;
	//minLoc.y = 17;

	// calc back

	Point absoluteLocation(startPoint.x + minLoc.x, startPoint.y + minLoc.y);


	float deltaX = absoluteLocation.x - probePoint.x;
	float deltaY = absoluteLocation.y - probePoint.y;


	float angle=  atan2(deltaY, deltaX);
	cout << "angle is " << angle * 180 / M_PI << endl;


	Mat visual;

	normalize(differenceAccu,visual,0,255,CV_MINMAX, CV_8UC1);

	imshow("bla", visual);

	imshow("probe", probe);





	imshow("ultrasound", ultraSound);
	waitKey();
	return 0;
}
