#pragma once


#include <iostream>
#include <string>
#include <opencv2/highgui.hpp>



class DisplayWindow
{
public:
	DisplayWindow(std::string name, cv::WindowFlags windowFlag );
	~DisplayWindow();

	void ChangeWindowTitle(std::string);
	void ChangeWindowSize(int, int);
	void SetFullScreen(bool);
public:
	std::string GetName() { return m_name; };

private:
	std::string m_name;
	cv::WindowFlags m_flag;

};

