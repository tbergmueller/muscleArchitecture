/*
 * AponeuroseCandidate.h
 *
 *  Created on: Jan 21, 2015
 *      Author: tbergmueller
 */

#ifndef APONEUROSECANDIDATE_H_
#define APONEUROSECANDIDATE_H_

#include <opencv2/core/core.hpp>


class AponeuroseCandidate {
public:
	AponeuroseCandidate(float angle, float d);
	virtual ~AponeuroseCandidate();
	void draw(cv::Mat& toDrawTo) const;
	void draw(cv::Mat& toDrawTo, cv::Scalar color) const;

	/**
	 * Returns the Y-Coordinate of the Aponeuroses for a given x-coordinate.
	 *
	 * Uses line equation <=k*x+d
	 * @param x value
	 * @return y-value
	 */
	float getY(float x) const;

	float getAngle() const;

private:
	float _angle;
	float _d;
};

#endif /* APONEUROSECANDIDATE_H_ */
