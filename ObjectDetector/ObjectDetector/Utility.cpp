#include "Utility.h"
#include "commonInclude.h"
#include <math.h>

using namespace cv;
using namespace std;
using namespace od;


std::vector<cv::Rect> Utility::splitRect(Rect box, Point centroid, int level, SplitMode mode)
{
	vector<Rect> splitRect;

	int x = box.x;
	int y = box.y;
	int h = box.height;
	int w = box.width;

	int cx = centroid.x;
	int cy = centroid.y;

	Rect r1, r2, r3, r4;

	if (mode == SplitMode::MODE_1)
	{
		if (level == 1)
		{
			r1 = Rect(x, y, cx - x, h);
			r2 = Rect(cx, y, w, h);
			r3 = Rect(x, y, w, cy - y);
			r4 = Rect(x, cy, w, h);
		}
		else if (level > 1)
		{
			r1 = Rect(x, y, cx - x, h);
			r2 = Rect(cx, y, (w + x) - cx, h);
			r3 = Rect(x, y, w, cy - y);
			r4 = Rect(x, cy, w, (h + y) - cy);
		}
	
		splitRect.push_back(r1);
		splitRect.push_back(r2);
		splitRect.push_back(r3);
		splitRect.push_back(r4);
	}
	else
	{
		if (level == 1)
		{
			r1 = Rect(x, y, cx - x, cy - y);
			r2 = Rect(cx, y, w, cy - y);
			r3 = Rect(x, cy, cx - x, h);
			r4 = Rect(cx, cy, w, h);
		}
		else if (level > 1)
		{
			r1 = Rect(x, y, cx - x, cy - y);
			r2 = Rect(cx, y, (w + x) - cx, cy - y);
			r3 = Rect(x, cy, cx - x, (h + y) - cy);
			r4 = Rect(cx, cy, (w + x) - cx, (h + y) - cy);
		}

		splitRect.push_back(r1);
		splitRect.push_back(r2);
		splitRect.push_back(r3);
		splitRect.push_back(r4);
	}
	return splitRect;
}


double Utility::calculateContourPercentageCompatibility(std::vector<cv::Point> contour, std::vector<cv::Point> base)
{

#ifdef DEBUG_MODE
	Mat tempImg(Size(640, 480), CV_8UC1);
	tempImg = cv::Scalar(0);

	vector<vector<Point>> vect;
	vect.push_back(contour);

	drawContours(tempImg, vect, -1, cv::Scalar(255), 1, CV_AA);
#endif

	double hamming3 = matchShapes(contour, base, CV_CONTOURS_MATCH_I3, 0.0);

	return ((1 - hamming3) * 100);
}

double Utility::correlationWithBase(std::vector<cv::Point> contourKeypoints, std::vector<cv::Point> baseKeypoints){


		
#ifdef DEBUG_MODE
	Mat tempImg(Size(1920, 1920), CV_8UC3);
	tempImg = Scalar(0);

	for (int i = 0; i < baseKeypoints.size(); i++)
		circle(tempImg, baseKeypoints[i], 6, Scalar(0, 0, 255), -1, 8, 0);

	for (int i = 0; i < contourKeypoints.size(); i++)
		circle(tempImg, contourKeypoints[i], 3, Scalar(0, 255, 0), -1, 8, 0);
#endif

		
	double correlation = 0;
			
	double centroidsCorrelation = cosineSimilarity(contourKeypoints, baseKeypoints);

	return centroidsCorrelation;
	
}


// COORELATION FUNCTIONS -------------------------------------------------------------------------------------------------


double Utility::cosineSimilarity(std::vector<cv::Point>& distribution, std::vector<cv::Point>& base)
{
	int validPoints = 0;

	int size = min(distribution.size(), base.size());
	
	
	Point2d product = 0;
	Point2d distr2 = 0;
	Point2d base2 = 0;

	for (int i = 0; i < size; i++)
	{
		//if (distribution[i] == Point(0, 0) || base[i] == Point(0, 0))
		//	continue;

		product.x += distribution[i].x * base[i].x;
		product.y += distribution[i].y * base[i].y;

		distr2.x += distribution[i].x * distribution[i].x;
		distr2.y += distribution[i].y * distribution[i].y;
		
		base2.x += base[i].x * base[i].x;
		base2.y += base[i].y * base[i].y;
	}

	Point2d correlation;
	correlation.x = product.x / (sqrt(base2.x) * sqrt(distr2.x));
	correlation.y = product.y / (sqrt(base2.y) * sqrt(distr2.y));
	

	/*
	Point2d diffDistr, diffBase;

	Point2d product = 0;
	Point2d distr2 = 0;
	Point2d base2 = 0;

	// FIND MEANS
	Point meanDistr(0, 0), meanBase(0, 0);
	for (int i = 0; i < size; i++)
	{
		//if (distribution[i] == Point(0, 0) || base[i] == Point(0, 0))
		//	continue;

		validPoints++;

		meanDistr.x += distribution[i].x;
		meanDistr.y += distribution[i].y;
		meanBase.x += base[i].x;
		meanBase.y += base[i].y;
	}

	meanDistr.x /= validPoints;
	meanDistr.y /= validPoints;

	meanBase.x /= validPoints;
	meanBase.y /= validPoints;



	for (int i = 0; i < size; i++)
	{
		if (distribution[i] == Point(0, 0) || base[i] == Point(0, 0))
			continue;

		diffDistr.x = distribution[i].x - meanDistr.x;
		diffDistr.y = distribution[i].y - meanDistr.y;

		diffBase.x = base[i].x - meanBase.x;
		diffBase.y = base[i].y - meanBase.y;

		product.x += diffDistr.x * diffBase.x;
		product.y += diffDistr.y * diffBase.y;

		distr2.x += diffDistr.x * diffDistr.x;
		distr2.y += diffDistr.y * diffDistr.y;

		base2.x += diffBase.x * diffBase.x;
		base2.y += diffBase.y * diffBase.y;
	}

	Point2d correlation;
	correlation.x = product.x / sqrt(distr2.x * base2.x);
	correlation.y = product.y / sqrt(distr2.y * base2.y);
	*/

	return ((correlation.x + correlation.y) / 2) * 100;
}


// FEATURE DETECTION FUNCTIONS ---------------------------------------------------------------------------------------------

void Utility::findCentroidsKeypoints(std::vector<cv::Point> contour,
													std::vector<cv::Point>& centroids,													
													CentroidDetectionMode mode)
{
	centroids.clear();
	Rect box = boundingRect(contour);

	box.height += 4;
	box.width += 4;
	
	if (box.x == 1)
		box.x -= 1;
	if (box.y == 1)
		box.y -= 1;	

	
	// center of the contour
	Moments m = moments(contour, true);
	int cx = int(m.m10 / m.m00);
	int cy = int(m.m01 / m.m00);
	Point2f center(cx, cy);

	
	Size size(box.x + box.width + cx, box.y + box.height + cy);
	Mat img(size, CV_8UC1);
	img = Scalar(0);
	vector<vector<Point>> tempVector;
	tempVector.push_back(contour);
	drawContours(img, tempVector, -1, cv::Scalar(255), 1, CV_AA);


	circle(img, center, 3, Scalar(255), -1, 8, 0);

	centroids.push_back(center);

	
	// SPLIT ======================= = LEVEL 1 = ===============================

	vector<Rect> split = splitRect(box, center, 1, SplitMode::MODE_2);

	for (int i = 0; i < split.size(); i++)
	{
		img = Scalar(0);
		drawContours(img, tempVector, -1, cv::Scalar(255), 1, CV_AA);
		Mat sub = img(split[i]);

		int dilation_size = 1;
		Mat element = getStructuringElement(0, Size(2 * dilation_size, 2 * dilation_size), Point(dilation_size, dilation_size));
		dilate(sub, sub, element);

		vector<vector<Point>> shape;
		findContours(sub, shape, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
		
		int cx_1, cy_1;
		
		if (shape.size() == 1)
		{
			// center of the contour
			Moments m = moments(shape[0], true);
			cx_1 = int(m.m10 / m.m00) + split[i].x;
			cy_1 = int(m.m01 / m.m00) + split[i].y;
		}
		else
		{
			// center of the sub image
			Moments m = moments(sub, true);
			cx_1 = int(m.m10 / m.m00) + split[i].x;
			cy_1 = int(m.m01 / m.m00) + split[i].y;
		}

		Point2f center(cx_1, cy_1);

		//Size size(box.x + box.width + cx, box.y + box.height + cy);
		//Mat img(size, CV_8UC1);
		//img = Scalar(0);
		//drawContours(img, tempVector, -1, cv::Scalar(255), 1, CV_AA);


		circle(img, center, 6, Scalar(255), -1, 8, 0);
		
		if(shape.size() != 0 && cx_1 > 0 && cy_1 > 0)
			centroids.push_back(center);
		else
			centroids.push_back(Point2f(0,0));


		if (cx_1 > 0 && cy_1 > 0)
		{
			if (mode != CentroidDetectionMode::ONE_LOOP)
			{
				// SPLIT ========================== = LEVEL 2 = ===============================

				vector<Rect> splitRect_1 = splitRect(split[i], center, 2, SplitMode::MODE_2);


				for (int j = 0; j < splitRect_1.size(); j++)
				{
					img = Scalar(0);
					drawContours(img, tempVector, -1, cv::Scalar(255), 1, CV_AA);
					Mat sub = img(splitRect_1[j]);

					int dilation_size = 1;
					Mat element = getStructuringElement(0, Size(2 * dilation_size, 2 * dilation_size), Point(dilation_size, dilation_size));
					dilate(sub, sub, element);

					vector<vector<Point>> shape_1;
					findContours(sub, shape_1, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);

					int cx_2, cy_2;

					if (shape_1.size() == 1)
					{
						// center of the contour
						Moments m = moments(shape_1[0], true);
						cx_2 = int(m.m10 / m.m00) + splitRect_1[j].x;
						cy_2 = int(m.m01 / m.m00) + splitRect_1[j].y;
					}
					else
					{
						// center of the sub image
						Moments m = moments(sub, true);
						cx_2 = int(m.m10 / m.m00) + splitRect_1[j].x;
						cy_2 = int(m.m01 / m.m00) + splitRect_1[j].y;

					}


					Point2f center(cx_2, cy_2);


					//Size size(box.x + box.width + cx, box.y + box.height + cy);
					//Mat img(size, CV_8UC1);
					//img = Scalar(0);
					//drawContours(img, tempVector, -1, cv::Scalar(255), 1, CV_AA);


					circle(img, center, 9, Scalar(255), -1, 8, 0);

					if (shape_1.size() != 0 && cx_2 > 0 && cy_2 > 0)
						centroids.push_back(center);
					else
						centroids.push_back(Point2f(0, 0));


					if (cx_2 > 0 && cy_2 > 0)
					{
						if (mode == CentroidDetectionMode::THREE_LOOP) // SPLIT ========================== = LEVEL 3 = ===============================
						{
							vector<Rect> splitRect_2 = splitRect(splitRect_1[j], center, 3, SplitMode::MODE_2);

							for (int k = 0; k < splitRect_2.size(); k++)
							{
								img = Scalar(0);
								drawContours(img, tempVector, -1, cv::Scalar(255), 1, CV_AA);
								Mat sub = img(splitRect_2[k]);

								int dilation_size = 1;
								Mat element = getStructuringElement(0, Size(2 * dilation_size, 2 * dilation_size), Point(dilation_size, dilation_size));
								dilate(sub, sub, element);

								vector<vector<Point>> shape_2;
								findContours(sub, shape_2, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
								int cx_3, cy_3;

								if (shape_2.size() == 1) // THIRD LEVEL LOOP
								{
									// center of the contour
									Moments m = moments(shape_2[0], true);
									cx_3 = int(m.m10 / m.m00) + splitRect_2[k].x;
									cy_3 = int(m.m01 / m.m00) + splitRect_2[k].y;

								}
								else
								{
									// center of the image
									Moments m = moments(sub, true);
									cx_3 = int(m.m10 / m.m00) + splitRect_2[k].x;
									cy_3 = int(m.m01 / m.m00) + splitRect_2[k].y;
								}

								Point2f center(cx_3, cy_3);


								//Size size(box.x + box.width + cx, box.y + box.height + cy);
								//Mat img(size, CV_8UC1);
								//img = Scalar(0);
								//drawContours(img, tempVector, -1, cv::Scalar(255), 1, CV_AA);


								circle(img, center, 9, Scalar(255), -1, 8, 0);

								if (shape_2.size() != 0 && cx_3 > 0 && cy_3 > 0)
									centroids.push_back(center);
								else
									centroids.push_back(Point2f(0, 0));


							}

						}//  ========================== = LEVEL 3 = ===============================	

					}
					else
					{
						for (int i = 0; i < 4; i++)
						{
							centroids.push_back(Point2f(0, 0));
						}
					}


				}

			}// ========================== = LEVEL 2 = ===============================	
		}
		else
		{
			int loop = (mode == TWO_LOOP) ? 4 : 16;
			
			for (int i = 0; i < loop; i++)
			{
				centroids.push_back(Point2f(0, 0));
			}
		}
	}
}



std::vector<double> Utility::findDistancesFromCenter(std::vector<cv::Point> distribution)
{
	Moments m = moments(distribution);
	Point center(m.m10 / m.m00, m.m01 / m.m00);

	vector<double> distances;
	
	for (int i = 0; i < distribution.size(); i++)
	{
		if (distribution[i] == Point(0, 0))
		{
			distances.push_back(0);
			continue;
		}

		double cat1 = (distribution[i].x - center.x);
		double cat2 = (distribution[i].y - center.y);
		double dist = sqrt(cat1*cat1 + cat2*cat2);
		distances.push_back(dist);
	}

	return distances;
}

std::vector<double> Utility::findAnglesRespectCenter(std::vector<cv::Point> distribution)
{
	Moments m = moments(distribution);
	Point center(m.m10 / m.m00, m.m01 / m.m00);

	vector<double> angles;

	for (int i = 0; i < distribution.size(); i++)
	{
		if (distribution[i] == Point(0, 0))
		{
			angles.push_back(numeric_limits<double>::max());
			continue;
		}

		double cat1 = (distribution[i].x - center.x);
		double cat2 = (distribution[i].y - center.y);
		double ip = sqrt(cat1*cat1 + cat2*cat2);
		double ang = asin(cat1 / ip);

		angles.push_back(ang);
	}

	return angles;
}

// --------------------------------------------------------------------------------------------------------------------------

/*void Utility::histCompare(std::vector<cv::Point> contour, std::vector<cv::Point> base)
{
	Rect boxB = boundingRect(base);
	Rect boxC = boundingRect(contour);

	vector<double> boundB, boundC;
	map<double, int> XC, YC, XB, YB;

	double stepC = ((double)boxC.width) / 256;
	double stepB = ((double)boxB.width) / 256;

	for (int i = 0; i < base.size(); i++)
	{
		for (double j = boxB.x; j < (boxB.x + boxB.width); j += stepB)
		{
			if (XB.count(j) == 0)
				XB.insert(pair<double, int>(j, 0));
			if (base[i].x >= j && base[i].x <= j + stepB)
			{				
				XB[j]++;
			}
		}		
	}

	for (int i = 0; i < contour.size(); i++)
	{
		for (double j = boxC.x; j < (boxC.x + boxC.width); j += stepC)
		{
			if (XB.count(j) == 0)
				XC.insert(pair<double, int>(j, 0));
			if (contour[i].x >= j && contour[i].x <= j + stepC)
			{				
				XC[j]++;
			}
		}
	}

	int pixel = 0;
	Mat histC(Size(1, 256), CV_8UC1);
	for (map<double, int>::iterator it = XC.begin(); it != XC.end(); it++)
	{
		histC.at<Vec3b>(pixel,0)[0] = it->second*10;
		pixel++;
	}

	pixel = 0;
	Mat histB(Size(1, 256), CV_8UC1);
	for (map<double, int>::iterator it = XB.begin(); it != XB.end(); it++)
	{
		histB.at<Vec3b>(pixel, 0)[0] = it->second*10;
		pixel++;
	}

	normalize(histC, histC, 0, histB.rows, NORM_MINMAX, -1, Mat());

	double base_base = compareHist(histB, histC, 0);
	double base_half = compareHist(histB, histC, 1);
	double base_test1 = compareHist(histB, histC, 2);
	double base_test2 = compareHist(histB, histC, 3);

	cout << "";
}*/