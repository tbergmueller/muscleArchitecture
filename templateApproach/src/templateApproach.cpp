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

float detectAponeurosesAngle(const Mat& src)
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


	float angle = 0.0;	// default value

    //imshow("dst", dst);

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

	    float maxY = 0.0;
	    int bestApproxIdx = -1;

	    for( size_t i = 0; i < lines.size(); i++ )
	    {
	        line( color_dst, Point(lines[i][0], lines[i][1]),
	            Point(lines[i][2], lines[i][3]), Scalar(0,0,255), 1, CV_AA);

	        float lineMiddlePointY = (lines[i][1] + lines[i][3])/2;
	        if( lineMiddlePointY > maxY && lineMiddlePointY < color_dst.rows/2)
	        {
	        	maxY = lineMiddlePointY;
	        	bestApproxIdx = i;
	        }
	    }


	    cout << "detected " << lines.size() << " aponeuroses candidates" << endl;


	    line( color_dst, Point(lines[bestApproxIdx][0], lines[bestApproxIdx][1]),
	    	            Point(lines[bestApproxIdx][2], lines[bestApproxIdx][3]), Scalar(0,255,0), 1, CV_AA);




		float deltaX = lines[bestApproxIdx][2] - lines[bestApproxIdx][0];
		float deltaY = lines[bestApproxIdx][3] - lines[bestApproxIdx][1];


		angle=  atan2(deltaY, deltaX);



	#endif
	//    namedWindow( "Source", 1 );
	//    imshow( "Source", src );

	    namedWindow( "Detected Lines", 1 );
	    imshow( "Detected Lines", color_dst );



	    return angle;

	    // Simple policy... select the line with the highest Y-Coordinate

}


float getAngleAtPosition(const Mat& ultraSound, const Point& probePoint)
{
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


		// TODO: Better than just one minimum, get N minima
		//cout << "minimum is at " << minLoc << endl;


		//minLoc.x = 19;
		//minLoc.y = 17;

		// calc back

		Point absoluteLocation(startPoint.x + minLoc.x, startPoint.y + minLoc.y);


		float deltaX = absoluteLocation.x - probePoint.x;
		float deltaY = absoluteLocation.y - probePoint.y;


		float angle=  atan2(deltaY, deltaX);
		//cout << "angle is " << angle * 180 / M_PI << endl;

		return angle;
}



void showAngleHisto(const cv::Mat& angleField)
{
	 // Quantize the hue to 30 levels
	    // and the saturation to 32 levels
	    int hbins = 360;
	    int histSize[] = {hbins};

	    float hranges[] = { 0, CV_PI * 2 };

	    const float* ranges[] = { hranges };
	    MatND hist;
	    // we compute the histogram from the 0-th and 1-st channels
	    int channels[] = {0, 1};

	    calcHist( &angleField, 1, channels, Mat(), // do not use mask
	             hist, 1, histSize, ranges,
	             true, // the histogram is uniform
	             false );

	    int scale = 200;
	    Mat histImg = Mat::zeros(scale, hbins, CV_8UC3);



	    for( int h = 0; h < hbins; h++ )
	    {
	    	float binVal = hist.at<float>(h,0);

	    //	cout << "binVal is " << binVal << endl;

	            rectangle( histImg, Point(h, 0),
	                        Point( (h+1), scale*(1- (binVal / (angleField.cols*angleField.rows)))),
	                        Scalar::all(255),
	                        CV_FILLED );
	    }

	    bitwise_not(histImg,histImg);



	    namedWindow( "Angle Histogram", 1 );
	    imshow( "Angle Histogram", histImg );

}

int main(int argc, char** argv)
{

	if(argc != 2)
	{
		cerr << "You have to call the program like 'program.exe <pathToImage>'" << endl;
		return -1;
	}
	string path =argv[1];

	Mat img = imread(path, CV_LOAD_IMAGE_GRAYSCALE);

	if(img.empty())
	{
		cerr << "Could not load image from " << path << endl;
		return -1;
	}

	Rect ultrasoundROI(258,130,694-258,515-130);

	Mat ultraSound = img(ultrasoundROI);


	float apoAngle = detectAponeurosesAngle(ultraSound);
	cout << "Aponeuroses angle is " << apoAngle << endl;
	cout << "Detecting angle field (may take some time)..." << endl;


	int stepSize = 5;
	Rect angleField(18,86,100/stepSize,100 / stepSize);
	Mat angField(angleField.height,angleField.width,CV_32FC1);
	for(int x=0; x<angleField.width; x++)
	{
		for(int y=0; y<angleField.height; y++)
		{
			Point probePoint(x*stepSize + angleField.x, y*stepSize + angleField.y);

			float angle = getAngleAtPosition(ultraSound, probePoint);
			angField.at<float>(y, x) = angle;
		}
	}

//	apoAngle = 0.0;
	// Correct for aponeuroses angle
	angField = angField - apoAngle;

	cout << "done!" << endl;


	Mat fullAngle;
	imshow("fullAngle", angField);


	fullAngle = angField * 180 / CV_PI;

	fullAngle.convertTo(fullAngle, CV_8UC1);

	imshow("visual", fullAngle);

	//cout << "angle is " << getAngleAtPosition(ultraSound, probePoint);

	showAngleHisto(angField);


	// EVALUATION


	cout << "Mean angle is " << mean(angField)[0]*  180 / CV_PI << "°. Please refer histogram for verification!" << endl;

	//imshow("ultrasound", ultraSound);
	waitKey();
	return 0;
}
