#include "ContourMatcher.h"


//OPENCV
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <iostream>

#include "ImageOperations.h"

ContourMatcher::ContourMatcher()
{
}


ContourMatcher::~ContourMatcher()
{
}


bool ContourMatcher::Initialize(std::vector<cv::Mat>& renders)
{
	m_renders = renders;

	GenerateContours();

	return true;
}

void ContourMatcher::GenerateContours()
{
	//reserve space for 60 contours
	std::vector<std::vector<std::vector<cv::Point>>> contours(m_renders.size());
	
	auto size = m_renders[0].size();
	m_width = size.width;
	m_height = size.height;

	for (size_t i = 0; i < m_renders.size(); i++)
	{
			
		//TO-DO: RESERVE THE SIZE
		std::vector<cv::Point> contourSingle;

		ImageOperations::ExtractContour(m_renders[i], contourSingle);

		//copy contourSignle into m_contours
		m_contours.push_back(contourSingle);
	

		std::cout << "finished copying vector into single vector" << std::endl;
	}
	
	std::cout << "Finished generating the contours of " << m_renders.size() << " renders" << std::endl;
}

cv::Mat ContourMatcher::ContourToMat(int contourID)
{
	// Draw contours
	cv::Mat drawing = cv::Mat::zeros(m_height,m_width, CV_8UC3);	//create a mat the size of the screenshot (contour img has the same size)

	cv::Scalar color = cv::Scalar(255, 0, 0);
	std::vector<cv::Vec4i> hierarchy;
	
	//cv::drawContours(drawing, m_contours[contourID], 0, color, 2, 8, hierarchy, 0, cv::Point());
	cv::drawContours(drawing, m_contours, contourID, color, 2, 8, hierarchy, 0, cv::Point());
	
	return drawing;
}

int ContourMatcher::MatchImgAgainstContours(cv::Mat image)
{
	//get contours of  image
	std::vector<cv::Point> imageContours;
	ImageOperations::ExtractContour(image, imageContours);


	//actually match test
	double lowestResult = 25.0;
	int lowestID = 0;
	for (size_t i = 0; i < m_renders.size(); i++)
	{
		double resultMatch = 0.0;

		//test by matching Mat against Mat (both single channel)
		cv::Mat temp;
		cv::Canny(m_renders[i], temp, 100, 100);
		//resultMatch= cv::matchShapes(grayImg, temp, cv::CONTOURS_MATCH_I1, 0.0);

		resultMatch= cv::matchShapes(imageContours, m_contours[i], cv::CONTOURS_MATCH_I1, 0.0);
		std::cout << i << ": " << resultMatch << std::endl;
		if (resultMatch <= lowestResult)
		{
			lowestResult = resultMatch;
			lowestID = i;
		}
	}

	std::cout << "Render with ID: " << lowestID << " is the best fit with value: " << lowestResult << std::endl;

	return lowestID;

}

