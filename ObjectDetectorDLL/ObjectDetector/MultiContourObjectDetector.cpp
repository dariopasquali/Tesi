#include "Utility.h"
#include "commonInclude.h"
#include "MultiContourObjectDetector.h"


using namespace std;
using namespace cv;

namespace od{
	MultiContourObjectDetector::MultiContourObjectDetector() :
		ObjectDetector()
	{}


	bool MultiContourObjectDetector::findBaseShape(cv::Mat& baseImage)
	{
		vector<vector<vector<Point>>> approxContours = findApproxContours(baseImage, true, true);

		if (approxContours.size() == 0)
		{
			cerr << "ERROR: No valid contours found in base image" << endl;
			return false;
		}

		_baseShape = approxContours[0];
		_aspectedContours = _baseShape.size();
		_minContourPoints = numeric_limits<int>::max();

		for (int i = 0; i < _baseShape.size(); i++)
		{
			vector<Point> tempKeypoints;

			Utility::findCentroidsKeypoints(_baseShape[i], tempKeypoints, Utility::CentroidDetectionMode::THREE_LOOP);
			_baseKeypoints.push_back(tempKeypoints);

			if (_baseShape[i].size() < _minContourPoints)
				_minContourPoints = _baseShape[i].size();
		}

#ifdef DEBUG_MODE
		Mat contoursImage(baseImage.size(), CV_8UC1);
		contoursImage = cv::Scalar(0);
		drawContours(contoursImage, _baseShape, -1, cv::Scalar(255), 1, CV_AA);
		imshow("TO FIND CONTOUR", contoursImage);
#endif

		return true;
	}

	std::vector<std::vector<std::vector<cv::Point>>> MultiContourObjectDetector::findApproxContours(
		cv::Mat image,
		bool performOpening,
		bool findBaseShape)
	{

		// CREATE ACTIVE ZONE 80% AND 50% ---------------------

		Point centre(image.size().width / 2, image.size().height / 2);

		int deleteHeight = image.size().height * _deleteFocus;
		int deleteWidth = image.size().width * _deleteFocus;
		int deleteX = centre.x - deleteWidth / 2;
		int deleteY = centre.y - deleteHeight / 2;

		int attenuationHeight = image.size().height * _attenuationFocus;
		int attenuationWidth = image.size().width * _attenuationFocus;
		int attenuationX = centre.x - attenuationWidth / 2;
		int attenuationY = centre.y - attenuationHeight / 2;

		Rect erase(deleteX, deleteY, deleteWidth, deleteHeight);
		_deleteRect = erase;

		Rect ease(attenuationX, attenuationY, attenuationWidth, attenuationHeight);
		_attenuationRect = ease;
		// ----------------------------------------

		bool imageTooBig = false;

		Mat newImage;

		if (image.size().height <= 400 || image.size().width <= 400)
		{
			Mat pickColor = image(Rect((image.size().width / 2) - 1, image.size().height - 2, 2, 2));
			Scalar color = mean(pickColor);

			int increment = 2;
			newImage = Mat(Size(image.size().width + increment, image.size().height + increment), image.type());
			newImage = color;

			Point nc(newImage.size().width / 2, newImage.size().height / 2);
			int incH = image.size().height;
			int incW = image.size().width;
			int incX = nc.x - incW / 2;
			int incY = nc.y - incH / 2;

			image.copyTo(newImage(Rect(incX, incY, incW, incH)));
		}
		else
		{
			imageTooBig = true;
			newImage = image;
		}

		Size imgSize = newImage.size();
		Mat gray(imgSize, CV_8UC1);
		Mat thresh(imgSize, CV_8UC1);

		if (newImage.channels() >= 3)
			cvtColor(newImage, gray, CV_BGR2GRAY);
		else
			newImage.copyTo(gray);

		int minThreshold;

		if (performOpening)
		{
			// PERFORM OPENING (Erosion --> Dilation)

			int erosion_size = 3;
			int dilation_size = 3;

			if (imageTooBig)
			{
				erosion_size = 5;
				dilation_size = 5;
			}

			Mat element = getStructuringElement(0, Size(2 * erosion_size, 2 * erosion_size), Point(erosion_size, erosion_size));
			erode(gray, gray, element);
			dilate(gray, gray, element);

			minThreshold = mean(gray)[0];

			if (minThreshold < 90)
				minThreshold = 60;
			else if (minThreshold >= 90 && minThreshold < 125)
				minThreshold = 100;
		}


		threshold(gray, thresh, minThreshold, 255, THRESH_BINARY);

#ifdef DEBUG_MODE
		imshow("Threshold", thresh);
#endif

		vector<vector<Point>> contours;
		vector<Vec4i> hierarchy;
		vector<Point> hull, approx;

		map<int, vector<vector<Point>>> hierachedContours;
		map<int, vector<vector<Point>>> approxHContours;

		findContours(thresh, contours, hierarchy, CV_RETR_TREE, CHAIN_APPROX_NONE);


#ifdef DEBUG_MODE
		Mat tempI(image.size(), CV_8UC1);
		tempI = Scalar(0);
		drawContours(tempI, contours, -1, cv::Scalar(255), 1, CV_AA);

		imshow("Contours", tempI);
#endif


		vector<vector<Point>> temp;

		// CATALOG BY HIERARCHY LOOP
		for (int i = 0; i < contours.size(); i++)
		{

#ifdef DEBUG_MODE
			tempI = Scalar(0);
			temp.clear();
			temp.push_back(contours[i]);
			drawContours(tempI, temp, -1, cv::Scalar(255), 1, CV_AA);
#endif
			int parent = hierarchy[i][3];
			if (parent == -1)
			{
				if (hierachedContours.count(i) == 0)
				{
					// me not found

					hierachedContours.insert(pair<int, vector<vector<Point>>>(i, vector<vector<Point>>()));
					hierachedContours[i].push_back(contours[i]);
				}
				else
				{
					// me found
					continue;
				}
			}
			else
			{
				if (hierachedContours.count(parent) == 0)
				{
					// dad not found
					hierachedContours.insert(pair<int, vector<vector<Point>>>(parent, vector<vector<Point>>()));
					hierachedContours[parent].push_back(contours[parent]);
				}
				hierachedContours[parent].push_back(contours[i]);
			}
		}


		int minPoint, maxPoint;
		minPoint = _minContourPoints - _minContourPoints / 2.1;
		maxPoint = _minContourPoints + _minContourPoints / 1.5;


		// APPROX LOOP

		for (map<int, vector<vector<Point>>>::iterator it = hierachedContours.begin(); it != hierachedContours.end(); it++)
		{

			if (it->second[0].size() < 400)
				continue;

#ifdef DEBUG_MODE
			tempI = Scalar(0);
			drawContours(tempI, it->second, -1, cv::Scalar(255), 1, CV_AA);
#endif

			if (it == hierachedContours.begin() && it->second.size() < _aspectedContours)
				continue;

			for (int k = 0; k < it->second.size(); k++)
			{
				if (it->second[k].size() < _minContourPoints)
				{
					if (k == 0) // padre
						break;
					else        // figlio
						continue;
				}

				convexHull(it->second[k], hull, false);

				double epsilon = it->second[k].size() * 0.003;
				approxPolyDP(it->second[k], approx, epsilon, true);

#ifdef DEBUG_MODE			
				tempI = Scalar(0);
				vector<vector<Point>> temp;
				temp.push_back(approx);
				drawContours(tempI, temp, -1, cv::Scalar(255), 1, CV_AA);
#endif

				// REMOVE TOO EXTERNAL SHAPES -------------

				if (imageTooBig)
				{
					Rect bounding = boundingRect(it->second[k]);

#ifdef DEBUG_MODE
					rectangle(tempI, _deleteRect, Scalar(255));
					rectangle(tempI, bounding, Scalar(255));
#endif

					bool isInternal = bounding.x > _deleteRect.x &&
						bounding.y > _deleteRect.y &&
						bounding.x + bounding.width < _deleteRect.x + _deleteRect.width &&
						bounding.y + bounding.height < _deleteRect.y + _deleteRect.height;


					if (!isInternal)
					{
						if (k == 0)
							break;
					}
				}

				// --------------------------------------------------

				if (!findBaseShape)
				{
					if (hull.size() < minPoint || hull.size() > maxPoint)
					{
						if (k == 0) // padre
							break;
						else        // figlio
							continue;
					}

				}


				if (k == 0)
				{
					approxHContours.insert(pair<int, vector<vector<Point>>>(it->first, vector<vector<Point>>()));
					approxHContours.at(it->first).push_back(approx);
				}
				else
				{
					approxHContours[it->first].push_back(approx);
				}
			}
		}

		int maxSize = 0,
			maxID = 0;

		vector<vector<vector<Point>>> lookupVector;
		for (map<int, vector<vector<Point>>>::iterator it = approxHContours.begin(); it != approxHContours.end(); it++)
		{
			if (it->second.size() <= 1)
				continue;

			if (findBaseShape)
			{
				int totSize = 0;
				for (int k = 0; k < it->second.size(); k++)
				{
					totSize += it->second[k].size();
				}

				if (totSize > maxSize)
				{
					maxSize = totSize;
					maxID = it->first;
				}
			}
			else
			{
				lookupVector.push_back(it->second);
			}

		}

		if (findBaseShape)
		{
			lookupVector.push_back(approxHContours.at(maxID));
		}

		return lookupVector;
	}

	std::vector<std::vector<std::vector<cv::Point>>> MultiContourObjectDetector::processContours(
		std::vector<std::vector<std::vector<cv::Point>>> approxContours,
		double hammingThreshold,
		double correlationThreshold,
		int* numberOfObject)
	{
		vector<vector<vector<Point>>> objects;
		double attenuation = 0;

		for (int i = 0; i < approxContours.size(); i++)
		{
			if (approxContours[i].size() != _baseShape.size())
				continue;
			attenuation = 0;

#ifdef DEBUG_MODE
			Mat tempI(Size(1000, 1000), CV_8UC1);
			tempI = Scalar(0);
			drawContours(tempI, approxContours[i], -1, cv::Scalar(255), 1, CV_AA);
#endif

			double totCorrelation = 0,
				totHamming = 0;

			Moments m = moments(approxContours[i][0], true);
			int cx = int(m.m10 / m.m00);
			int cy = int(m.m01 / m.m00);

			Point c(cx, cy);

			if (!(c.x >= _attenuationRect.x &&
				c.y >= _attenuationRect.y &&
				c.x <= (_attenuationRect.x + _attenuationRect.width) &&
				c.y <= (_attenuationRect.y + _attenuationRect.height)))
				attenuation = 5;

			// C and H with external contour
			vector<Point> externalKeypoints;
			Utility::findCentroidsKeypoints(approxContours[i][0], externalKeypoints, Utility::CentroidDetectionMode::THREE_LOOP);
			totCorrelation += (Utility::correlationWithBase(externalKeypoints, _baseKeypoints[0]) - attenuation);

			totHamming += (Utility::calculateContourPercentageCompatibility(approxContours[i][0], _baseShape[0]) - attenuation);

			// looking for the contour with the better cnetroids and shape match

			for (int j = 1; j < approxContours[i].size(); j++)
			{
				attenuation = 0;

				Moments m = moments(approxContours[i][j], true);
				int cx = int(m.m10 / m.m00);
				int cy = int(m.m01 / m.m00);

				Point c(cx, cy);

				if (!(c.x >= _attenuationRect.x &&
					c.y >= _attenuationRect.y &&
					c.x <= (_attenuationRect.x + _attenuationRect.width) &&
					c.y <= (_attenuationRect.y + _attenuationRect.height)))
					attenuation = 5;


				double maxCorrelation = std::numeric_limits<double>::min(),
					maxHamming = std::numeric_limits<double>::min();

				for (int k = 1; k < _baseShape.size(); k++)
				{
					vector<Point> internalKeypoints;
					Utility::findCentroidsKeypoints(approxContours[i][j], internalKeypoints, Utility::CentroidDetectionMode::THREE_LOOP);
					maxCorrelation = max(maxCorrelation, Utility::correlationWithBase(internalKeypoints, _baseKeypoints[k]));

					maxHamming = max(maxHamming, Utility::calculateContourPercentageCompatibility(approxContours[i][j], _baseShape[k]));
				}

				totCorrelation += (maxCorrelation - attenuation);
				totHamming += (maxHamming - attenuation);
			}

			totCorrelation /= approxContours[i].size();
			totHamming /= approxContours[i].size();

			cout << "Middle Correlation " << to_string(i) << " with base ---> " << totCorrelation << endl;
			cout << "Middle Hamming distance" << to_string(i) << " with base ---> " << totHamming << endl;

			if (totCorrelation >= correlationThreshold && totHamming >= hammingThreshold)
				objects.push_back(approxContours[i]);
		}

		*numberOfObject = objects.size();

		return objects;
	}

}


