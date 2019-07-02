#include "ContourMatcher.h"


//OPENCV
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <iostream>
#include <iomanip>


#include "ImageOperations.h"

ContourMatcher::ContourMatcher()
{
}


ContourMatcher::~ContourMatcher()
{
}


bool ContourMatcher::Initialize(std::vector<RenderStruct>& renders)
{
	m_renders = renders;

	GenerateContours();

	return true;
}

void ContourMatcher::GenerateContours()
{
	//reserve space for 60 contours
	std::vector<std::vector<std::vector<cv::Point>>> contours(m_renders.size());
	
	auto size = m_renders[0].renderImage.size();
	m_width = size.width;
	m_height = size.height;

	double averageArea=0;
	double averageSquareness = 0;

	for (size_t i = 0; i < m_renders.size(); i++)
	{
			
		//TO-DO: RESERVE THE SIZE
		std::vector<cv::Point> contourSingle;

		ImageOperations::ExtractContourFromRender(m_renders[i].renderImage, contourSingle);

		//copy contourSignle into m_contoursRenders
		m_contoursRenders.push_back(contourSingle);
	
		averageSquareness += ImageOperations::GetShapeFactor(contourSingle);
		averageArea += cv::contourArea(contourSingle);

		std::cout << "finished generating contour id: " << i << std::endl;
	}
	
	//m_averageArea = averageArea / (double)m_renders.size();
	//m_averageSquareness = averageSquareness / (double)m_renders.size();
	//std::cout << m_averageSquareness << std::endl;

	std::cout << "Finished generating the contours of " << m_renders.size() << " renders" << std::endl;
}

cv::Mat ContourMatcher::ContourToMat(int contourID)
{
	// Draw contours
	cv::Mat drawing = cv::Mat::zeros(m_height,m_width, CV_8UC3);	//create a mat the size of the screenshot (contour img has the same size)

	cv::Scalar color = cv::Scalar(255, 0, 0);
	std::vector<cv::Vec4i> hierarchy;
	
	//cv::drawContours(drawing, m_contoursRenders[contourID], 0, color, 2, 8, hierarchy, 0, cv::Point());
	cv::drawContours(drawing, m_contoursRenders, contourID, color, 2, 8, hierarchy, 0, cv::Point());
	
	return drawing;
}

ContourMatchOut ContourMatcher::MatchImgAgainstContours(cv::Mat image)
{
	//get contours of  image
	std::vector<std::vector<cv::Point>> imageContours;
	std::vector<cv::Vec4i> contourHierarchy;
	ImageOperations::ExtractContourFromImage(image, imageContours, contourHierarchy);


	double lowestResult = 10000000.0;
	int lowestRenderID = 0;
	int lowestImageContourID = 0;
	for (size_t i = 0; i < m_renders.size(); i++)
	{
		double resultMatch = 0.0;

		std::cout << std::fixed;
		std::cout << std::setprecision(2);

		for (size_t j = 0; j < imageContours.size(); j++)
		{
			resultMatch = cv::matchShapes(imageContours[j], m_contoursRenders[i], cv::CONTOURS_MATCH_I1, 0.0);
			//std::cout << i << ": " << resultMatch << "with Contour " << j << " from the test image" << std::endl;
			std::cout << resultMatch << " | ";
			if (resultMatch <= lowestResult)
			{
				lowestResult = resultMatch;
				lowestRenderID = (int)i;
				lowestImageContourID = (int)j;
			}
		}

	
	}
	std::cout << std::endl << "FINISHED MATCHING" << std::endl;

	ContourMatchOut output;
	output.lowestImageContourID = lowestImageContourID;
	output.lowestRenderID = lowestRenderID;
	output.lowestResult = lowestResult;

	output.lowestContourRender.reserve(m_contoursRenders[lowestRenderID].size());
	std::copy(m_contoursRenders[lowestRenderID].begin(), m_contoursRenders[lowestRenderID].end(), std::back_inserter(output.lowestContourRender));

	output.lowestContourImage.reserve(imageContours[lowestImageContourID].size());
	std::copy(imageContours[lowestImageContourID].begin(), imageContours[lowestImageContourID].end(), std::back_inserter(output.lowestContourImage));
	
	std::cout << "Render with ID: " << output.lowestRenderID << " is the best fit with value: " << output.lowestResult << " to contour " << output.lowestImageContourID << std::endl << std::endl;
	std::cout << std::setprecision(9);


	return output;

}



