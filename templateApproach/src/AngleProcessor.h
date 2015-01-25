/*
 * AngleProcessor.h
 *
 *  Created on: Jan 25, 2015
 *      Author: tbergmueller
 */

#ifndef ANGLEPROCESSOR_H_
#define ANGLEPROCESSOR_H_

#include "AngleField.h"
#include "AponeurosesFinder.h"

class AngleProcessor {
public:
	AngleProcessor(const AponeurosesFinder& apoFinder, const AngleField& angleField);
	virtual ~AngleProcessor();
	void showResults(const cv::Mat& ultraSound);

private:
	const  AponeurosesFinder& _apoFinder;
	const AngleField& _angleField;
};

#endif /* ANGLEPROCESSOR_H_ */
