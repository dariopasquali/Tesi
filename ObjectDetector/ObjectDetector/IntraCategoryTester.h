#include "CategoryTester.h"

#include "opencv2/opencv.hpp"



namespace od
{
	class IntraCategoryTester : public CategoryTester{

	public:

		IntraCategoryTester(od::ObjectDetector* detector);

		IntraCategoryTester(od::ObjectDetector* detector, char* datasetDirectory);

		IntraCategoryTester(od::ObjectDetector* detector, char* datasetDirectory, double hammingThreshold, double correlationThreshold);

		IntraCategoryTester(od::ObjectDetector* detector, double hammingThreshold, double correlationThreshold);

		double categoryDetectionRate();
	};
}