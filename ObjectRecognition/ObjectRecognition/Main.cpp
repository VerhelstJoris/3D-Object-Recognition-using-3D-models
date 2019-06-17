
#include <iostream>
#include <vector>
#include <opencv2/core.hpp>

#include "OpenGL/OGLRenderer.h"
#include "OpenCV/ContourMatcher.h"

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

#include "OpenCV/ImageOperations.h"

//assimp test
//#include <assimp/scene.h>
//#include <assimp/Importer.hpp>
//#include <assimp/postprocess.h>

int main(void)
{

	//Assimp::Importer Importer;
	//
	//const aiScene* pScene = Importer.ReadFile("../Resources/Stopsign/StopSign.obj", aiProcess_CalcTangentSpace |
	//	aiProcess_Triangulate |
	//	aiProcess_JoinIdenticalVertices |
	//	aiProcess_SortByPType);
	//
	//std::cout << "IMPORTED BOI" << std::endl;

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
	

	//CONTOUR MATCHING TEST
	//cv::Mat testImg = cv::imread("../Resources/Test/suzanne2.jpg");
	//int testResult = matchingObject->MatchImgAgainstContours(testImg);
	//imshow("ContourFit", matchingObject->ContourToMat(testResult));
	////imshow("Contour", drawing);
	//imshow("TestImage", testImg);

	//TESTING
	cv::Mat testImg = cv::imread("../Resources/Test/suzanne2.jpg");
	
	systemObject->SwitchToDisplayMode(testImg);
	systemObject->Run();



	std::vector<std::vector<cv::Point>> contourTestImg;
	std::vector<cv::Vec4i> contourHierarchy;
	
	std::cout << matchingObject->GetAverageAreaRenders() << std::endl;
	
	ImageOperations::ExtractContourFromImage(testImg, contourTestImg, contourHierarchy, matchingObject->GetAverageAreaRenders(), matchingObject->GetAverageSquarenessRenders());
	
	//DRAW CONTOURS
	auto size = testImg.size();
	cv::Mat drawing = cv::Mat::zeros(size.height, size.width, CV_8UC3);	//create a mat the size of the screenshot (contour img has the same size)
	
	cv::Scalar color;
	std::vector<cv::Vec4i> hierarchy;
	
	cv::RNG rng(12345);
	
	for (int i = 0; i < contourTestImg.size(); i++)
	{
		cv::Scalar color;
		//if (contourHierarchy[i][2] != -1) 
		{
			// random colour
			color = cv::Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
			drawContours(drawing, contourTestImg, i, color, 0.5, 8, hierarchy, 0, cv::Point());
		}
	
	}
	
	imshow("Contour", drawing);
	imshow("TestImage", testImg);
	//int id = matchingObject->MatchImgAgainstContours(testImg);
	//imshow("TestContours", matchingObject->ContourToMat(id));
	
	





	cv::waitKey();
	cv::destroyAllWindows();

	// Shutdown and release the OPENGL object.
	systemObject->Shutdown();
	delete systemObject;
	systemObject = 0;


	return 0;
}

