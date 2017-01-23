#include "ImageRecognizer.h"


ImageRecognizer::ImageRecognizer()
{
	srand(time(NULL));
}


ImageRecognizer::~ImageRecognizer()
{
}

void ImageRecognizer::showElements(Mat& img, Vec2d m1, Vec2d m3, Vec2d m7)
{
	//rysowanie ramki wokó³ elementu, jeœli niezmienniki mieszcz¹ siê w podanych granicach
	for (int i = 0; i < elements.size(); ++i)
	{
		Element el = elements.at(i);
		if (m1[0] <= el.getM(0) && el.getM(0) <= m1[1])
			if (m3[0] <= el.getM(2) && el.getM(2) <= m3[1])
				if (m7[0] <= el.getM(6) && el.getM(6) <= m7[1])
					rectangle(img, Point(el.min_x, el.min_y), Point(el.max_x, el.max_y), Scalar(255, 255, 255));
	}
}

bool ImageRecognizer::isSimilarColor(Vec3b& which, Vec3b range1, Vec3b range2)
{
	if (which[0] >= range1[0] && which[0] <= range2[0])
		if (which[1] >= range1[1] && which[1] <= range2[1])
			if (which[2] >= range1[2] && which[2] <= range2[2])
				return true;
	return false;
}

void ImageRecognizer::filterColor(Mat& img, Vec3b color1, Vec3b color2)
{
	Mat hsv;
	cvtColor(img, hsv, CV_BGR2HSV);

	Mat_<Vec3b> temp = hsv;
	Mat_<Vec3b> _I = img;

	for (int y = 0; y < _I.rows; ++y)
		for (int x = 0; x < _I.cols; ++x)
		{
			if (isSimilarColor(temp(y, x), color1, color2))
				_I(y, x) = Vec3b(255, 255, 255);
			else
				_I(y, x) = Vec3b(0, 0, 0);
		}
	img = _I;
}

vector<int> ImageRecognizer::calcHistogram(Mat& img)
{
	vector<int> histogram(256);

	cv::Mat_<cv::Vec3b> _I = img;
	for (int y = 0; y < _I.rows; ++y)
		for (int x = 0; x < _I.cols; ++x)
			histogram.at((_I(y, x)[0] + _I(y, x)[1] + _I(y, x)[2]) / 3)++;

	//przeskalowanie: max = 255
	int max = 0;
	for (int elem : histogram)
		if (elem > max)
			max = elem;

	for (int& elem : histogram)
		elem = elem * 255 / max;

	return histogram;
}

void ImageRecognizer::enhance(cv::Mat &img, double margin)
{
	//usuwam margin% pikseli z ka¿dej strony histogramu
	//piksele wyjœciowe bêd¹ mia³y jasnoœæ Iwy = Iwe*a+b
	vector<int> hist = calcHistogram(img);
	vector<double> sumHist(256);	//przechowuje liczbê pikseli o danej lub mniejszej jasnoœci

	sumHist[0] = hist[0];
	for (int i = 1; i < 256; ++i)
	{
		sumHist[i] = hist[i] + sumHist[i - 1];
	}

	for (double& elem : sumHist){
		elem = elem * 100 / sumHist[255];
	}

	//minimalna i maksymalna wartoœæ docelowego obrazu
	int min_i = 0, max_i = 255;
	while (sumHist[min_i] < margin/2.0)
		min_i++;

	while (sumHist[max_i] >= (max_i - margin/2.0))
		max_i--;

	double a = 255 / (double)(max_i - min_i);
	double b = -min_i * a;

	cv::Mat_<cv::Vec3b> _I = img;
	for (int y = 0; y < _I.rows; ++y)
		for (int x = 0; x < _I.cols; ++x)
		{
			_I(y, x)[0] = min(255, max(0, int(_I(y, x)[0] * a + b)));
			_I(y, x)[1] = min(255, max(0, int(_I(y, x)[1] * a + b)));
			_I(y, x)[2] = min(255, max(0, int(_I(y, x)[2] * a + b)));
		}
	img = _I;
}

void ImageRecognizer::findElements(Mat& img)
{
	cv::Mat_<cv::Vec3b> _I = img;
	for (int y = 0; y < _I.rows; ++y)
	{
		for (int x = 0; x < _I.cols; ++x)
		{
			if (_I(y, x) == Vec3b(255, 255, 255))
			{
				_I = img;
				Vec3b color = Vec3b(rand() % 256, rand() % 256, rand() % 256);
				Element el(color, x, y);
				fillElement(img, el);
				elements.push_back(el);

			}
		}
	}
}

void ImageRecognizer::removeInvalidElements(Mat& img)
{
	for (int i = 0; i < elements.size(); ++i)
	{
		float width = elements.at(i).max_x - elements.at(i).min_x;
		float height = elements.at(i).max_y - elements.at(i).min_y;

		if (width / height > 1.2 || height / width > 1.2 || width < 25 || height < 25)
		{
			//oznacz jako z³y, wype³nij na czarno
			elements.at(i).isValid = false;
			elements.at(i).color_ = Vec3b(0, 0, 0);
			fillElement(img, elements.at(i));
		}
		else
		{
			elements.at(i).isValid = true;
		}

	}
	//kasuje niepoprawne elementy
	elements.erase(remove_if(elements.begin(), elements.end(), [](const Element& e) { return !e.isValid; }), elements.end());
}

void ImageRecognizer::rankFilter(Mat& img, int windowSize, int rank)
{
	cv::Mat_<cv::Vec3b> _I = img, ret;
	ret = img.clone();
	for (int x = windowSize / 2; x < _I.cols - windowSize / 2; ++x)
	{
		for (int y = windowSize / 2; y < _I.rows - windowSize / 2; ++y)
		{
			vector<int> pixels;
			for (int wx = -windowSize/2; wx <= windowSize/2; ++wx)
			{
				for (int wy = -windowSize/2; wy <= windowSize/2; ++wy)
				{
					pixels.push_back(_I(y+wy, x+wx)[0]);	//dla czarno-bia³ego _I[0] powinno byæ takie samo jak œrednia z 3 kana³ów
				}
			}
			sort(pixels.begin(), pixels.end());
			ret(y, x) = Vec3b(pixels.at(rank), pixels.at(rank), pixels.at(rank));
		}
	}
	img = ret;
}

void ImageRecognizer::fillElement(Mat& img, Element& el)
{
	//przy rekurencji przepe³nia³ siê stos, zastosujê kolejkê
	Vec3b color = el.color_;
	queue< pair<int, int> > q;
	cv::Mat_<cv::Vec3b> _I = img;
	Vec3b start_col = _I(el.start_y, el.start_x);

	q.push(pair<int, int>(el.start_x, el.start_y));

	while (q.size() > 0)
	{
		pair<int, int> pos = q.front();
		q.pop();

		int x = pos.first, y = pos.second;
		if (_I(y, x) == start_col)
		{
			_I(y, x) = color;

			if (x < el.min_x)
				el.min_x = x;
			if (x > el.max_x)
				el.max_x = x;
			if (y < el.min_y)
				el.min_y = y;
			if (y > el.max_y)
				el.max_y = y;

			if (x + 1 < _I.cols)
				q.push(pair<int, int>(x + 1, y));
			if (x-1 >= 0)
				q.push(pair<int, int>(x - 1, y));
			if (y + 1 < _I.rows)
				q.push(pair<int, int>(x, y + 1));
			if (y - 1 >= 0)
				q.push(pair<int, int>(x, y - 1));

		}
	}
	img = _I;
}

void ImageRecognizer::computeMoments(Mat& img)
{
	for (int i = 0; i < elements.size(); ++i)
	{
		elements.at(i).compute(img);
	}
}
