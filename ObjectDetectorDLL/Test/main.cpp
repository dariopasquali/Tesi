#include <time.h>
#include <windows.h>
#include <tchar.h> 
#include <stdio.h>
#include <strsafe.h>

#include "ObjectDetector.h"
#include "ObjectdetectorFactory.h"

using namespace cv;
using namespace std;
using namespace od;



int main(int, char)
{
	String filename;
	Mat baseImage = imread("mecha2.jpg");

	ObjectDetector* detector = ObjectDetectorFactory::getObjectDetector(ObjectDetectorFactory::DetectorType::MONO);
	if (!detector->loadImage(baseImage))
		exit(1);
	/*

	// TEST SINGLE IMAGE

	Mat image = imread("stelle.jpg");

	if (image.empty())
	exit(2);


	if (image.size().height > 800 || image.size().width > 800)
	{
	Size s = image.size(), ss;
	ss.height = s.height / 3;
	ss.width = s.width / 3;

	resize(image, image, ss);
	}


	vector<vector<vector<Point>>> objects;
	int numberOfObjects = 0;

	Mat mask = detector->findObjectsInImage(image, 50, 50, ObjectDetector::OutputMaskMode::PRECISE_CONTOURS, &objects, &numberOfObjects);

	imshow("FINAL RESULT",mask);
	waitKey(0);

	*/

	// TEST VIDEO CAPTURE



	Mat image;
	Mat fpsImg(Size(100, 50), CV_8UC1);

	time_t start, end;
	int counter = 0;

	VideoCapture cap(1);
	if (!cap.isOpened())
	{
		return 0;
	}



	counter = 0;
	time(&start);

	for (;;)
	{

		//waitKey(0);
		cap >> image;

		imshow("Source", image);


		vector<vector<vector<Point>>> objects;
		int numberOfObjects = 0;



		Mat result = detector->findObjectsInImage(image, 75, 50, ObjectDetector::OutputMaskMode::PRECISE_CONTOURS, &objects, &numberOfObjects);


		time(&end);

		counter++;
		double sec = difftime(end, start);
		double fps = counter / sec;

		fpsImg = Scalar(255);
		putText(fpsImg, to_string(fps), Point(10, 50), FONT_HERSHEY_SIMPLEX, 1, Scalar(0));
		imshow("FPS", fpsImg);

		imshow("FINAL RESULT", result);

		if (waitKey(30) > 0)
			break;
	}



	waitKey(0);
	return 0;

}