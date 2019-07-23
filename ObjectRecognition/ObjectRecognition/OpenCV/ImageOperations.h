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
	static void ExtractContourFromImage(const cv::Mat& image, std::vector<std::vector<cv::Point>>& result, std::vector<cv::Vec4i>& hierarchyResult)
	{
		cv::Mat temp1 , temp2;
		std::vector<std::vector<cv::Point>> contours;
		temp1 = image;

		//cv::resize(image, temp1, cv::Size(image.size().width / 2, image.size().height/2));
		temp2 = ColorReduce(temp1, 2);

		cv::cvtColor(temp2, temp1, cv::COLOR_BGR2GRAY);	// image to grayscale
		cv::blur(temp1, temp2, cv::Size(3,3));

		cv::Canny(temp2, temp1, 0, 100);

		//THRESHOLD THE IMAGE????????
		//gray_image = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
		//ret, thresh = cv2.threshold(gray_image, 50, 255, cv2.THRESH_BINARY)
		//blur = cv2.medianBlur(thresh, 7)
		//img2, contours, hierarchy = cv2.findContours(~blur, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)

		cv::findContours(temp1, contours, hierarchyResult, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE, cv::Point(0, 0));

		//remove small contours
		float maxSize = image.size().width * image.size().height / 400.0f;

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


		//DRAW THE CONTOURS
		cv::Mat drawing = cv::Mat::zeros(image.size().height, image.size().width, CV_8UC3);

		//paste together contours that are on the same level???
		for (size_t i = 0; i < contours.size(); i++)
		{
			cv::RNG rng(12345);
			cv::Scalar color = cv::Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
			cv::drawContours(drawing, contours, i, color, 1, 8, hierarchyResult, 0, cv::Point());

		}

		cv::imshow("IMAGE CONTOURS", drawing);

		result = contours;

		return;
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

	static bool FindBlobs(const std::vector<cv::Point> &contours, cv::Point2f &MassCentre, float& DiagonalLength)
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

		return 1;
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