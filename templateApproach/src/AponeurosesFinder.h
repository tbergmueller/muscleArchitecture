/*
 * AponeurosesFinder.h
 *
 *  Created on: Jan 21, 2015
 *      Author: tbergmueller
 */

#ifndef APONEUROSESFINDER_H_
#define APONEUROSESFINDER_H_

#include <opencv2/core/core.hpp>
#include "AponeuroseCandidate.h"
#include <vector>

class AponeurosesFinder {
public:
	AponeurosesFinder();
	virtual ~AponeurosesFinder();

	void findAponeuroses(const cv::Mat& ultrasonic);

	bool getUpperApoAngle(float* angle) const;
	bool getLowerApoAngle(float* angle) const;

	void drawCandidates(cv::Mat& colorMat);

	void drawWithFasicleAngle(cv::Mat& colorMat, float fasicleAngle) const;

	/**
	 * It is assumed the fasicles are over the whole width of the image
	 *
	 * With a certain spacing to the lower and upper Aponeuroses, the region is declared as ROI
	 *
	 * @param ultrasonic
	 * @return
	 */
	cv::Rect getFasicleRegion(const cv::Mat& ultrasonic) const;

private:
	/**
	 * Selects the likeliest two boundary aponeuroses.
	 *
	 * It uses the assumption that the correct ones are the two closest (above and below) the middle of the ultrasonic
	 * image. Furthermore, since it is highly unlikely that the aponeuroses is steeper than 20°, steeper lines are interpreted as
	 * missclassified candidates.
	 *
	 * @param ultrasonic Ultrasonic images (just important for Size)
	 */
	void selectLikeliest(const cv::Mat& ultrasonic);


private:
	std::vector<AponeuroseCandidate> _candidates;

	std::vector<AponeuroseCandidate>::const_iterator _upperApo;
	std::vector<AponeuroseCandidate>::const_iterator _lowerApo;
};

#endif /* APONEUROSESFINDER_H_ */
