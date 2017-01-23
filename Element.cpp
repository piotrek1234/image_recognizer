#include "Element.h"


Element::Element(Vec3b color, int x, int y)
{
	color_ = color;
	min_x = INT_MAX;
	min_y = INT_MAX;
	max_x = 0;
	max_y = 0;
	isValid = true;
	start_x = x;
	start_y = y;
}


Element::~Element()
{
}

void Element::compute(Mat& img)
{
	double m00 = mpq(img, 0, 0);
	double x_ = mpq(img, 1, 0) / m00;
	double y_ = mpq(img, 0, 1) / m00;

	double M11 = Mpq(img, 1, 1, x_, y_);
	double M02 = Mpq(img, 0, 2, x_, y_);
	double M20 = Mpq(img, 2, 0, x_, y_);
	double M03 = Mpq(img, 0, 3, x_, y_);
	double M30 = Mpq(img, 3, 0, x_, y_);
	double M12 = Mpq(img, 1, 2, x_, y_);
	double M21 = Mpq(img, 2, 1, x_, y_);

	M[0] = (M20 + M02) / pow(m00, 2);
	//M[1] = (pow(M20+M02, 2)+4*pow(M11, 2)) / pow(m00, 4);
	M[2] = (pow(M30 + 3 * M12, 2) + pow(3 * M21 - M03, 2)) / pow(m00, 5);
	//M[3] = (pow(M30 + M12, 2) + pow(M21 - M03, 2)) / pow(m00, 5);
	//M[4] = ((M30 - 3 * M12)*(M30 + M12)*(pow(M30 + M12, 2) - 3 * pow(M21 + M03, 2)) + (3*M21-M03)*(M21+M03)*(3 * pow(M30+M12, 2) - pow(M21+M03, 2))) / pow(m00, 10);
	//M[5] = ((M20-M02)*(pow(M30+M12, 2)-pow(M21+M03, 2)) + 4 * M11*(M30+M12)*(M21+M03)) / pow(m00, 7);
	M[6] = (M20*M02-pow(M11, 2)) / pow(m00, 4);
	//M[7] = (M30*M12+M21*M03-pow(M12, 2)-pow(M21, 2)) / pow(m00, 5);
	//M[8] = (M20*(M21*M03 - pow(M12, 2)) + M02*(M03*M12 - pow(M21, 2)) - M11*(M30*M03 - M21*M12)) / pow(m00, 7);
	//M[9] = (pow(M30*M03 - M12*M21, 2) - 4 * (M30*M12 - pow(M21, 2))*(M03*M21 - M12)) / pow(m00, 10);

	//writeM(img);	//wypisywanie niezmienników na elemencie
}

void Element::writeM(Mat& img)
{
	//wypisywanie niezmienników na elemencie
	for (int i = 0; i < 10; ++i)
	{
		std::string text = "M"+std::to_string(i+1)+": "+std::to_string(M[i]);
		putText(img, text, Point(start_x, start_y + i * 10), 1, 1, Scalar(255, 255, 255));
	}
		
}

int Element::mpq(Mat& img, int p, int q)
{
	int ret = 0;
	cv::Mat_<cv::Vec3b> _I = img;
	for (int x = 0; x < _I.cols; ++x)
	{
		for (int y = 0; y < _I.rows; ++y)
		{
			if (_I(y, x) == color_)
				ret += pow(x, p)*pow(y, q);
		}
	}
	return ret;
}

double Element::Mpq(Mat& img, int p, int q, int x_, int y_)
{
	int ret = 0;
	cv::Mat_<cv::Vec3b> _I = img;
	for (int x = 0; x < _I.cols; ++x)
	{
		for (int y = 0; y < _I.rows; ++y)
		{
			if (_I(y, x) == color_)
				ret += pow(x-x_, p)*pow(y-y_, q);
		}
	}
	return ret;
}