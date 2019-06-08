#pragma once


#include <vector>



namespace cv { class Mat; }	//forward declaration


class ContourMatcher
{
public:
	ContourMatcher();
	~ContourMatcher();

public:
	bool MatchImgAgainstContours();

	bool Initialize(std::vector<cv::Mat>& renders);

private:
	bool GenerateContours();

	std::vector<cv::Mat> m_renders;
	std::vector<std::vector<cv::Point>> m_contours;

};

