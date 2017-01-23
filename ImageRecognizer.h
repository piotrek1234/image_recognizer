#pragma once

#include "opencv2/imgproc.hpp"
#include "Element.h"
#include <vector>
#include <utility>
#include <queue>
#include <cstdlib>
#include <time.h>
#include <iostream>
#include <algorithm>

using namespace cv;
using namespace std;

class ImageRecognizer
{
public:
	ImageRecognizer();
	~ImageRecognizer();
	void filterColor(Mat& img, Vec3b color1, Vec3b color2);
	void findElements(Mat& img);
	void showElements(Mat& img, Vec2d m1, Vec2d m3, Vec2d m7);
	void removeInvalidElements(Mat& img);
	void rankFilter(Mat& img, int windowSize, int rank);
	void computeMoments(Mat& img);
	void enhance(cv::Mat &img, double margin);
private:
	vector<Element> elements;
	void fillElement(Mat& img, Element& el);
	bool isSimilarColor(Vec3b& which, Vec3b range1, Vec3b range2);
	bool isInvalid(const Element& elem);
	vector<int> calcHistogram(Mat& img);
};

