//
// Created by Ramachandran on 23/03/19.
//

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <stdint.h>
#include <iostream>
#include "file_intensity.hpp"

using namespace cv;
using namespace std;

void remove_channel(cv::Mat image, char *channeltype)
{
	//Filters the seleted channel out of image

	int i = 0;

	if (strcmp(channeltype, "BLUE") == 0)
	{
		//Remove Blue Channel
		i = 0;
	}

	else if (strcmp(channeltype, "GREEN") == 0)
	{
		//Remove Green Channel
		i = 1;
	}

	else
	{
		i = 2;
	}

	for (int r = 0; r < image.rows; r++)
	{
		for (int c = 0; c < image.cols; c++)
		{
			//CV Template for 3byte data (Each COLORED image has 3 parts -B,G,R)
			image.at<cv::Vec3b>(r, c)[i] = image.at<cv::Vec3b>(r, c)[i] * 0.0f;
		}
	}
}

void remove_via_merge(cv::Mat image, char *channeltype)
{
	//Filters the seleted channel out of image

	int i = 0;

	if (strcmp(channeltype, "BLUE") == 0)
	{
		//Remove Blue Channel
		i = 0;
	}
	else if (strcmp(channeltype, "GREEN") == 0)
	{
		//Remove Green Channel
		i = 1;
	}
	else
	{
		i = 2;
	}

	Mat splitchannels[3];

	cv::split(image, splitchannels);

	splitchannels[i] = Mat::zeros(splitchannels[i].size(), CV_8UC1);

	merge(splitchannels, 3, image);

	return;
}

void dft_transform(cv::Mat &src, cv::Mat &dst)
{
	//ONLY for GREYSCALE image
	//Performs a Discrete Fourier Transorm of the given image

	assert(src.type() == 0);

	//First, convert the integer fields of the colors of the GREYSCALE image

	Mat FloatArray;

	src.convertTo(FloatArray, CV_32FC1, 1.0 / 255.0);
	Mat ComplexPart[2] = {FloatArray, Mat::zeros(FloatArray.size(), CV_32F)};

	Mat dftReady;

	//Merge both parts
	merge(ComplexPart, 2, dftReady);

	Mat dftOriginal;

	cv::dft(dftReady, dftOriginal, DFT_COMPLEX_OUTPUT);

	dst = dftOriginal;
}

void showDFT(cv::Mat &src)
{
	Mat splitArray[2] = {Mat::zeros(src.size(), CV_32F), Mat::zeros(src.size(), CV_32F)};

	split(src, splitArray);

	//Find Magnitude of the Vector
	Mat dftMagnitude;

	magnitude(splitArray[0], splitArray[1], dftMagnitude);

	//First, use logs to scale down the values
	//To ensure positive values, add 1 to every value
	dftMagnitude += Scalar::all(1);

	log(dftMagnitude, dftMagnitude);

	//Finally, normalize
	normalize(dftMagnitude, dftMagnitude, 0, 1, NORM_MINMAX);

	//Now recentre before displaying
	recenterDFT(dftMagnitude);

	imshow("DFT", dftMagnitude);

	waitKey(0);
}

void swapMatrices(cv::Mat &m1, cv::Mat &m2)
{

	Mat swapMat;

	m1.copyTo(swapMat);
	m2.copyTo(m1);
	swapMat.copyTo(m2);

	return;
}

void recenterDFT(cv::Mat &src)
{
	//Re-centers the DFT

	//Find the midpoint (centre) and swap about the centre
	int centerX = src.cols / 2;
	int centerY = src.rows / 2;

	//  1   2       4   3
	//    -     ->    -
	//  3   4       2   1

	//4 Quadrant image matrices
	//Initialize 4 objects
	Mat q1(src, Rect(0, 0, centerX, centerY));
	Mat q2(src, Rect(centerX, 0, centerX, centerY));
	Mat q3(src, Rect(0, centerY, centerX, centerY));
	Mat q4(src, Rect(centerX, centerY, centerX, centerY));

	swapMatrices(q1, q4);
	swapMatrices(q2, q3);

	// Now the DFT is in such a way such that the lower frequency information has a higher Amplitude about the center
	// as compared to the rest.
}

void image_modifier(Mat original)
{
	cout << "OpenCV Image modifier\n";

	Mat modified = original.clone();
	cvtColor(original, modified, COLOR_BGR2GRAY);

	for (int r = 0; r < modified.rows; r++)
	{
		for (int c = 0; c < modified.cols; c++)
		{
			// We're doing this for a Greyscale image, so the pointer is a <uchar>
			modified.at<uchar>(r, c) = modified.at<uchar>(r, c) * 0.5f;
		}
	}

	cv::Point pt1(0, 0);
	//cv::Point pt2(modified.rows-1,modified.cols-1);
	cv::Point pt2(100, 100);

	rectangle(modified, pt1, pt2, (0, 255, 0));

	//remove_channel(modified,"BLUE");

	//imshow("Original", original);

	//imshow("Modified", modified);

	//remove_via_merge(modified, "BLUE");
	//imshow("Modified", modified);
	//waitKey();

	dft_transform(modified, modified);

	showDFT(modified);

	return;
}
