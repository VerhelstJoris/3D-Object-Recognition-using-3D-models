#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/photo.hpp>


struct RenderStruct
{
	cv::Mat renderImage;
	float rotationX;
	float rotationY;
	float rotationZ;
};

namespace ImageOperations //optional, just for clarity
{

	// quantize the image to numBits 
	static cv::Mat ColorReduce(const cv::Mat& inImage, int numBits)
	{
		cv::Mat retImage = inImage.clone();

		uchar maskBit = 0xFF;

		// keep numBits as 1 and (8 - numBits) would be all 0 towards the right
		maskBit = maskBit << (8 - numBits);

		for (int j = 0; j < retImage.rows; j++)
		{
			for (int i = 0; i < retImage.cols; i++)
			{
				cv::Vec3b valVec = retImage.at<cv::Vec3b>(j, i);
				valVec[0] = valVec[0] & maskBit;
				valVec[1] = valVec[1] & maskBit;
				valVec[2] = valVec[2] & maskBit;
				retImage.at<cv::Vec3b>(j, i) = valVec;
			}
		}
		
		return retImage;
	}
	
	static double GetShapeFactor(std::vector<cv::Point> contour)
	{
		double area = cv::contourArea(contour);
		double arc = cv::arcLength(contour, true);

		double squareness = 4 * 3.14159f * area / (arc * arc);

		return squareness;
	}

	static bool inRange(double low, double high, double x)
	{
		return (low <= x && x <= high);
	}

	//KernelSize is the size of the dimensions of the 2D matrix used as kernel
	//thresholdValue is the minimum value pixels need to be, after the image is turned to grayscale, to not be set to 0
	static void ExtractContourFromImage(const cv::Mat& image, std::vector<std::vector<cv::Point>>& result, std::vector<cv::Vec4i>& hierarchyResult, double contourArea, double contourShapeFactor, int kernelSize = 50,int thresholdValue = 50)
	{
		cv::Mat temp1 , temp2;
		temp1 = image;
		//cv::resize(image, temp1, cv::Size(image.size().width / 2, image.size().height/2));
		temp2 = ColorReduce(temp1, 2);

		cv::cvtColor(temp2, temp1, cv::COLOR_BGR2GRAY);	// image to grayscale
		cv::blur(temp1, temp2, cv::Size(3,3));

		cv::Canny(temp2, temp1, 0, 100);

		std::vector<std::vector<cv::Point>> contours;

		cv::findContours(temp1, contours, hierarchyResult, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE, cv::Point(0, 0));

		//remove unnecessary contours
		//for (size_t i = 0; i < contours.size();)
		//{
		//	double area = cv::contourArea(contours[i]);
		//	if (area <= 500 /*|| !inRange(contourShapeFactor-0.1, contourShapeFactor+0.1, GetShapeFactor(contours[i]))*/)
		//	{
		//		contours.erase(contours.begin() + i);
		//		//std::cout << "my 'King Crimson' erases elements and skips past them" << std::endl;
		//	}
		//	else
		//	{
		//		//std::cout << GetShapeFactor(contours[i]) << "VS: " << contourShapeFactor << std::endl;
		//		i++;
		//	}
		//}

		result = contours;

		return;
	}

	static void ExtractContourFromRender(const cv::Mat& image, std::vector<cv::Point>& result, int kernelSize = 50, int thresholdValue = 50, bool invertGrayScale = false)
	{
		cv::Mat temp;

		cv::cvtColor(image, temp, cv::COLOR_BGR2GRAY);	// image to grayscale

		//cv::Canny(temp3, result, 100, 100);	
		std::vector<std::vector<cv::Point>> contours;

		cv::findContours(temp, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE, cv::Point(0, 0));

		//make vector of vector<vector<point>> to just vector<point>
		for (size_t vecElement = 0; vecElement < contours.size(); vecElement++)
		{
			result.insert(result.end(), contours.at(vecElement).begin(), contours.at(vecElement).end());
		}

		return;
	}


}