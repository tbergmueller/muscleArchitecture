/*
 * AngleField.cpp
 *
 *  Created on: Jan 25, 2015
 *      Author: tbergmueller
 */

#include "AngleField.h"

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>

#define PROBE_WIDTH		25
#define PROBE_HEIGHT	2*PROBE_WIDTH

#define MAX_SEARCH_X	PROBE_WIDTH
#define MAX_SEARCH_Y	PROBE_WIDTH/2	// Maximum 45°


using namespace cv;
using namespace std;


float getAngleAtPosition(const Mat& ultraSound, const Point& probePoint)
{


		Mat probe = ultraSound(Rect(probePoint.x, probePoint.y, PROBE_WIDTH, PROBE_HEIGHT));


		Mat differenceAccu(MAX_SEARCH_Y, MAX_SEARCH_X - PROBE_WIDTH/2, CV_32FC1);

		Point startPoint = Point(probePoint.x+PROBE_WIDTH/2, probePoint.y); // we assume they go downwards...

		for(int dx = startPoint.x; dx < startPoint.x+MAX_SEARCH_X - PROBE_WIDTH/2; dx++)
		{
			for(int dy = startPoint.y; dy < startPoint.y+MAX_SEARCH_Y; dy++)
			{

				Rect compareROI(dx,dy, PROBE_WIDTH,PROBE_HEIGHT);

				Mat toCompare = ultraSound(compareROI);

				/*imshow("toCompare", toCompare);
				waitKey();
*/

				Mat diff;
				absdiff(toCompare, probe, diff);

				float differ = sum(diff)[0] ;

				Point akkuPoint(dx-startPoint.x, dy-startPoint.y);

			//	cout << "differ @" << akkuPoint << " => "<< differ << endl;


				differenceAccu.at<float>(akkuPoint) = differ;
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




AngleField::AngleField(int gridSize, bool verbose)
{
	_gridSize = gridSize;
	_verbose = verbose;
}

AngleField::~AngleField() {
	// TODO Auto-generated destructor stub
}

void AngleField::compute(const cv::Mat& ultraSound, cv::Rect& roi)
{
	_roi = roi;
		// ok, latest point

	// FIXME: Somewhat inaccurate calculation of what's possible to compute Why the minuses...
		int nrAnglesX = (roi.width - PROBE_WIDTH -MAX_SEARCH_X) / _gridSize ;
		int nrAnglesY = (roi.height - PROBE_HEIGHT -MAX_SEARCH_Y) / _gridSize;

		_angleField = Mat::zeros(nrAnglesY, nrAnglesX, CV_32FC1);

		float nrOfComputations = _angleField.cols * _angleField.rows;

		int nrCompleted = 0;

		for(int x=0; x<_angleField.cols; x++)
		{
			for(int y=0; y<_angleField.rows; y++)
			{
				Point probePoint(x*_gridSize + roi.x, y*_gridSize + roi.y);


				float angle = getAngleAtPosition(ultraSound, probePoint);

				// cout <<

				_angleField.at<float>(y,x) = angle;
				nrCompleted++;

				if(_verbose)
				{
					cout << "Compute at " << probePoint <<  ": "<< angle << "\tProgress: " << (float)(nrCompleted) / nrOfComputations * 100.0 << "%" << endl;
				}

			}
		}



}

void AngleField::illustrate(cv::Mat& rgbUltraSound) const
{



			for(int x=0; x<_angleField.cols; x++)
			{
				for(int y=0; y<_angleField.rows; y++)
				{
					Point probePoint(x*_gridSize + _roi.x, y*_gridSize + _roi.y);

					circle(rgbUltraSound,probePoint,1,CV_RGB(0,0,255), CV_FILLED, CV_AA);

					Point endPoint((x+1)*_gridSize, probePoint.y + _gridSize*tan(-_angleField.at<float>(y,x)));

					line(rgbUltraSound, probePoint,endPoint, CV_RGB(0,0,255), 1, CV_AA);

				}
			}


}

float AngleField::getMeanAngle() const
{
	assert(!_angleField.empty());
	return mean(_angleField)[0];

}

void AngleField::getAngleHistogram(cv::Mat& output) const
{
	assert(!_angleField.empty());

	 // Quantize the hue to 30 levels
		    // and the saturation to 32 levels
		    int hbins = 360;
		    int histSize[] = {hbins};

		    float hranges[] = { 0, CV_PI * 2 };

		    const float* ranges[] = { hranges };
		    MatND hist;
		    // we compute the histogram from the 0-th and 1-st channels
		    int channels[] = {0, 1};

		    calcHist( &_angleField, 1, channels, Mat(), // do not use mask
		             hist, 1, histSize, ranges,
		             true, // the histogram is uniform
		             false );

		    int scale = 200;
		    output = Mat::zeros(scale, hbins, CV_8UC3);


		    float maxVal = 0;

		    for( int h = 0; h < hbins; h++ )
		    {
		    	float binVal = hist.at<float>(h,0);

		    	if(binVal > maxVal)
		    	{
		    		maxVal = binVal;
		    	}
		    }



		    for( int h = 0; h < hbins; h++ )
		    {
		    	float binVal = hist.at<float>(h,0);

		    //	cout << "binVal is " << binVal << endl;

		            rectangle( output, Point(h, 0),
		                        Point( (h+1), scale*(1- (binVal / (maxVal)))),
		                        Scalar::all(255),
		                        CV_FILLED );
		    }

		    bitwise_not(output,output);


}
