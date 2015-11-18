#include "ObjectDetectorFactory.h"
#include "CategoryTesterFactory.h"
#include <time.h>
#include <windows.h>
#include <tchar.h> 
#include <stdio.h>
#include <strsafe.h>


using namespace cv;
using namespace std;
using namespace od;

String loadRandomImage(char* dir)
{
	TCHAR szDir[MAX_PATH];
	WIN32_FIND_DATA ffd;
	HANDLE hFind = INVALID_HANDLE_VALUE;

	srand(time(NULL));
	int dirNum = rand() % 81;

	StringCchCopy(szDir, MAX_PATH, dir);
	StringCchCat(szDir, MAX_PATH, TEXT("\\*"));

	hFind = FindFirstFile(szDir, &ffd);
	
	int d = 0;
	while (d < dirNum && FindNextFile(hFind, &ffd) != 0)
		d++;

	FindNextFile(hFind, &ffd);
	if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
	{
		FindClose(hFind);
		HANDLE hFindSub = INVALID_HANDLE_VALUE;
		WIN32_FIND_DATA subffd;

		StringCchCopy(szDir, MAX_PATH, dir);
		StringCchCat(szDir, MAX_PATH, TEXT("\\"));
		StringCchCat(szDir, MAX_PATH, ffd.cFileName);
		StringCchCat(szDir, MAX_PATH, TEXT("\\*"));

		int fileNum = rand() % 15;

		hFindSub = FindFirstFile(szDir, &subffd);

		int f = 0;
		while (f < fileNum && FindNextFile(hFindSub, &subffd) != 0)
			f++;

		FindNextFile(hFindSub, &subffd);

		StringCchCopy(szDir, MAX_PATH, dir);
		StringCchCat(szDir, MAX_PATH, TEXT("\\"));
		StringCchCat(szDir, MAX_PATH, ffd.cFileName);
		StringCchCat(szDir, MAX_PATH, TEXT("\\"));
		StringCchCat(szDir, MAX_PATH, subffd.cFileName);

		return szDir;
	}
	else
		return NULL;


}


int main(int, char)
{


	

	
	String filename;
	Mat baseImage = imread("mecha2.jpg");
	
	ObjectDetector* detector = ObjectDetectorFactory::getObjectDetector(1);
	if(!detector->loadImage(baseImage))
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
		
		

		Mat result = detector->findObjectsInImage(image, 80, 75, ObjectDetector::OutputMaskMode::PRECISE_CONTOURS, &objects, &numberOfObjects);
		
		
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
	


	cout << "1 = INTRA_CATEGORY_TEST / 2 = INTER_CATEGORY_TEST" << endl;
	cout << "Mode: ";

	char mode[256];
	gets(mode);

	CategoryTesterFactory::TestMode tm;

	if (strcmp(mode, "1") == 0)
		tm = CategoryTesterFactory::TestMode::INTRA_CATEGORY_TEST_MODE;
	else
		tm = CategoryTesterFactory::TestMode::INTER_CATEGORY_TEST_MODE;


	// INTER-CATEGORY TEST

	CategoryTester* interTester = CategoryTesterFactory::getCategoryTester(tm, detector);

	vector<string> categories = interTester->loadCategories();

	char choosen[260];

	cout << "category to test?" << endl;
	for (int i = 0; i < categories.size(); i++)
	{
		cout << categories[i] << endl;
	}
	cout << endl << "category: ";

	gets(choosen);

	interTester->setCategory(choosen);

	double CDR = interTester->startTest();

	cout << endl << endl << "Correlation Detection Rate: " << to_string(CDR) << endl << endl;

	waitKey(0);
	return 0;

}