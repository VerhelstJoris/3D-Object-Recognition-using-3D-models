#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/photo.hpp>

namespace ImageOperations //optional, just for clarity
{
	//the higher the divisionValue, the less color variation there will be in the image
	static void colorReduce(cv::Mat& image, int div = 64)
	{
		int nl = image.rows;                    // number of lines
		int nc = image.cols * image.channels(); // number of elements per line

		for (int j = 0; j < nl; j++)
		{
			// get the address of row j
			uchar* data = image.ptr<uchar>(j);

			for (int i = 0; i < nc; i++)
			{
				// process each pixel
				data[i] = data[i] / div * div + div / 2;
			}
		}
	}
	
	//Increase the contrast of the image
	//myImage is the image that you want to sharpen
	//result is stored in the second parameter variable
	//DO NOT RECOMMEND USING IT, CODE IS HERE FOR EXPLANATION AS TO HOW FUNCTIONALITY WORKS
	static void Sharpen(const cv::Mat& myImage, cv::Mat& Result)
	{
		CV_Assert(myImage.depth() == CV_8U);  // accept only uchar images

		const int nChannels = myImage.channels();
		Result.create(myImage.size(), myImage.type());	//make sure result image has the same size

		for (int j = 1; j < myImage.rows - 1; ++j)
		{
			const uchar* previous = myImage.ptr<uchar>(j - 1);
			const uchar* current = myImage.ptr<uchar>(j);
			const uchar* next = myImage.ptr<uchar>(j + 1);
			uchar* output = Result.ptr<uchar>(j);

			for (int i = nChannels; i < nChannels*(myImage.cols - 1); ++i)
			{
				*output++ = cv::saturate_cast<uchar>(5 * current[i]
					- current[i - nChannels] - current[i + nChannels] - previous[i] - next[i]);
			}
		}

		//borders, just reset to 0 to avoid weird results
		Result.row(0).setTo(cv::Scalar(0));
		Result.row(Result.rows - 1).setTo(cv::Scalar(0));
		Result.col(0).setTo(cv::Scalar(0));
		Result.col(Result.cols - 1).setTo(cv::Scalar(0));
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


		cv::resize(image, temp1, cv::Size(image.size().width / 2, image.size().height/2));
		colorReduce(temp1,64);
		cv::cvtColor(temp1, temp2, cv::COLOR_BGR2GRAY);	// image to grayscale
		cv::blur(temp2, temp1, cv::Size(3, 3));
		
		cv::Canny(temp1, temp2, 0, 100);
		
		std::vector<std::vector<cv::Point>> contours;

		cv::findContours(temp2, contours, hierarchyResult, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE, cv::Point(0, 0));

		//remove unnecessary contours
		for (size_t i = 0; i < contours.size();)
		{
			double area = cv::contourArea(contours[i]);
			if (area <= 500 /*|| !inRange(contourShapeFactor-0.1, contourShapeFactor+0.1, GetShapeFactor(contours[i]))*/)
			{
				contours.erase(contours.begin() + i);
				//std::cout << "my 'King Crimson' erases elements and skips past them" << std::endl;
			}
			else
			{
				std::cout << GetShapeFactor(contours[i]) << "VS: " << contourShapeFactor << std::endl;
				i++;
			}
		}

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