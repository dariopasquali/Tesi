#ifndef CATEGORY_TESTER
#define CATEGORY_TESTER


#include "ObjectDetector.h"
#include <time.h>
#include <windows.h>
#include <tchar.h> 
#include <stdio.h>
#include <strsafe.h>

namespace od
{
	class CategoryTester
	{
	public:
	
		CategoryTester(od::ObjectDetector* detector);

		CategoryTester(od::ObjectDetector* detector, char* datasetDirectory);

		CategoryTester(od::ObjectDetector* detector, char* datasetDirectory, double hammingThreshold, double correlationThreshold);

		CategoryTester(od::ObjectDetector* detector, double hammingThreshold, double correlationThreshold);

		std::vector<std::string> loadCategories();

		bool setCategory(char* category);

		double startTest();

	private:

		virtual double categoryDetectionRate() = 0;

	protected:

		char* _datasetDirectory;
		std::vector<std::string> _categories;
		char* _choosenCategory;

		od::ObjectDetector* _detector;
		double _hammingThreshold;
		double _correlationThreshold;
	};
}

#endif
