#include "DisplayWindow.h"



DisplayWindow::DisplayWindow(std::string name, cv::WindowFlags windowFlag):m_name(name), m_flag(windowFlag)
{
	cv::namedWindow(name, windowFlag); // Create a window for display.
}


DisplayWindow::~DisplayWindow()
{
}

void DisplayWindow::ChangeWindowTitle(std::string newName)
{
	cv::setWindowTitle(m_name, newName);

}

void DisplayWindow::ChangeWindowSize(int width, int height)
{
	cv::setWindowProperty(m_name, cv::WindowPropertyFlags::WND_PROP_AUTOSIZE, cv::WindowFlags::WINDOW_NORMAL);
	cv::resizeWindow(m_name, width, height);
}

void DisplayWindow::SetFullScreen(bool active)
{
	if (active)
	{
		cv::setWindowProperty(m_name, cv::WindowPropertyFlags::WND_PROP_FULLSCREEN, cv::WindowFlags::WINDOW_FULLSCREEN);
		return;
	}
	else
	{
		cv::setWindowProperty(m_name, cv::WindowPropertyFlags::WND_PROP_FULLSCREEN, cv::WindowFlags::WINDOW_NORMAL);
		return;
	}
}
