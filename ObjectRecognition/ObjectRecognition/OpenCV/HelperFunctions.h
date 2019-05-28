#pragma once


#include <opencv2/opencv.hpp>

#include "DisplayWindow.h"


namespace HelperFunctions //optional, just for clarity
{

	static void ShowMultipleImages(DisplayWindow* window, std::vector<cv::Mat> images) {
		int size;
		int i;
		int m, n;
		int x, y;

		size_t amountOfImages = images.size();

		// imagesRow - Maximum number of images in a row
		// imagesCol - Maximum number of images in a column
		int imagesRow, imagesCol;

		// scale - How much we have to resize the image
		float scale;
		int max;

		// If the number of arguments is lesser than 0 or greater than 12
		// return without displaying
		if (amountOfImages <= 0) {
			printf("Number of arguments too small....\n");
			return;
		}
		else if (amountOfImages > 14) {
			printf("Number of arguments too large, can only handle maximally 12 images at a time ...\n");
			return;
		}
		// Determine the size of the image,
		// and the number of rows/cols
		// from number of arguments
		else if (amountOfImages == 1) {
			imagesRow = imagesCol = 1;
			size = 300;
		}
		else if (amountOfImages == 2) {
			imagesRow = 2; imagesCol = 1;
			size = 300;
		}
		else if (amountOfImages == 3 || amountOfImages == 4) {
			imagesRow = 2; imagesCol = 2;
			size = 300;
		}
		else if (amountOfImages == 5 || amountOfImages == 6) {
			imagesRow = 3; imagesCol = 2;
			size = 200;
		}
		else if (amountOfImages == 7 || amountOfImages == 8) {
			imagesRow = 4; imagesCol = 2;
			size = 200;
		}
		else {
			imagesRow = 4; imagesCol = 3;
			size = 150;
		}

		// Create a new 3 channel image
		cv::Mat DispImage = cv::Mat::zeros(cv::Size(100 + size * imagesRow, 60 + size * imagesCol), CV_8UC3);


		// Loop for nArgs number of arguments
		for (i = 0, m = 20, n = 20; i < amountOfImages; i++, m += (20 + size)) {
			// Get the Pointer to the IplImage
			cv::Mat img = images.at(i);

			// Check whether it is NULL or not
			// If it is NULL, release the image, and return
			if (img.empty()) {
				printf("Invalid arguments");
				return;
			}

			// Find the width and height of the image
			x = img.cols;
			y = img.rows;

			// Find whether height or width is greater in order to resize the image
			max = (x > y) ? x : y;

			// Find the scaling factor to resize the image
			scale = (float)((float)max / size);

			// Used to Align the images
			if (i % imagesRow == 0 && m != 20) {
				m = 20;
				n += 20 + size;
			}

			// Set the image ROI to display the current image
			// Resize the input image and copy the it to the Single Big Image
			cv::Rect ROI(m, n, (int)(x / scale), (int)(y / scale));
			cv::Mat temp = img; 
			resize(img, temp, cv::Size(ROI.width, ROI.height));
			temp.copyTo(DispImage(ROI));

		}

		// Create a new window, and show the Single Big Image
		imshow(window->GetName(), DispImage);

	}

}