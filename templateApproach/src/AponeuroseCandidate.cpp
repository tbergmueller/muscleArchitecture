/*
 * AponeuroseCandidate.cpp
 *
 *  Created on: Jan 21, 2015
 *      Author: tbergmueller
 */

#include "AponeuroseCandidate.h"
#include <iostream>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;

AponeuroseCandidate::AponeuroseCandidate(float angle, float d)
{
	// TODO Auto-generated constructor stub
	_angle = angle;
	_d = d;
}

AponeuroseCandidate::~AponeuroseCandidate() {
	// TODO Auto-generated destructor stub
}


float AponeuroseCandidate::getY(float x) const
{
	return tan(_angle)*x + _d;
}



void AponeuroseCandidate::draw(cv::Mat& toDrawTo, cv::Scalar color) const
{
	assert(toDrawTo.type() == CV_8UC3);

		Point p1;
		p1.x = 0;
		p1.y = tan(_angle)*p1.x + _d;

		Point p2;
		p2.x = toDrawTo.cols;
		p2.y = p2.x * tan(_angle) + _d;

		line(toDrawTo,p1,p2,color, 3, CV_AA);
}
void AponeuroseCandidate::draw(cv::Mat& toDrawTo) const
{
	draw(toDrawTo, CV_RGB(0,0,255));
}

float AponeuroseCandidate::getAngle() const
{
	return _angle;
}
