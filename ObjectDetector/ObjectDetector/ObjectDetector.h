#ifndef OBJECT_DETECTOR
#define OBJECT_DETECTOR

#ifdef OBJECT_DETECTOR_DLL_EXPORT
#define OBJECT_DETECTOR_DLL_API __declspec(dllexport) 
#else
#define OBJECT_DETECTOR_DLL_API __declspec(dllimport) 
#endif

#include "opencv2/opencv.hpp"

namespace od
{
	class ObjectDetector
	{
	public:

		enum OutputMaskMode{
			PRECISE_CONTOURS,
			NO_MASK
		};

		ObjectDetector();

		cv::Mat findObjectsInImage(cv::Mat& image,
			double hammingThreshold,
			double correlationThreshold,
			OutputMaskMode maskMode,
			std::vector<std::vector<std::vector<cv::Point>>>* detectedContours,
			int*numberOfObject);

		bool loadImage(cv::Mat& baseImage);

	private:

		virtual bool findBaseShape(cv::Mat& baseImage) = 0;

		virtual std::vector<std::vector<std::vector<cv::Point>>> findApproxContours(
			cv::Mat image,
			bool performOpening,
			bool findBaseShape) = 0;

		virtual std::vector<std::vector<std::vector<cv::Point>>> processContours(
			std::vector<std::vector<std::vector<cv::Point>>> approxContours,
			double hammingThreshold,
			double correlationThreshold,
			int* numberOfObject) = 0;

		cv::Mat generateDetectionMask(
			std::vector<std::vector<std::vector<cv::Point>>> detectedObjects,
			cv::Mat& image,
			OutputMaskMode maskMode);

	protected:
		cv::Size _baseSize;

		int _minContourPoints;
		int _aspectedContours;

		const double _deleteFocus;
		const double _attenuationFocus;
		cv::Rect _deleteRect;
		cv::Rect _attenuationRect;
	};
}

#endif