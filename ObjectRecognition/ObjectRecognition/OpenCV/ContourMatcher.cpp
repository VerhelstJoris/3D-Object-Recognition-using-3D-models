#include "ContourMatcher.h"


//OPENCV
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <iostream>

ContourMatcher::ContourMatcher()
{
}


ContourMatcher::~ContourMatcher()
{
}


bool ContourMatcher::Initialize(std::vector<cv::Mat>& renders)
{
	m_renders = renders;
	//m_contours.reserve(m_renders.size());

	GenerateContours();

	return true;
}

void ContourMatcher::GenerateContours()
{

	std::vector<std::vector<std::vector<cv::Point>>> contours(m_renders.size());
	
	cv::Mat temp;
	auto size = m_renders[0].size();
	m_width = size.width;
	m_height = size.height;

	for (size_t i = 0; i < m_renders.size(); i++)
	{
		cv::Mat temp = m_renders[i];
	
		//cv::Canny(m_screenShots[i], contours[i], 100, 100);
		cv::Canny(m_renders[i], temp, 100, 100);
		cv::findContours(temp, contours[i], cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);
	}
	
	//m_contours = contours;
	std::cout << "Finished generating the contours of " << m_renders.size() << " renders" << std::endl;
}

cv::Mat ContourMatcher::ContourToMat(int contourID)
{
	// Draw contours
	cv::Mat drawing = cv::Mat::zeros(m_height,m_width, CV_8UC3);	//create a mat the size of the screenshot (contour img has the same size)

	cv::Point;
	cv::Size;
	for (int i = 0; i < m_contours[contourID].size(); i++)
	{
		cv::Scalar color = cv::Scalar(255, 0, 0);
		std::vector<cv::Vec4i> hierarchy;
		cv::drawContours(drawing, m_contours[i], i, color, 2, 8, hierarchy, 0, cv::Point());
	}

	return drawing;
}

double ContourMatcher::MatchImgAgainstContours(cv::Mat image)
{
	//get contours of  image
	cv::Mat grayImg;
	cv::Canny(image, grayImg, 100, 100);
	std::vector<std::vector<cv::Point>> testContours;
	cv::findContours(grayImg, testContours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);

	//actually match test
	double lowestResult = 25.0f;
	int lowestID = 0;
	for (size_t i = 0; i < m_contours.size(); i++)
	{
		double resultMatch = 0.0;
		resultMatch= cv::matchShapes(testContours, m_contours[i], cv::CONTOURS_MATCH_I2, 0.0);
	
		std::cout << i << ": " << resultMatch << std::endl;
		if (resultMatch <= lowestResult)
		{
			lowestResult = resultMatch;
			lowestID = i;
		}
	}

	return lowestResult;

}

