/*
 * AngleProcessor.cpp
 *
 *  Created on: Jan 25, 2015
 *      Author: tbergmueller
 */

#include "AngleProcessor.h"

#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;
using namespace std;

AngleProcessor::AngleProcessor(const AponeurosesFinder& apoFinder,
		const AngleField& angleField):
		_apoFinder(apoFinder),
		_angleField(angleField)
{

}

AngleProcessor::~AngleProcessor() {
	// TODO Auto-generated destructor stub
}

void AngleProcessor::showResults(const cv::Mat& ultraSound)
{

	// Allright, illustrate angleField


	Mat visual;
	cvtColor(ultraSound,visual,CV_GRAY2BGR);

	// From angleField
	float meanFasicleAngle = _angleField.getMeanAngle();

	_apoFinder.drawWithFasicleAngle(visual,meanFasicleAngle);

	imshow("Visual", visual);


	float lApoAngle, uApoAngle;

	if(_apoFinder.getLowerApoAngle(&lApoAngle))
	{
		cout <<  "Angle to lower Apo ("<< (lApoAngle*180/CV_PI)<<") is " << (-lApoAngle + meanFasicleAngle)*180/CV_PI << endl;

	}

	if(_apoFinder.getUpperApoAngle(&uApoAngle))
	{
		cout <<  "Angle to upper Apo is " << (-uApoAngle + meanFasicleAngle)*180/CV_PI << endl;
	}


	Mat angleHisto;
	_angleField.getAngleHistogram(angleHisto);
    imshow( "Angle Histogram", angleHisto );


}
