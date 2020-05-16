#include<iostream>
#include "opencv2/opencv.hpp"
#include<string.h>
#include "opencv2/shape.hpp"


void image_modifier(cv::Mat original);

void remove_channel(cv::Mat image,char* channeltype);

void remove_via_merge(cv::Mat image,char* channeltype);

void dft_transform(cv::Mat& src , cv::Mat& dst);

void showDFT(cv::Mat& src);

void recenterDFT(cv::Mat& src);

void swapMatrices(cv::Mat& m1,cv::Mat& m2);
