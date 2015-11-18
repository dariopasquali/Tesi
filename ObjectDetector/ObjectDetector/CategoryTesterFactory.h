#include "CategoryTester.h"

namespace od
{
	class CategoryTesterFactory
	{
	public:

		enum TestMode{
			INTER_CATEGORY_TEST_MODE,
			INTRA_CATEGORY_TEST_MODE
		};

		static CategoryTester* getCategoryTester(TestMode test, od::ObjectDetector* detector, char* datasetDirectory, double hammingThreshold, double correlationThreshold);

		static CategoryTester* getCategoryTester(TestMode test, od::ObjectDetector* detector, double hammingThreshold, double correlationThreshold);

		static CategoryTester* getCategoryTester(TestMode test, od::ObjectDetector* detector, char* datasetDirectory);

		static CategoryTester* getCategoryTester(TestMode test, od::ObjectDetector* detector);


	};
}