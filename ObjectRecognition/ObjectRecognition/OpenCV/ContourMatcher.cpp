#include "ContourMatcher.h"


//OPENCV
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>


ContourMatcher::ContourMatcher()
{
}


ContourMatcher::~ContourMatcher()
{
}


bool ContourMatcher::Initialize(std::vector<cv::Mat>& renders)
{
	m_renders = renders;
	m_contours.reserve(m_renders.size());

	return true;
}

bool ContourMatcher::GenerateContours()
{
	for (size_t i = 0; i < m_renders.size(); i++)
	{
		cv::Mat temp = m_renders[i];
		//cv::Canny(m_screenShots[i], contours[i], 100, 100);
		cv::Canny(m_renders[i], temp, 100, 100);
		cv::findContours(temp, contours[i], cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);
	}
}

