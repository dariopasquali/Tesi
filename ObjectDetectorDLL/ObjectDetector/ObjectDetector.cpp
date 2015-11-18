#include "ObjectDetector.h"
#include "Utility.h"
#include "commonInclude.h"


using namespace std;
using namespace cv;

namespace od{

	ObjectDetector::ObjectDetector() :
		_deleteFocus(0.79),
		_attenuationFocus(1)
	{}


	bool ObjectDetector::loadImage(cv::Mat& baseImage)
	{
		if (baseImage.empty())
		{
			cerr << "The base image is empty";
			return false;
		}

		bool found = findBaseShape(baseImage);

		//if (found)
		//{
		//	if (baseImage.size().height > 600 || baseImage.size().width > 600)
		//	{
		//		Size s = baseImage.size(), small;
		//		small.height = s.height / 5;
		//		small.width = s.width / 5;

		//		resize(baseImage, baseImage, small);
		//	}
		//	namedWindow("FIND THIS", 1);
		//	imshow("FIND THIS", baseImage);
		//}

		return found;
	}


	cv::Mat ObjectDetector::findObjectsInImage(cv::Mat& image,
		double hammingThreshold,
		double correlationThreshold,
		OutputMaskMode maskMode,
		std::vector<std::vector<std::vector<cv::Point>>>* detectedContours,
		int* numberOfObject)
	{
		vector<vector<vector<Point>>> approxContours = findApproxContours(image, true, false); //prima 60

		vector<vector<vector<Point>>> detectedObjects = processContours(approxContours, hammingThreshold, correlationThreshold, numberOfObject);

		if (maskMode == OutputMaskMode::NO_MASK)
			return image;

		cvtColor(image, image, CV_BGR2BGRA);
		Mat mask = generateDetectionMask(detectedObjects, image, maskMode);

		*detectedContours = detectedObjects;

		Mat alpha(image.size(), CV_8UC4);
		bitwise_and(image, mask, alpha);

		return alpha;
	}

	cv::Mat ObjectDetector::generateDetectionMask(
		std::vector<std::vector<std::vector<cv::Point>>> detectedObjects,
		cv::Mat& image,
		OutputMaskMode maskMode)
	{

		Scalar base, pen;

		if (image.type() == CV_8UC1)
		{
			base = Scalar(0);
			pen = Scalar(255);
		}
		else
		{
			base = Scalar(0, 0, 0, 0);
			pen = Scalar(255, 255, 255);
		}

		Mat mask(image.size(), image.type());
		mask = base;

		for (int i = 0; i < detectedObjects.size(); i++)
		{
			/*
			if (maskMode == OutputMaskMode::CONVEX_HULL)
			{

			vector<vector<Point>> preciseContours;

			for (int j = 0; j < detectedObjects[i].size(); j++)
			{
			if (detectedObjects[i].size() == 2)
			cout << "";

			Rect objectRect = boundingRect(detectedObjects[i][j]);
			objectRect.width += 15;
			objectRect.height += 15;

			if (objectRect.x >10)
			objectRect.x -= 10;
			else
			objectRect.x = 0;

			if (objectRect.y >10)
			objectRect.y -= 10;
			else
			objectRect.y = 0;


			Mat rect = image(objectRect);

			Point centre(rect.size().width / 2, rect.size().height / 2);
			int delH = rect.size().height * 0.99;
			int delW = rect.size().width * 0.99;
			int delX = centre.x - delW / 2;
			int delY = centre.y - delH / 2;
			Rect del(delX, delY, delW, delH);


			Mat gray(rect.size(), rect.type());
			cvtColor(rect, gray, CV_BGRA2GRAY);

			Size s = rect.size();
			s.width += 1;
			s.height += 1;
			Mat thresh(s, CV_8UC1);
			thresh = Scalar(255);

			int minThreshold = mean(gray)[0];

			if (minThreshold < 90)
			minThreshold = 60;
			else if (minThreshold >= 90 && minThreshold < 150)
			minThreshold = 100;

			threshold(gray, thresh, minThreshold, 255, THRESH_BINARY);

			//imshow("Thresh", thresh);

			vector<vector<Point>> contours;
			findContours(thresh, contours, CV_RETR_LIST, CHAIN_APPROX_NONE);

			#ifdef DEBUG_MODE
			gray = cv::Scalar(0);
			drawContours(gray, contours, -1, cv::Scalar(255), 1, CV_AA);
			#endif

			vector<Point> maxC;
			int size = 0;

			for (int k = 0; k < contours.size(); k++)
			{

			#ifdef DEBUG_MODE
			gray = cv::Scalar(0);
			vector<vector<Point>> temp;
			temp.push_back(contours[k]);
			drawContours(gray, temp, -1, cv::Scalar(255), 1, CV_AA);
			#endif
			if (contours[k].size() > size)
			{
			vector<Point> approx;
			approxPolyDP(contours[k], approx, contours[k].size() * 0.03, true);
			if (approx.size() == 4)
			continue;

			Rect bounding = boundingRect(contours[k]);

			#ifdef DEBUG_MODE
			rectangle(gray, del, Scalar(255));
			rectangle(gray, bounding, Scalar(255));
			#endif

			bool isInternal = bounding.x > del.x &&
			bounding.y > del.y &&
			bounding.x + bounding.width < del.x + del.width &&
			bounding.y + bounding.height < del.y + del.height;


			if (!isInternal)
			continue;

			size = contours[k].size();
			maxC = contours[k];
			}
			}

			for (int k = 0; k < maxC.size(); k++)
			{
			maxC[k].x += objectRect.x;
			maxC[k].y += objectRect.y;
			}

			preciseContours.push_back(maxC);
			}


			drawContours(mask, preciseContours, -1, pen, -1, CV_AA);



			}*/
			if (maskMode == OutputMaskMode::PRECISE_CONTOURS)
			{
				drawContours(mask, detectedObjects[i], -1, pen, -1, CV_AA);
			}
		}

		return mask;
	}
}

