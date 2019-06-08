#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>


namespace ImageOperations //optional, just for clarity
{
	//the higher the divisionValue, the less color variation there will be in the image
	static cv::Mat& ColorReduction(cv::Mat& I, int divisionValue=100)
	{
		double timeBefore = (double)cv::getTickCount();

		//uchar has value 0 to 256
		uchar table[256];
		for (int i = 0; i < 256; ++i)
		{
			table[i] = (uchar)(divisionValue * (i / divisionValue));
		}


		// accept only char type matrices
		CV_Assert(I.depth() == CV_8U);
		int channels = I.channels();
		int nRows = I.rows;
		int nCols = I.cols * channels;
		if (I.isContinuous())
		{
			nCols *= nRows;
			nRows = 1;
		}
		int i, j;
		uchar* p;
		for (i = 0; i < nRows; ++i)
		{
			p = I.ptr<uchar>(i);
			for (j = 0; j < nCols; ++j)
			{
				p[j] = table[p[j]];
			}
		}

		double timeAfter = (double)cv::getTickCount();

		double timeTake = (timeAfter-timeBefore) / cv::getTickFrequency();
		//std::cout << "Color Reduction Operation Time: " << timeTake << std::endl;
		return I;
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

	//KernelSize is the size of the dimensions of the 2D matrix used as kernel
	//thresholdValue is the minimum value pixels need to be, after the image is turned to grayscale, to not be set to 0
	static void ExtractContour(const cv::Mat& image, std::vector<std::vector<cv::Point>> result, int kernelSize = 50,int thresholdValue = 50, bool invertGrayScale = false)
	{
		cv::Mat temp1 , temp2 , temp3;

		cv::cvtColor(image, temp1, cv::COLOR_BGR2GRAY);	// image to grayscale


		if (invertGrayScale)
		{
			//INVERT
			bitwise_not(temp1, temp2);
			cv::threshold(temp2, temp3, thresholdValue, 255, cv::THRESH_BINARY_INV);		
			//threshold = set pixels under min value to 0,
		}
		else
		{
			cv::threshold(temp1, temp3, thresholdValue, 255, cv::THRESH_BINARY);
		}

		cv::Mat kernel = cv::Mat::ones(kernelSize, kernelSize, CV_32F);	//create a kernel


		cv::morphologyEx(temp3, temp1, cv::MORPH_CLOSE, kernel); //fills in the holes in the silhouette
		//cv::Canny(temp3, result, 100, 100);	
		cv::findContours(temp3, result, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE, cv::Point(0, 0));

		return;
	}
}