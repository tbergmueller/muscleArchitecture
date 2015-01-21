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



#define SLOPE_GROUPING_TOLERANCE	5.0	// Tolerance when grouping slopes together in Degrees


#define MAX_APONEUROSES_ANGLE		10.0


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



	cvtColor( dst, color_dst, CV_GRAY2BGR );




	    vector<Vec4i> lines;
	    HoughLinesP( dst, lines, 1, CV_PI/180, 300, src.cols/2, 5 ); // FIXME: PARAMETRIZE


	    // Draw all found lines
	    for( size_t i = 0; i < lines.size(); i++ )
	    {
	        line( color_dst, Point(lines[i][0], lines[i][1]),
	            Point(lines[i][2], lines[i][3]), Scalar(0,0,255), 1, CV_AA);
	    }


	    groupAponeurosesCandidates(src,lines,candidates);



	    imshow("ApoCandidates", color_dst);



	//    namedWindow( "Source", 1 );
	//    imshow( "Source", src );


	    // Simple policy... select the line with the highest Y-Coordinate

}


void AponeurosesFinder::findAponeuroses(const cv::Mat& ultrasonic)
{

	detectAponeuroses(ultrasonic, _candidates);
	selectLikeliest(ultrasonic);

	cout << _candidates.size() << endl;
}


float AponeurosesFinder::getAngle()
{
	assert(_upperApo != _candidates.end());
	return _upperApo->getAngle();
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
}

/*
void AponeurosesFinder::selectLikeliest(const Mat& uSoundImg)
{
	_upperApo = _candidates.end();
	_lowerApo = _candidates.end();


	int middle = uSoundImg.rows / 2;

	float middleX = uSoundImg.cols / 2;

	for(vector<AponeuroseCandidate>::const_iterator it = _candidates.begin(); it != _candidates.end(); it++)
	{
		int yCandidate = it->getY(middleX);
		int diff = yCandidate - middle;


		if(diff < 0) // upper APO
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
		else
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
*/






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

