#pragma once


#include <vector>

namespace cv 
{ 
	class Mat;
	
	//forward declaration of Point
	template<typename _Tp> class Point_;
	typedef Point_<int> Point2i;
	typedef Point2i Point;

	//forward declaration of Size
	//TO-DO: GET THIS ONE WORKING
	//template<typename _Tp> class Size_;
	//typedef Size_<int> Size2i;
	//typedef Size2i Size;
}	

typedef struct RenderStruct;


class ContourMatcher
{
public:
	ContourMatcher();
	~ContourMatcher();

public:
	int MatchImgAgainstContours(cv::Mat image);

	bool Initialize(std::vector<RenderStruct>& renders);

	double GetAverageAreaRenders() { return m_averageArea; };
	double GetAverageSquarenessRenders() { return m_averageSquareness; };

	cv::Mat ContourToMat(int contourID);


private:
	void GenerateContours();


	std::vector<RenderStruct> m_renders;
	std::vector<std::vector<cv::Point>> m_contours;

	//CAN'T USE UNTIL FWD DECLARATION WORKS
	//cv::Size m_imageSize;
	int m_width, m_height;

	double m_averageArea;
	double m_averageSquareness;

};

