
#include <iostream>
#include <vector>
#include <opencv2/core.hpp>

#include "OpenGL/OGLRenderer.h"
#include "OpenCV/ContourMatcher.h"


int main(void)
{
	//OPENGL object renderer
	OGLRenderer* systemObject;
	ContourMatcher* matchingObject;

	bool result;

	// Create the system object.
	systemObject = new OGLRenderer;
	if (!systemObject)
	{
		return 0;
	}

	//create the contourmatcher
	matchingObject = new ContourMatcher();
	if (!matchingObject)
	{
		return 0;
	}

	//initialize the renderer
	result = systemObject->Initialize("../Resources/Test/Suzanne.obj");
	if (result)
	{
		systemObject->Run();
	}
	//retrieve the vector of cv::Mat screenshots from the renderer
	matchingObject->Initialize(systemObject->GetScreenRenders());


	//sets the size of the vector
	//std::vector<std::vector<cv::Point>> contours (m_screenShots.size());
	//std::vector<cv::Vec4i> hierarchy;
	//

	//
	//std::cout << "Finished Generating the contours" << std::endl;

	////DISPLAYING THE CONTOURS
	//namedWindow("Contours", cv::WINDOW_AUTOSIZE);
	//int iteration = 0;
	//while (true)
	//{
	//	// Draw contours
	//	cv::Mat drawing = cv::Mat::zeros(m_screenShots[iteration].size(), CV_8UC3);	//create a mat the size of the screenshot (contour img has the same size)
	//	for (int i = 0; i < contours[iteration].size(); i++)
	//	{
	//		cv::Scalar color = cv::Scalar(255, 0, 0);
	//		drawContours(drawing, contours[iteration], i, color, 2, 8, hierarchy, 0, cv::Point());
	//	}
	//
	//	// Show in a window
	//	imshow("Contours", drawing);
	//
	//	cv::waitKey(50);
	//	iteration = (iteration + 1) % contours.size();
	//}


	//CONTOUR MATCHING TEST
	//cv::Mat testImg = cv::imread("../Resources/Test/Capture.jpg");
	//cv::Mat testImg2 = cv::imread("../Resources/Test/Capture3.jpg");
	//cv::Mat temp1, temp2;
	//
	////get contours of test image
	//cv::Canny(testImg, temp1, 100, 100);
	//std::vector<cv::Point> testContours;
	//cv::findContours(temp1, testContours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);
	//
	//cv::Canny(testImg2, temp2, 100, 100);


	////match the shapes
	//double lowestResult = 25.0f;
	//int lowestID = 0;
	//for (size_t i = 0; i < contours.size(); i++)
	//{
	//	double resultMatch = 0.0;
	//	resultMatch= cv::matchShapes(temp2, contours[i], cv::CONTOURS_MATCH_I2, 0.0);
	//
	//	std::cout << i << ": " << resultMatch << std::endl;
	//	if (resultMatch <= lowestResult)
	//	{
	//		lowestResult = resultMatch;
	//		lowestID = i;
	//	}
	//}
	//
	//
	//std::cout << "Best Match ID: " << lowestID << ", result: " << lowestResult << std::endl;
	//
	////Draw contours
	//cv::Mat drawing = cv::Mat::zeros(m_screenShots[lowestID].size(), CV_8UC3);	//create a mat the size of the screenshot (contour img has the same size)
	
	
	// Show in a window
	//imshow("Contours", contours[lowestID]);

	//cv::waitKey();
	std::cin.get();
	std::cin.get();

	// Shutdown and release the OPENGL object.
	systemObject->Shutdown();
	delete systemObject;
	systemObject = 0;

	return 0;
}

