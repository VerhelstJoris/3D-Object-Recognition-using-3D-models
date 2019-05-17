#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

#include <iostream>

#include "DisplayWindow.h"
#include "ImageOperations.h"

using namespace cv;
using namespace std;

int main(int argc, char** argv)
{
	if (argc != 2)
	{
		cout << " Usage: display_image ImageToLoadAndDisplay" << endl;
		return -1;
	}

	Mat image;
	image = imread(argv[1], IMREAD_COLOR); // Read the file

	if (image.empty()) // Check for invalid input
	{
		cout << "Could not open or find the image" << std::endl;
		return -1;
	}


	//IMAGE REDUCTION
	int divideValue;
	cout << "Please enter an integer value: ";
	cin >> divideValue;
	cout << "The value you entered is " << divideValue;

	uchar table[256];
	for (int i = 0; i < 256; ++i)
	{
		table[i] = (uchar)(divideValue * (i / divideValue));
	}

	//MAIN WINDOW
	DisplayWindow* m_MainWindow = new DisplayWindow("MAIN",WINDOW_NORMAL);
	m_MainWindow->ChangeWindowSize(600, 600);
	m_MainWindow->ChangeWindowTitle("BEFORE");
	imshow(m_MainWindow->GetName(), image); // Show our image inside it.

	Mat secondImage = ImageOperations::ScanImageAndReduceC(image, table);


	//SECONDARY
	DisplayWindow* m_SecondWindow = new DisplayWindow("SECONDARY", WINDOW_NORMAL);
	m_SecondWindow->ChangeWindowSize(600, 600);
	m_SecondWindow->ChangeWindowTitle("AFTER");
	imshow(m_SecondWindow->GetName(), secondImage); // Show our image inside it.

	waitKey(0); // Wait for a keystroke in the window
	return 0;
}

