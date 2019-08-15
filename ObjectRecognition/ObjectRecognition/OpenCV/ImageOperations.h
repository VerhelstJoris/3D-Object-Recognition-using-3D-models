#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/photo.hpp>
#include <opencv2/highgui.hpp>	//for test cv::imshow


struct RenderStruct
{
	cv::Mat renderImage;
	float rotationX;
	float rotationY;
	float rotationZ;
};

struct ContourMatchOut
{
	int lowestRenderID;
	int lowestImageContourID;
	double lowestResult;
	std::vector<cv::Point> lowestContourRender;
	std::vector<cv::Point> lowestContourImage;
};

namespace ImageOperations //optional, just for clarity
{

	// quantize the image to numBits 
	static cv::Mat ColorReduce(const cv::Mat& inImage, int numBits)
	{
		cv::Mat retImage = inImage.clone();

		uchar maskBit = 0xFF;

		// keep numBits as 1 and (8 - numBits) would be all 0 towards the right
		maskBit = maskBit << (8 - numBits);

		for (int j = 0; j < retImage.rows; j++)
		{
			for (int i = 0; i < retImage.cols; i++)
			{
				cv::Vec3b valVec = retImage.at<cv::Vec3b>(j, i);
				valVec[0] = valVec[0] & maskBit;
				valVec[1] = valVec[1] & maskBit;
				valVec[2] = valVec[2] & maskBit;
				retImage.at<cv::Vec3b>(j, i) = valVec;
			}
		}
		
		return retImage;
	}
	
	static double GetShapeFactor(std::vector<cv::Point> contour)
	{
		double area = cv::contourArea(contour);
		double arc = cv::arcLength(contour, true);

		double squareness = 4 * CV_PI * area / (arc * arc);

		return squareness;
	}

	static bool inRange(double low, double high, double x)
	{
		return (low <= x && x <= high);
	}

	//KernelSize is the size of the dimensions of the 2D matrix used as kernel
	//thresholdValue is the minimum value pixels need to be, after the image is turned to grayscale, to not be set to 0
	static bool ExtractContourFromImage(const cv::Mat& image, std::vector<std::vector<cv::Point>>& result, std::vector<cv::Vec4i>& hierarchyResult)
	{
		cv::Mat temp1;
		std::vector<std::vector<cv::Point>> contours;

		//OLD
		//========================
		temp1 = image;

		//cv::resize(image, temp1, cv::Size(image.size().width / 2, image.size().height / 2));
		cv::blur(temp1, temp1, cv::Size(3, 3));
		//temp1 = ColorReduce(temp1, 4);
		cv::cvtColor(temp1, temp1, cv::COLOR_BGR2GRAY);	// image to grayscale
		//cv::threshold(temp1, temp1, 75, 255, cv::THRESH_BINARY);
		
		int lowThreshold = 0;
		cv::Canny(temp1, temp1, lowThreshold, lowThreshold*3, 3);

		//NEW TEST TO SEE IF BETTER RESULTS ARE ACHIEVABLE
		//=========================
		//Prepare the image for findContours
		////cv::resize(image, temp1, cv::Size(image.size().width / 2, image.size().height / 2));
		//cv::cvtColor(temp1, temp1, cv::COLOR_BGR2GRAY);
		//cv::blur(temp1, temp1, cv::Size(5, 5));
		//int lowThreshold = 10;
		//cv::Canny(temp1, temp1, lowThreshold, lowThreshold*3, 3);
		

		//LAPLACIAN TEST FOR FUTURE RESEARCH
		//=========================
		//cv::GaussianBlur(image, temp1, cv::Size(3, 3), 0, 0, cv::BORDER_DEFAULT);
		//cv::cvtColor(temp1, temp1, cv::COLOR_BGR2GRAY);
		//cv::Laplacian(temp1, temp2, CV_16S, 3, 1, 0, cv::BORDER_DEFAULT);
		//cv::convertScaleAbs(temp2, temp1);
		//
		//cv::imshow("LAPLACIAN", temp1);
		//cv::waitKey(10);
		//
		//int lowThreshold = 1;
		//cv::Canny(temp1, temp1, lowThreshold, lowThreshold*3, 3);

		//Find the contours
		cv::findContours(temp1, contours, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);


		//remove small contours
		float maxSize = image.size().width * image.size().height / 1600;
		//float maxSize = 40;

		for (size_t i = 0; i < contours.size();)
		{
			double area = cv::contourArea(contours[i]);
			if (area <= maxSize /*|| !inRange(contourShapeFactor-0.1, contourShapeFactor+0.1, GetShapeFactor(contours[i]))*/)
			{
				contours.erase(contours.begin() + i);
				//std::cout << "my 'King Crimson' erases elements and skips past them" << std::endl;
			}
			else
			{
				//std::cout << GetShapeFactor(contours[i]) << "VS: " << contourShapeFactor << std::endl;
				i++;
			}
		
			
		}

		if (contours.size() <= 0)
		{
			std::cout << std::endl << "NO CONTOURS FOUND IN IMAGE THAT ARE USABLE" << std::endl;
			return false;
		}


		result = contours;

		//DRAW CONTOURS
		std::vector<cv::Vec4i> hierarchyTemp;
		cv::RNG rng(897324);
		cv::Mat drawing = cv::Mat::zeros(image.size(), CV_8UC3);     // change to the size of your image

		for (int i = 0; i < contours.size(); i++) {
			cv::Scalar color = cv::Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
			drawContours(drawing, contours, i, color,1, 8, hierarchyTemp);
		}

		cv::imshow("IMAGE CONTOURS", drawing);

		cv::waitKey(10);

		return true;

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

	static double distanceBtwPoints(const cv::Point2f &point1, const cv::Point2f &point2)
	{
		//simple euclidean distance
		double xDiff = point1.x - point2.x;
		double yDiff = point1.y - point2.y;

		return std::sqrt((xDiff * xDiff) + (yDiff * yDiff));
	}

	static void AngleContour(const std::vector<cv::Point>& contour,double& angle, double& rectAngle)
	{
		cv::RotatedRect _minAreaRect;

		_minAreaRect = cv::minAreaRect(contour);
		cv::Point2f pts[4];
		_minAreaRect.points(pts);

		double dist0 = ImageOperations::distanceBtwPoints(pts[0], pts[1]);
		double dist1 = ImageOperations::distanceBtwPoints(pts[1], pts[2]);

		rectAngle = _minAreaRect.angle;
		angle = atan2(pts[0].y - pts[1].y, pts[0].x - pts[1].x) * 180.0 / CV_PI;
	}

	static cv::Point RotatePoint(const cv::Mat &image, const cv::Point &point)
	{
		cv::Point2f rotated;
		rotated.x = (float)(image.at<double>(0, 0)*point.x + image.at<double>(0, 1)*point.y + image.at<double>(0, 2));
		rotated.y = (float)(image.at<double>(1, 0)*point.x + image.at<double>(1, 1)*point.y + image.at<double>(1, 2));
		return rotated;
	}

	static void FindBlobs(const std::vector<cv::Point> &contours, cv::Point2f &MassCentre, float& DiagonalLength)
	{
	
		cv::Rect boundsRect;
		cv::Moments momentsVal;

		// Find Minimum bounding Rect
		boundsRect = cv::boundingRect(cv::Mat(contours));

		// Get the moments 
		momentsVal = moments(contours, false);

		// Find the Mass Centre
		MassCentre = (cv::Point2f(static_cast<float>(momentsVal.m10 / momentsVal.m00), static_cast<float>(momentsVal.m01 / momentsVal.m00)));

		//Find Distance between the TopLeft  and Bottrom Right Points
		DiagonalLength = ((float)cv::norm(boundsRect.tl() - boundsRect.br()));
		
		//std::cout << "DIAGONAL LENGTH OF: " << diagonalLength << std::endl;
		//std::cout << "CENTER AT: " << massCentre.x << ", " << massCentre.y << std::endl << std::endl;
	}

	static bool TranslateContour(std::vector<cv::Point> contours, std::vector<cv::Point> &result, cv::Vec2f Translate)
	{
		if (contours.size() == 0)
		{
			std::cout << "Invalid input!";
			return 0;
		}

		result = contours;

		for (size_t i = 0; i < contours.size(); i++)
		{	
			result[i] = cv::Point(contours[i].x + Translate(0), contours[i].y + Translate(1));
		}
		return 1;
	}

	static void TranslateContourToPoint(const std::vector<cv::Point>& contour, std::vector<cv::Point>& result, cv::Point endPoint, cv::Point2f &massCentre, float& diagonalLength)
	{
		ImageOperations::FindBlobs(contour, massCentre, diagonalLength);

		std::vector<cv::Point> contours_Trans(contour.size());
		cv::Point2f ptCCentre(diagonalLength / 2, diagonalLength / 2);
		//cv::Vec2f Translation(ptCCentre.x - massCentre.x, ptCCentre.y - massCentre.y);
		cv::Vec2f Translation(massCentre.x - endPoint.x, massCentre.y - endPoint.y);

		//Move the contour Mass Center to the Image Space Centre!
		ImageOperations::TranslateContour(contour, contours_Trans, -Translation);

		result = contours_Trans;
	}

	//rotate a single contour around a centerpoint
	static bool RotateContour(const std::vector<cv::Point>& contour, std::vector<cv::Point> &result, float angle, cv::Point2f center)
	{
		if (contour.size() == 0)
		{
			std::cout << "Invalid input!";
			return false;
		}

		cv::Mat mRot = cv::getRotationMatrix2D(center, (double)angle, 1.0);
		result = contour;

		for (int i = 0; i < contour.size(); i++)
		{
			result[i] = RotatePoint(mRot, contour[i]);
		}
		
		return true;
	}

	static void ScaleContour(const std::vector<cv::Point>& contour, std::vector<cv::Point>& result, cv::Point2f anchor, float scaleAmount)
	{
		//scale separately for X and Y??
		std::vector<cv::Point> resultVec;

		for (size_t i = 0; i < contour.size(); i++)
		{
			cv::Vec2f moveVec = { (float)contour[i].x - anchor.x , (float)contour[i].y - anchor.y };
			moveVec *= (scaleAmount);
			cv::Point newPoint = cv::Point(anchor.x + moveVec[0], anchor.y + moveVec[1]);
			resultVec.push_back(newPoint);
		}

		result = resultVec;
	}

	static void simpleContour(const std::vector<cv::Point>& contour, std::vector<cv::Point>& result ,int n = 100)
	{
		std::vector<cv::Point> contourVec;

		for (size_t p = 0; p < contour.size(); p++)
		{
			contourVec.push_back(contour[p]);
		}

		// In case actual number of points is less than n
		int dummy = 0;
		for (int add = (int)contour.size() - 1; add < n; add++)
		{
			dummy = (dummy + 1) % contour.size();
			contourVec.push_back(contour[dummy++]); //adding dummy values
		}

		// Uniformly sampling
		random_shuffle(contourVec.begin(), contourVec.end());
		std::vector<cv::Point> cont;
		for (int i = 0; i < n; i++)
		{
			cont.push_back(contourVec[i]);
		}

		result = cont;
	}

}