
#include <iostream>
#include <vector>
#include <opencv2/core.hpp>

#include "OpenGL/OGLRenderer.h"

#include "OpenCV/ImageOperations.h"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"

int main(void)
{
	//OPENGL object renderer
	OGLRenderer* systemObject;
	bool result;

	// Create the system object.
	systemObject = new OGLRenderer;
	if (!systemObject)
	{
		return 0;
	}

	result = systemObject->Initialize("../Resources/Test/Suzanne.obj");
	if (result)
	{
		systemObject->Run();
	}
	//retrieve the vector of cv::Mat screenshots from the renderer
	std::vector<cv::Mat> m_screenShots = systemObject->GetScreenRenders();

	//sets the size of the vector
	std::vector<std::vector<std::vector<cv::Point> >> contours (m_screenShots.size());
	std::vector<cv::Vec4i> hierarchy;

	//GENERATING THE CONTOURS
	for (size_t i = 0; i < m_screenShots.size(); i++)
	{
		cv::Mat temp = m_screenShots[i];
		cv::Canny(m_screenShots[i], temp, 100, 100);
		cv::findContours(temp, contours[i], cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE, cv::Point(0, 0));
	}
	
	std::cout << "Finished Generating the contours" << std::endl;

	//DISPLAYING THE CONTOURS
	namedWindow("Contours", cv::WINDOW_AUTOSIZE);
	int iteration = 0;
	while (true)
	{
		// Draw contours
		cv::Mat drawing = cv::Mat::zeros(m_screenShots[iteration].size(), CV_8UC3);	//create a mat the size of the screenshot (contour img has the same size)
		for (int i = 0; i < contours[iteration].size(); i++)
		{
			cv::Scalar color = cv::Scalar(255, 0, 0);
			drawContours(drawing, contours[iteration], i, color, 2, 8, hierarchy, 0, cv::Point());
		}

		// Show in a window
		imshow("Contours", drawing);

		cv::waitKey(50);
		iteration = (iteration + 1) % contours.size();
	}


	cv::waitKey(0);


	// Shutdown and release the OPENGL object.
	systemObject->Shutdown();
	delete systemObject;
	systemObject = 0;

	return 0;
}

