#include "CategoryTesterFactory.h"
#include "InterCategoryTester.h"
#include "IntraCategoryTester.h"

using namespace od;
using namespace std;

od::CategoryTester* CategoryTesterFactory::getCategoryTester(TestMode test, od::ObjectDetector* detector, char* datasetDirectory, double hammingThreshold, double correlationThreshold)
{
	if (test == TestMode::INTER_CATEGORY_TEST_MODE)
		return (CategoryTester*) new InterCategoryTester(detector, datasetDirectory, hammingThreshold, correlationThreshold);
	else
		return (CategoryTester*) new IntraCategoryTester(detector, datasetDirectory, hammingThreshold, correlationThreshold);

}

od::CategoryTester* CategoryTesterFactory::getCategoryTester(TestMode test, od::ObjectDetector* detector, double hammingThreshold, double correlationThreshold)
{
	if (test == TestMode::INTER_CATEGORY_TEST_MODE)
		return (CategoryTester*) new InterCategoryTester(detector, hammingThreshold, correlationThreshold);
	else
		return (CategoryTester*) new IntraCategoryTester(detector, hammingThreshold, correlationThreshold);
}

od::CategoryTester* CategoryTesterFactory::getCategoryTester(TestMode test, od::ObjectDetector* detector, char* datasetDirectory)
{
	if (test == TestMode::INTER_CATEGORY_TEST_MODE)
		return (CategoryTester*) new InterCategoryTester(detector, datasetDirectory);
	else
		return (CategoryTester*) new IntraCategoryTester(detector, datasetDirectory);
}

od::CategoryTester* CategoryTesterFactory::getCategoryTester(TestMode test, od::ObjectDetector* detector)
{
	if (test == TestMode::INTER_CATEGORY_TEST_MODE)
		return (CategoryTester*) new InterCategoryTester(detector);
	else
		return (CategoryTester*) new IntraCategoryTester(detector);
}