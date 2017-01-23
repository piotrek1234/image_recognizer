#pragma once

#include "opencv2/imgproc.hpp"
#include <string>

using namespace cv;

class Element
{
public:
	Element(Vec3b color, int x, int y);
	~Element();
	int min_x;
	int max_x;
	int min_y;
	int max_y;
	int start_x;
	int start_y;
	bool isValid;
	Vec3b color_;
	void compute(Mat& img);
	void writeM(Mat& img);
	double getM(int i) { return M[i]; }
private:
	int mpq(Mat& img, int p, int q);
	double Mpq(Mat& img, int p, int q, int x_, int y_);
	double M[10];
};

