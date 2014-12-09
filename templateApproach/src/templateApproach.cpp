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

void detectAponeuroses(const Mat& src)
{
	Mat dst, color_dst;


	  //  Canny( src, dst, 200, 100, 7 );

	adaptiveThreshold(src,dst,255,CV_ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY, 65,0);

	int erosion_size=1;

	 Mat element = getStructuringElement( MORPH_RECT,
	                                       Size( 2*erosion_size + 1, 2*erosion_size+1 ),
	                                       Point( erosion_size, erosion_size ) );


	 erode(dst,dst,element);
	 dilate(dst,dst,element);
	 erode(dst,dst,element);
	 dilate(dst,dst,element);
	 erode(dst,dst,element);
		 dilate(dst,dst,element);
		 erode(dst,dst,element);
			 dilate(dst,dst,element);






	cvtColor( dst, color_dst, CV_GRAY2BGR );




	    imshow("dst", dst);

	#if 0
	    vector<Vec2f> lines;
	    HoughLines( dst, lines, 1, CV_PI/180, 100 );

	    for( size_t i = 0; i < lines.size(); i++ )
	    {
	        float rho = lines[i][0];
	        float theta = lines[i][1];
	        double a = cos(theta), b = sin(theta);
	        double x0 = a*rho, y0 = b*rho;
	        Point pt1(cvRound(x0 + 1000*(-b)),
	                  cvRound(y0 + 1000*(a)));
	        Point pt2(cvRound(x0 - 1000*(-b)),
	                  cvRound(y0 - 1000*(a)));
	        line( color_dst, pt1, pt2, Scalar(0,0,255), 3, 8 );
	    }
	#else
	    vector<Vec4i> lines;
	    HoughLinesP( dst, lines, 1, CV_PI/180, 300, src.cols/2, 5 );
	    for( size_t i = 0; i < lines.size(); i++ )
	    {
	        line( color_dst, Point(lines[i][0], lines[i][1]),
	            Point(lines[i][2], lines[i][3]), Scalar(0,0,255), 3, 8 );
	    }
	#endif
	    namedWindow( "Source", 1 );
	    imshow( "Source", src );

	    namedWindow( "Detected Lines", 1 );
	    imshow( "Detected Lines", color_dst );

}


int main()
{
	string path ="/home/tbergmueller/DEV/muscle/01_MuscularArchitecture/Muscle_VascusLateralis/FISCHERM_2.bmp";


	Mat img = imread(path, CV_LOAD_IMAGE_GRAYSCALE);

	Rect ultrasoundROI(258,130,694-258,515-130);

	Mat ultraSound = img(ultrasoundROI);



	detectAponeuroses(ultraSound);


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
