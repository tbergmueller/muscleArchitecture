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

void AngleProcessor::showResults(const cv::Mat& ultraSound, std::string storePath)
{



	// Allright, illustrate angleField


	Mat visual;
	cvtColor(ultraSound,visual,CV_GRAY2BGR);

	// From angleField
	float meanFasicleAngle = _angleField.getMeanAngle();

	_apoFinder.drawWithFasicleAngle(visual,meanFasicleAngle);
	_angleField.illustrate(visual);

	//imshow("Visual", visual);

	Mat angleHisto;
	_angleField.getAngleHistogram(angleHisto);

	Mat text = Mat::zeros(150, angleHisto.cols, CV_8UC3);

	float lApoAngle, uApoAngle;

	int fontFace = FONT_HERSHEY_PLAIN;
	double fontScale = 1.0;
	int thickness = 1;


	int lineHeight = 30;
	int xOffset = 10;

	cv::putText(text, "Angles to aponeuroses:" , Point(xOffset,lineHeight), fontFace, fontScale, CV_RGB(255,255,255), thickness, CV_AA);



	cout << "<results>";

	if(_apoFinder.getLowerApoAngle(&lApoAngle))
	{


	    stringstream ss;

	    float angleDegress = (-lApoAngle + meanFasicleAngle)*180/CV_PI;
	    ss << "Lower A.: " <<  angleDegress<< " deg";
	    cv::putText(text, ss.str(), Point(2*xOffset,2*lineHeight), fontFace, fontScale, CV_RGB(255,0,0), thickness, CV_AA);

	    cout << "lower:" << angleDegress << ";";

	}
	else
	{
		 cout << "lower:" << -666 << ";";
	}

	if(_apoFinder.getUpperApoAngle(&uApoAngle))
	{
		 stringstream ss;
		 float angleDegress = (uApoAngle + meanFasicleAngle)*180/CV_PI;
		ss << "Upper A.: " << angleDegress << " deg";
		cv::putText(text, ss.str(), Point(2*xOffset,3*lineHeight), fontFace, fontScale, CV_RGB(0,255,0), thickness, CV_AA);

		 cout << "upper:" << angleDegress << ";";
	}
	else
	{
		 cout << "upper:" << -666 << ";";
	}

	cout << "</results>" << endl;


    Mat resultImage = Mat::zeros(ultraSound.rows+20, 2*ultraSound.rows, CV_8UC3);

    Rect uSroi(10,10,visual.cols, visual.rows);
    Rect textROI(resultImage.cols - 10 - text.cols,resultImage.rows - 10 - text.rows,text.cols, text.rows);

    Rect histROI(resultImage.cols - 10 - angleHisto.cols,10 ,angleHisto.cols, angleHisto.rows);

    visual.copyTo(resultImage(uSroi));
    rectangle(resultImage, uSroi, Scalar::all(255), 2, CV_AA);

    text.copyTo(resultImage(textROI));
    rectangle(resultImage, textROI, Scalar::all(255), 2, CV_AA);

    angleHisto.copyTo(resultImage(histROI));
    rectangle(resultImage, histROI, Scalar::all(255), 2, CV_AA);

    imshow("Result", resultImage);

    if(storePath != "")
    {
    	if(!imwrite(storePath, resultImage))
    	{
    		cerr << "Could not write result to " << storePath << endl;
    	}
    }











}
