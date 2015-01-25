/*
 * AponeurosesFinder.cpp
 *
 *  Created on: Jan 21, 2015
 *      Author: tbergmueller
 */

#include "AponeurosesFinder.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <vector>
#include <iostream>

using namespace std;
using namespace cv;

//#define MIN(X,Y) ((X) < (Y) ? : (X) : (Y))
//#define MAX(X,Y) ((X) > (Y) ? : (X) : (Y))


#define SLOPE_GROUPING_TOLERANCE	5.0	// Tolerance when grouping slopes together in Degrees


#define MAX_APONEUROSES_ANGLE		10.0

#define FASICLE_BORDER_PERCENTAGE	0.1


/*********************************************
 * ANGLE GROUPING
 */
float computeAngle(Vec4i line)
{
	float deltaX = line[2] - line[0];
	float deltaY = line[3] - line[1];

	return atan2(deltaY, deltaX);
}


float computeD(Vec4i line)
{
	float deltaX = line[2] - line[0];
	float deltaY = line[3] - line[1];


	float k = (float)deltaY / (float)deltaX;
	float d = (float)line[1] - k*line[0];

	d += line[3] - k*line[2];

	return d/2.0;
}

bool findAngleIdxWithTolerance(float angle, float angleToleranceRadiants, const map<float, vector<Vec4i> >& groups, float* groupAngle)
{
	//cout << "Angle is " << angle;

	for(map<float, vector<Vec4i> >::const_iterator it = groups.begin(); it != groups.end(); it++)
	{

		if(fabs(angle - it->first) < angleToleranceRadiants)
		{
			*groupAngle = it->first;

		//	cout << "\t add to " << *groupAngle << endl;
			return true;
		}

	}

	//cout << "\t make new group" << endl;

	return false; // not found
}


bool findDWithTolerance(float d, float dTolerance, const map<float, vector<Vec4i> >& groups, float* groupD)
{
	//cout << "Angle is " << angle;

	for(map<float, vector<Vec4i> >::const_iterator it = groups.begin(); it != groups.end(); it++)
	{

		if(fabs(d - it->first) < dTolerance)
		{
			*groupD = it->first;

		//	cout << "\t add to " << *groupAngle << endl;
			return true;
		}

	}

	//cout << "\t make new group" << endl;

	return false; // not found
}




void recomputeGroupAngles( map<float, vector<Vec4i> >& groups)
{
	map<float, vector<Vec4i> > newMap;


	for(map<float, vector<Vec4i> >::iterator it = groups.begin(); it != groups.end(); it++)
	{
		vector<Vec4i> lines = it->second;

		float gAngle = 0.0;

		for(int i=0; i< (int)lines.size(); i++)
		{
			gAngle +=  computeAngle(lines[i]);
		}

		gAngle /= (float)lines.size();

		newMap[gAngle] = it->second;
	}

	groups = newMap;
}



void groupByDistance( map<float, vector<Vec4i> >& angleGrouped, vector<AponeuroseCandidate>& apoCandidates)
{
	float dTolerance = 5.0;

	for(map<float, vector<Vec4i> >::const_iterator it = angleGrouped.begin(); it != angleGrouped.end(); it++ )
	{
		// Now, group that thing by D

		vector<Vec4i> linesWithSameK = it->second;


		map<float, vector<Vec4i> > groupedByD;

		for(vector<Vec4i>::const_iterator line = linesWithSameK.begin(); line != linesWithSameK.end(); line++ )
		{
			float d = computeD(*line);

			float dToAddTo;

			// Look, if the angle is already in the tolerance band
			if(findDWithTolerance(d,dTolerance, groupedByD, &dToAddTo))
			{
				groupedByD[dToAddTo].push_back(*line);
			}
			else
			{
				groupedByD[d].push_back(*line);
			}
		}

		cout << "Grouped by D:" << groupedByD.size() << endl;

		for(map<float, vector<Vec4i> >::const_iterator dGrouped = groupedByD.begin(); dGrouped != groupedByD.end(); dGrouped++)
		{
			float meanD = 0.0;

			for(int i=0; i< (int) dGrouped->second.size(); i++)
			{
				meanD += computeD(dGrouped->second[i]);
			}

			meanD /= dGrouped->second.size();
			apoCandidates.push_back(AponeuroseCandidate(it->first, meanD));
		}

	}
}


void groupAponeurosesCandidates(const Mat& img, const vector<Vec4i>& candidates, vector<AponeuroseCandidate>& apoCandidates)
{


	 cout << "detected " << candidates.size() << " aponeuroses candidates" << endl;

	 // ######################################################### First group by slope (k)

	 map<float, vector<Vec4i> > groupedBySlope;

	 float tolRadiants = SLOPE_GROUPING_TOLERANCE * CV_PI / 180.0 / 2.0;

	for( size_t i = 0; i < candidates.size(); i++ )
	{


		float angle= computeAngle(candidates[i]);
		float angleToAddTo;

		// Look, if the angle is already in the tolerance band
		if(findAngleIdxWithTolerance(angle,tolRadiants, groupedBySlope, &angleToAddTo))
		{
			groupedBySlope[angleToAddTo].push_back(candidates[i]);
		}
		else
		{
			groupedBySlope[angle].push_back(candidates[i]);
		}


		recomputeGroupAngles(groupedBySlope);
	}



	// ####################### NOW, group it by distance


	cout << "There are " << groupedBySlope.size() << " groups left" << endl;

	groupByDistance(groupedBySlope,apoCandidates);




}


void detectAponeuroses(const Mat& src, vector<AponeuroseCandidate>& candidates)
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





	    vector<Vec4i> lines;
	    HoughLinesP( dst, lines, 1, CV_PI/180, 300, src.cols/2, 5 ); // FIXME: PARAMETRIZE



	    groupAponeurosesCandidates(src,lines,candidates);




	    // Draw all found lines
	    /*
	    cvtColor( dst, color_dst, CV_GRAY2BGR );
	    for( size_t i = 0; i < lines.size(); i++ )
	    {
	        line( color_dst, Point(lines[i][0], lines[i][1]),
	            Point(lines[i][2], lines[i][3]), Scalar(0,0,255), 1, CV_AA);
	    }
	    imshow("ApoLineFinder", color_dst);
*/



}


void AponeurosesFinder::findAponeuroses(const cv::Mat& ultrasonic)
{

	detectAponeuroses(ultrasonic, _candidates);
	selectLikeliest(ultrasonic);

	cout << _candidates.size() << endl;
}


bool AponeurosesFinder::getUpperApoAngle(float* angle) const
{
	if(_upperApo != _candidates.end())
	{
		*angle = _upperApo->getAngle();
		return true;
	}
	return false;
}

bool AponeurosesFinder::getLowerApoAngle(float* angle) const
{
	if(_lowerApo != _candidates.end())
		{
			*angle = _lowerApo->getAngle();
			return true;
		}
		return false;
}


void AponeurosesFinder::drawWithFasicleAngle(cv::Mat& colorMat, float fasicleAngle) const
{

	bool fasicleDrawn = false;

	if(_lowerApo != _candidates.end())
		{
			_lowerApo->draw(colorMat, CV_RGB(255,0,0));

			float x = (float)colorMat.cols-10;
			float y = _lowerApo->getY(x);

			float d = y - tan(fasicleAngle)*x;

			Point start(0, d);
			Point stop(colorMat.cols, colorMat.cols * tan(fasicleAngle) + d);

			line(colorMat,start,stop,CV_RGB(255,0,0), 1, CV_AA);
			fasicleDrawn = true;

		}

	if(_upperApo != _candidates.end())
	{
		_upperApo->draw(colorMat, CV_RGB(0,255,0));


			float x = 10;
			float y = _upperApo->getY(x);

			float d = y - tan(fasicleAngle)*x;

			Point start(0, d);
			Point stop(colorMat.cols, colorMat.cols * tan(fasicleAngle) + d);

			line(colorMat,start,stop,CV_RGB(0,255,0), 1, CV_AA);

	}


}


void AponeurosesFinder::drawCandidates(cv::Mat& colorMat)
{
	for(vector<AponeuroseCandidate>::const_iterator it = _candidates.begin(); it != _candidates.end(); it++)
	{
		it->draw(colorMat);
	}


	if(_upperApo != _candidates.end())
	{
		_upperApo->draw(colorMat, CV_RGB(0,255,0));
	}

	if(_lowerApo != _candidates.end())
	{
		_lowerApo->draw(colorMat, CV_RGB(255,0,0));
	}

	// Draw fasicleRegion

	rectangle(colorMat, getFasicleRegion(colorMat),CV_RGB(255,0,0), 1, CV_AA);

}


cv::Rect AponeurosesFinder::getFasicleRegion(const cv::Mat& ultrasonic) const
{
	assert(_upperApo != _candidates.end());

	// Define startY
	int startY = MAX(_upperApo->getY(0), _upperApo->getY(ultrasonic.cols));




	int stopY;

	if(_lowerApo == _candidates.end())
	{
		stopY = ultrasonic.rows;
	}
	else
	{
		stopY = MIN(_lowerApo->getY(0), _lowerApo->getY(ultrasonic.cols));
	}
	cout << "startY is " << startY << " and stopY is " << stopY << endl;


	int width = ultrasonic.cols * (1-2*FASICLE_BORDER_PERCENTAGE);
	int height = (stopY - startY)*(1-2*FASICLE_BORDER_PERCENTAGE);

	Rect fasicleRegion( ultrasonic.cols * FASICLE_BORDER_PERCENTAGE, (FASICLE_BORDER_PERCENTAGE)*height + startY, width, height);

	return fasicleRegion;

}


void AponeurosesFinder::selectLikeliest(const Mat& uSoundImg)
{
	_upperApo = _candidates.end();
	_lowerApo = _candidates.end();


	int upperThird = uSoundImg.rows * 1/3;
	int lowerThird = uSoundImg.rows * 1/2;


	float middleX = uSoundImg.cols / 2;

	float minAngle = -MAX_APONEUROSES_ANGLE* CV_PI / 180.0;
	float maxAngle = MAX_APONEUROSES_ANGLE* CV_PI / 180.0;


	for(vector<AponeuroseCandidate>::const_iterator it = _candidates.begin(); it != _candidates.end(); it++)
	{

		// check steepness

		if(it->getAngle() < minAngle || it->getAngle() > maxAngle)
		{
			cout << "ignored for to steep angle" << endl;
			continue;
		}

		int yCandidate = it->getY(middleX);


		if(yCandidate < upperThird) // upper APO
		{
			if(_upperApo == _candidates.end())
			{
				_upperApo = it;
			}
			else
			{
				float yBefore = _upperApo->getY(middleX);

				if(yCandidate > yBefore)
				{
					_upperApo = it;
				}
			}
		}
		else if(yCandidate > lowerThird)
		{
			if(_lowerApo == _candidates.end())
			{
				_lowerApo = it;
			}
			else
			{
				float yBefore = _lowerApo->getY(middleX);

				if(yCandidate < yBefore)
				{
					_lowerApo = it;
				}
			}
		}
	}
}







AponeurosesFinder::AponeurosesFinder() {
	// TODO Auto-generated constructor stub

}

AponeurosesFinder::~AponeurosesFinder() {
	// TODO Auto-generated destructor stub
}

