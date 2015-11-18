#include "ObjectDetector.h"

namespace od
{
	class ObjectDetectorFactory
	{
	public:

		enum DetectorType{
			MONO,
			MULTI
		};

		static OBJECTDETECTOR_API ObjectDetector* getObjectDetector(DetectorType type);
	};
}