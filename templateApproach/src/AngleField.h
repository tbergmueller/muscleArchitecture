/*
 * AngleField.h
 *
 *  Created on: Jan 25, 2015
 *      Author: tbergmueller
 */

#ifndef ANGLEFIELD_H_
#define ANGLEFIELD_H_

#include <opencv2/core/core.hpp>

class AngleField {
public:
	AngleField(int gridSize, bool verbose=false);
	virtual ~AngleField();

	void compute(const cv::Mat& ultraSound, cv::Rect& roi);
	void illustrate(cv::Mat& rgbUltraSound);
	float getMeanAngle() const;

	void getAngleHistogram(cv::Mat& output) const;

private:
	int _gridSize;
	cv::Mat _angleField;
	bool _verbose;
};

#endif /* ANGLEFIELD_H_ */
