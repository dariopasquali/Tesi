
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/opencv.hpp"
#include <stdlib.h>
#include <stdio.h>

using namespace cv;


int threshold_value = 0;
int threshold_type = 3;

int const max_value = 255;
int const max_type = 4;
int const max_BINARY_value = 255;

Mat src, src_gray, dst;

const char* window_name = "Threshold Demo";
const char* trackbar_type = "Type: \n 0: Binary \n 1: Binary Inverted \n 2: Truncate \n 3: To Zero \n 4: To Zero Inverted";
const char* trackbar_value = "Value";

void Threshold_Demo(int, void*);

int main(int, char** argv)
{
	VideoCapture cap(0);
	if (!cap.isOpened())
		return -1;

	namedWindow(window_name, WINDOW_AUTOSIZE);

	createTrackbar(trackbar_type,
		window_name, &threshold_type,
		max_type, Threshold_Demo);

	createTrackbar(trackbar_value,
		window_name, &threshold_value,
		max_value, Threshold_Demo);

	for (;;)
	{
		cap >> src; // get a new frame from camera
		cvtColor(src, src_gray, COLOR_RGB2GRAY);

		Threshold_Demo(0, 0);
		
		if (waitKey(30) >= 0)
			break;
	}
	
	/*
	src = imread("foto.jpg", 1);
	cvtColor(src, src_gray, COLOR_RGB2GRAY);

	namedWindow(window_name, WINDOW_AUTOSIZE);

	createTrackbar(trackbar_type,
					window_name, &threshold_type,
					max_type, Threshold_Demo);

	createTrackbar(trackbar_value,
					window_name, &threshold_value,
					max_value, Threshold_Demo);

	Threshold_Demo(0, 0);
	*/

	//waitKey(0);
	
	return 0;
}

void Threshold_Demo(int, void*)
{
	/* 0: Binary
	1: Binary Inverted
	2: Threshold Truncated
	3: Threshold to Zero
	4: Threshold to Zero Inverted
	*/
	threshold(src_gray, dst, threshold_value, max_BINARY_value, threshold_type);
	imshow(window_name, dst);
}