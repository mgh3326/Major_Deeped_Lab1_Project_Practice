#define _CRT_SECURE_NO_WARNINGS
#include "opencv2\opencv.hpp"

#include <iostream>
#include <cstdlib>
#include <vector>
#include <string>
using namespace cv;

using namespace std;
#define HEIGHT 256
#define WIDTH 256
//RGB to GRAY
//Defferecnes of Gaussian
//binaray 
unsigned char** mem_alloc2_d(int n_height, int n_width, unsigned char nlnit_val)
{
	unsigned char** rtn = new unsigned char*[n_height];
	for (int n = 0; n < n_height; n++)
	{
		rtn[n] = new unsigned char[n_width];
		memset(rtn[n], nlnit_val, sizeof(unsigned char) * n_width);
	}
	return rtn;
}

unsigned char** padding(unsigned char** in, int n_height, int n_width, int n_filter_size)
{
	int n_pad_size = (int)(n_filter_size / 2);
	unsigned char** pad = mem_alloc2_d(n_height + 2 * n_pad_size, n_width + 2 * n_pad_size, 0);

	for (int h = 0; h < n_height; h++)
	{
		for (int w = 0; w < n_width; w++)
		{
			pad[h + n_pad_size][w + n_pad_size] = in[h][w];
		}
	}

	for (int h = 0; h < n_pad_size; h++)
	{
		for (int w = 0; w < n_width; w++)
		{
			pad[h][w + n_pad_size] = in[0][w];
			pad[h + (n_height - 1)][w + n_pad_size] = in[n_height - 1][w];
		}
	}

	for (int h = 0; h < n_height; h++)
	{
		for (int w = 0; w < n_pad_size; w++)
		{
			pad[h + n_pad_size][w] = in[h][0];
			pad[h + n_pad_size][w + (n_width - 1)] = in[h][n_width - 1];
		}
	}

	for (int h = 0; h < n_pad_size; h++)
	{
		for (int w = 0; w < n_pad_size; w++)
		{
			pad[h][w] = in[0][0];
			pad[h + (n_height - 1)][w] = in[n_height - 1][0];
			pad[h][w + (n_width - 1)] = in[0][n_width - 1];
			pad[h + (n_height - 1)][w + (n_width - 1)] = in[n_height - 1][n_width - 1];
		}
	}

	return pad;
}

void gausian_filter(unsigned char** in, unsigned char** Out, int nHeight, int nWidth,double sigma, int n_filter_size = 3)
{
	if (n_filter_size % 2 == 0)//필터는 꼭 홀수 이어야 하므로 홀수로 만들어주어야 합니다.
	{
		n_filter_size++;
	}
	unsigned char** pad = padding(in, nHeight, nWidth, n_filter_size);
	int pad_size = (int)(n_filter_size / 2);
	double **gaussian = new double*[n_filter_size];//histogram을 받을 배열입니다.
	for (int n = 0; n < n_filter_size; n++)
	{
		gaussian[n] = new double[n_filter_size];
		memset(gaussian[n], 0, sizeof(n_filter_size) * n_filter_size);
	}

	double result = 0;

	double stdv = sigma;

	double r, s = 2.0 * stdv * stdv;

	double sum = 0.0;

	for (int x = -pad_size; x <= pad_size; x++)

	{
		for (int y = -pad_size; y <= pad_size; y++)

		{
			r = (x * x + y * y);

			gaussian[x + pad_size][y + pad_size] = (exp(-(r) / s)) / sqrt(3.14 * s);

			sum += gaussian[x + pad_size][y + pad_size];
		}
	}
	for (int i = 0; i < 3; ++i) // Loop to normalize the kernel

		for (int j = 0; j < 3; ++j)

			gaussian[i][j] /= sum;

	for (int h = 0; h < nHeight; h++)

	{
		for (int w = 0; w < nWidth; w++)

		{
			result = 0;

			for (int i = -pad_size; i <= pad_size; i++)

				for (int j = -pad_size; j <= pad_size; j++)

				{
					result += pad[h + i + pad_size][w + j + pad_size] * gaussian[i + pad_size][j + pad_size];
				}

			Out[h][w] = result;
		}
	}
}

Mat GrayToMat(unsigned char** Img, int nHeight, int nWidth)
{
	Mat rtn = Mat(nHeight, nWidth, CV_8UC1);

	for (int h = 0; h < nHeight; h++)
	{
		for (int w = 0; w < nWidth; w++)
		{
			rtn.at<unsigned char>(h, w) = Img[h][w];
		}
	}

	return rtn;
}

unsigned char** get_gray(Mat image_origin)
//""" image 객체를 인자로 받아서 Gray-scale 을 적용한 2차원 이미지 객체로 반환합니다.
//이 때 인자로 입력되는 이미지는 BGR 컬러 이미지여야 합니다.
//
//	:param image_origin : OpenCV 의 BGR image 객체(3 dimension)
//	: return : gray - scale 이 적용된 image 객체(2 dimension)
//	"""
{
	unsigned char** img_array_gray = mem_alloc2_d(image_origin.rows, image_origin.cols, 0); //가로 세로
	Mat image_gray(Size(image_origin.cols, image_origin.rows), CV_8UC1);

	for (int h = 0; h < image_origin.rows; h++)
	{
		for (int w = 0; w < image_origin.cols; w++)
		{
			Vec3b RGB = image_origin.at<Vec3b>(h, w); //unsigned char 3°³
			img_array_gray[h][w] = RGB[2] * 0.299 + RGB[1] * 0.587 + RGB[0] * 0.114;
		}
	}
	return img_array_gray;

}
Mat get_gradient(Mat image_gray)
//""" 이미지에 Dilation 과 Erosion 을 적용하여 그 차이를 이용해 윤곽선을 추출합니다.
//이 때 인자로 입력되는 이미지는 Gray scale 이 적용된 2차원 이미지여야 합니다.

{
	/*imshow("image_gray", image_gray);*/
	cv::Mat kernel;

	kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size2d(2, 2));
	cv::Mat image_gradient;

	cv::morphologyEx(image_gray, image_gradient, cv::MORPH_GRADIENT, kernel);
	return image_gradient;
}
Mat get_threshold(Mat image_gradient)
{
	Mat image_threshold;
	cv::adaptiveThreshold(image_gradient, image_threshold, 255, ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY_INV, 5, 8);
	return image_threshold;

}
Mat remove_long_line(Mat image_threshold)
//""" 이미지에서 직선을 찾아서 삭제합니다.
//글자 경계를 찾을 때 방해가 되는 직선을 찾아서 삭제합니다.
//이 때 인자로 입력되는 이미지 2 차원(2 dimension) 흑백(Binary) 이미지여야 합니다.
//직선을 삭제할 때는 해당 라인을 검정색으로 그려 덮어 씌웁니다.
{
	vector<Vec4i>  lines;
	HoughLinesP(image_threshold, lines, 1, 3.141592 / 180, 180, 100, 120);
	for (size_t i = 0; i < lines.size(); i++)
	{
		Vec4i l = lines[i];
		/*line(image_threshold, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0, 0, 0), 3, CV_AA);*/
		line(image_threshold, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0, 0, 0), 2);
	}
	/*imshow("remove_line", image_threshold);
*/
	return image_threshold;
}
Mat   get_closing(Mat image_threshold)
{
	Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size2d(20, 19));
	Mat image_close;
	morphologyEx(image_threshold, image_close, MORPH_CLOSE, kernel);
	return image_close;
	/*imshow("image_close", image_close);*/

}
Mat get_contours(Mat blur_out,Mat image_origin)
{
	vector<vector<cv::Point> > contours;
	vector<cv::Point> points;
	/*cv::findContours(image_gray, contours, 0, 2);
*/
	findContours(blur_out, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

	// Approximate contours to polygons + get bounding rects 
	vector<vector<cv::Point> > contours_poly(contours.size());
	vector<cv::Rect> boundRect(contours.size());
	vector<cv::Point2f>center(contours.size());
	vector<float>radius(contours.size());

	for (int i = 0; i < contours.size(); i++)
	{
		approxPolyDP(cv::Mat(contours[i]), contours_poly[i], 3, true);
		boundRect[i] = cv::boundingRect(cv::Mat(contours_poly[i]));
	}

	//Draw bounding rect
	int x = 1;
	cv::Mat drawing(image_origin);
	for (int i = 0; i < contours.size(); i++)
	{
		std::stringstream s;
		s << x;
		std::string str1 = s.str();
		cv::Scalar color = cv::Scalar(0, 255, 0);

		if (boundRect[i].height > 8 && boundRect[i].width > 8)
		{
			rectangle(drawing, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0);
			cv::Rect region_of_interest = cv::Rect(boundRect[i].x, boundRect[i].y, boundRect[i].width, boundRect[i].height);
			/*cv::Mat imageroi = binary(region_of_interest);

			std::stringstream ss;
			ss << i;
			std::string str = ss.str();
			const char *cstr = str.c_str();

			cv::imwrite(str1 + ".jpg", imageroi);

			cvNamedWindow(cstr, CV_WINDOW_AUTOSIZE);
			imshow(str, imageroi);*/
			x++;
		}
	}
	return image_origin;
}
Mat  get_step_compare_image(string image_path)// 이미지 프로세싱 전 단계의 중간 결과물을 하나로 병합하여 반환합니다.
{
	Mat image_origin = cv::imread(image_path);
	if (!image_origin.data)
	{
		std::cout << "Image file not found\n";
		exit(1);
	}
	pyrUp(image_origin, image_origin);//pyrUP 함수를 실행하면 이미지가 커지는데 이로써 무슨 장점이 있는지는 몰라도 결과물의 차이가 있음

	/*imshow("image_origin", image_origin);*/

	Mat image_gray = GrayToMat(get_gray(image_origin), image_origin.rows, image_origin.cols);
	// threshold image
	Mat image_gradient = get_gradient(image_gray);
	//imshow("image_gradient", image_gradient);
	Mat image_threshold = get_threshold(image_gradient);
	Mat image_line_removed = remove_long_line(image_threshold);
	Mat image_close = get_closing(image_line_removed);
	
	Mat blur_out(image_close);

	image_origin = get_contours(blur_out, image_origin);

	return image_origin;
	

}
void main()
{

	string image_path = "input.jpg";
	Mat image_origin = get_step_compare_image(image_path);

	imshow("Box", image_origin);


	/*imshow("Box", image_origin);*/
	waitKey(0);


	//unsigned char** Out2 = mem_alloc2_d(Img_Input.rows, Img_Input.cols, 0);

	//gausian_filter(img_array_gray, Out, Img_Input.rows, Img_Input.cols, 0.5,5); //가우시안이 어떤 효과를 가져오는지를 알아봐야겠다.
	//gausian_filter(img_array_gray, Out2, Img_Input.rows, Img_Input.cols, 5,5);
	//for (int h = 0; h < Img_Input.rows; h++)
	//{
	//	for (int w = 0; w < Img_Input.cols; w++)
	//	{
	//		//if (Out[h][w] - Out2[h][w] < 0)//이러면 0으로 만들어야 됬던거 같은데 샤픈에서
	//		//{
	//		//	if (Out[h][w] - Out2[h][w] < -1)
	//		//	{
	//		//		Img_Gray.at<uchar>(h, w) = (Out[h][w] - Out2[h][w]);
	//		//	}
	//		//	else {
	//		//		Img_Gray.at<uchar>(h, w) = (Out[h][w] - Out2[h][w]);

	//		//	}
	//		//}
	//		if (Out[h][w] - Out2[h][w] > 255)
	//		{
	//			Img_Gray.at<uchar>(h, w) = 255;
	//		}
	//		else
	//		{
	//			Img_Gray.at<uchar>(h, w) = (Out[h][w] - Out2[h][w]);
	//		}
	//	}
	//}
	//Mat Img_Binary(Size(Img_Input.cols, Img_Input.rows), 0);

	//for (int h = 0; h < Img_Input.rows; h++) //이진화
	//{
	//	for (int w = 0; w < Img_Input.cols; w++)
	//	{
	//		if (Img_Gray.at<uchar>(h, w) > 127)
	//		{
	//			Img_Binary.at<uchar>(h, w) = 0;
	//		}
	//		else
	//		{
	//			Img_Binary.at<uchar>(h, w) = 255;
	//		}
	//	}
	//}
	//imshow("Ori", Img_Input);
	//imshow("Gray", Img_Gray);
	//imshow("Binary", Img_Binary);


	//waitKey();
}
