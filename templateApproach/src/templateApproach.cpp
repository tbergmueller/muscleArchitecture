//============================================================================
// Name        : templateApproach.cpp
// Author      : tbergmueller
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "AponeurosesFinder.h"
#include "AngleField.h"
#include "AngleProcessor.h"

using namespace cv;

using namespace std;







void showAngleHisto(const cv::Mat& angleField)
{

}

int main(int argc, char** argv)
{

	if(argc != 2)
	{
		cerr << "You have to call the program like 'program.exe <pathToImage>'" << endl;
		return -1;
	}
	string path =argv[1];

	Mat img = imread(path, CV_LOAD_IMAGE_GRAYSCALE);

	if(img.empty())
	{
		cerr << "Could not load image from " << path << endl;
		return -1;
	}

	Rect ultrasoundROI(258,130,694-258,558-130);

	Mat ultraSound = img(ultrasoundROI);


	// ########################################## Detect aponeuroses
	AponeurosesFinder apoFinder;
	apoFinder.findAponeuroses(ultraSound);


	/*Mat apoImg;
	cvtColor(ultraSound, apoImg, CV_GRAY2BGR);
	apoFinder.drawCandidates(apoImg);
	imshow("Possible Aponeuroses", apoImg);
*/




	AngleField af(20);
	Rect fasicleRegion = apoFinder.getFasicleRegion(ultraSound);
	cout << "Computing Fasicle angle.. may take some time..." << endl;
	af.compute(ultraSound,fasicleRegion);

	cout << "Done" << endl;




	AngleProcessor processor(apoFinder, af);

	processor.showResults(ultraSound);




	//imshow("ultrasound", ultraSound);
	waitKey();
	return 0;
}
