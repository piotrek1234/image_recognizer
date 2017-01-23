#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>
#include "ImageRecognizer.h"

using namespace std;
using namespace cv;

int main(int, char *[]) {

	//graniczne momenty
	Vec2d m1(0.18, 0.24);
	Vec2d m3(0.00055, 0.0016);
	Vec2d m7(0.0072, 0.013);
	//graniczne kolory:
	Vec3b col1(26, 70, 25);
	Vec3b col2(100, 255, 255);

	for (int i = 1; i < 6; ++i)
	{
		cout << "Przerabiam " << i << ".jpg" << endl;
		Mat temp = imread(std::to_string(i) + ".jpg");
		Mat oryginal = temp.clone();
		
		//imshow("przed", skoda1);
		ImageRecognizer ir;
		ir.enhance(temp, 2.0);
		ir.filterColor(temp, col1, col2);
		ir.findElements(temp);
		//ir.rankFilter(temp, 3, 4);
		ir.removeInvalidElements(temp);
		ir.computeMoments(temp);
		ir.showElements(oryginal, m1, m3, m7);
		imshow(std::to_string(i), oryginal);
		waitKey(1);
	}
	
	// suwaki doboru koloru
	/*namedWindow("po");
	namedWindow("suwaki");
	int h1 = 0, h2 = 255, s1 = 0, s2 = 255, v1 = 0, v2 = 255;
	Mat skoda1 = imread("1.jpg");
	ImageRecognizer ir;
	while (1)
	{
		Mat tmp = skoda1.clone();
		ir.enhance(tmp, 2.0);
		createTrackbar("h1", "suwaki", &h1, 255);
		createTrackbar("s1", "suwaki", &s1, 255);
		createTrackbar("v1", "suwaki", &v1, 255);
		createTrackbar("h2", "suwaki", &h2, 255);
		createTrackbar("s2", "suwaki", &s2, 255);
		createTrackbar("v2", "suwaki", &v2, 255);
		ir.filterColor(tmp, Vec3b(h1, s1, v1), Vec3b(h2, s2, v2));
		imshow("po", tmp);
		cv::waitKey(1);

	}*/

	waitKey(-1);

    return 0;
}
