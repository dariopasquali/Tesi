#include "ObjectDetectorFactory.h"
#include "MultiContourObjectDetector.h"
#include "MonoContourObjectDetector.h"
#include "commonInclude.h"


using namespace std;

namespace od{

	ObjectDetector* ObjectDetectorFactory::getObjectDetector(DetectorType type)
	{
		if (type == DetectorType::MONO)
			return (ObjectDetector*)new MonoContourObjectDetector();

		else if (type == DetectorType::MULTI)
			return (ObjectDetector*)new MultiContourObjectDetector();

		else
		{
			cerr << "invalid contoursNumber" << endl;
			return NULL;
		}
	}
}

