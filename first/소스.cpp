#define _CRT_SECURE_NO_WARNINGS
#include "opencv2\opencv.hpp"
#include <iostream>
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
		memset(rtn[n], nlnit_val, sizeof(unsigned char)*n_width);
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
void gausian_filter(unsigned char **in, unsigned char** Out, int nHeight, int nWidth, double sigma)
{
	unsigned char** pad = padding(in, nHeight, nWidth, 3);
	int pad_size = 1;
	double gaussian[3][3];

	double result = 0;

	double stdv = sigma;

	double r, s = 2.0 * stdv * stdv;

	double sum = 0.0;

	for (int x = -1; x <= 1; x++)

	{
		for (int y = -1; y <= 1; y++)

		{
			r = (x * x + y * y);

			gaussian[x + 1][y + 1] = (exp(-(r) / s)) / sqrt(3.14 * s);

			sum += gaussian[x + 1][y + 1];
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
void main()
{
	Mat Img_Input = imread("input.jpg", 1);
	unsigned char** img_array_gray = mem_alloc2_d(Img_Input.rows, Img_Input.cols, 0);//가로 세로
	Mat Img_Gray(Size(Img_Input.cols, Img_Input.rows), 0);

	for (int h = 0; h < Img_Input.rows; h++)
	{
		for (int w = 0; w < Img_Input.cols; w++)
		{
			Vec3b RGB = Img_Input.at<Vec3b>(h, w); //unsigned char 3°³
			img_array_gray[h][w] = RGB[2] * 0.299 + RGB[1] * 0.587 + RGB[0] * 0.114;
		}
	}
	unsigned char** Out = mem_alloc2_d(Img_Input.rows, Img_Input.cols, 0);
	unsigned char** Out2 = mem_alloc2_d(Img_Input.rows, Img_Input.cols, 0);

	gausian_filter(img_array_gray, Out, Img_Input.rows, Img_Input.cols, 0.5);//가우시안이 어떤 효과를 가져오는지를 알아봐야겠다.
	gausian_filter(img_array_gray, Out2, Img_Input.rows, Img_Input.cols, 5);
	for (int h = 0; h < Img_Input.rows; h++)
	{
		for (int w = 0; w < Img_Input.cols; w++)
		{
			//if (Out[h][w] - Out2[h][w] < 0)//이러면 0으로 만들어야 됬던거 같은데 샤픈에서
			//{
			//	if (Out[h][w] - Out2[h][w] < -1)
			//	{
			//		Img_Gray.at<uchar>(h, w) = (Out[h][w] - Out2[h][w]);
			//	}
			//	else {
			//		Img_Gray.at<uchar>(h, w) = (Out[h][w] - Out2[h][w]);

			//	}
			//}
			if (Out[h][w] - Out2[h][w] > 255)
			{
				Img_Gray.at<uchar>(h, w) = 255;

			}
			else {
				Img_Gray.at<uchar>(h, w) = (Out[h][w] - Out2[h][w]);

			}

		}
	}
	Mat Img_Binary(Size(Img_Input.cols, Img_Input.rows), 0);

	for (int h = 0; h < Img_Input.rows; h++)//이진화
	{
		for (int w = 0; w < Img_Input.cols; w++)
		{
			if (Img_Gray.at<uchar>(h, w) > 127)
			{
				Img_Binary.at<uchar>(h, w) = 0;

			}
			else {
				Img_Binary.at<uchar>(h, w) = 255;
			}

		}
	}
	imshow("Ori", Img_Input);
	imshow("Gray", Img_Gray);
	imshow("Binary", Img_Binary);


	//FILE *fp_in, *fp_out;
	//fopen_s(&fp_in, "lena256.raw", "rb");
	////fp_in = fopen("lena256.raw", "rb"); // 난 이렇게 했는데 여기서는 이렇게 했네
	//unsigned char** Y = (unsigned char**)malloc(sizeof(unsigned char*) * HEIGHT);

	//for (int h = 0; h < HEIGHT; h++)
	//{
	//	Y[h] = (unsigned char*)malloc(sizeof(unsigned char) * WIDTH);
	//	fread(Y[h], sizeof(unsigned char), WIDTH, fp_in);
	//}
	//Mat Ori_Img = Mat(HEIGHT, WIDTH, CV_8UC1);

	//for (int h = 0; h < HEIGHT; h++)

	//{
	//	for (int w = 0; w < WIDTH; w++)

	//	{
	//		Ori_Img.at<unsigned char>(h, w) = Y[h][w];
	//	}
	//}
	//imshow("Ori", Ori_Img);


	//unsigned char** temp;

	//temp = (unsigned char**)malloc(sizeof(unsigned char*) * HEIGHT);

	//for (int h = 0; h < HEIGHT; h++)
	//{
	//	temp[h] = (unsigned char*)malloc(sizeof(unsigned char) * WIDTH);
	//	fread(temp[h], sizeof(unsigned char), WIDTH, fp_in);
	//}
	//for (int i = 0; i < HEIGHT; i++)
	//{
	//	for (int j = 0; j < WIDTH; j++)
	//	{
	//		temp[i][j] = 0;
	//	}
	//}
	//Mat Img_squre = Mat(HEIGHT, WIDTH, CV_8UC1);

	//for (int h = 0; h < HEIGHT; h++)

	//{
	//	for (int w = 0; w < WIDTH; w++)

	//	{
	//		Img_squre.at<unsigned char>(h, w) = temp[h][w];
	//	}
	//}
	//imshow("Img_squre", Img_squre);

	//for (int i = 0; i < HEIGHT; i++)
	//{
	//	for (int j = 0; j < WIDTH; j++)
	//	{
	//		temp[i][j] = 255;
	//	}
	//}
	//for (int h = 0; h < HEIGHT; h++)

	//{
	//	for (int w = 0; w < WIDTH; w++)

	//	{
	//		Img_squre.at<unsigned char>(h, w) = temp[h][w];
	//	}
	//}
	//imshow("Img_squre_white", Img_squre);

	//for (int i = 0; i < HEIGHT; i++)
	//{
	//	for (int j = 0; j < WIDTH; j++)
	//	{
	//		/*if ((i<60 && i>HEIGHT > 60 )&& (j<WIDTH - 60 && j> 60))*/
	//		if ((i > 50 && i < HEIGHT - 50 && j > 50 && j < WIDTH - 50))

	//		{
	//			temp[i][j] = 255;
	//		}
	//		else
	//		{
	//			temp[i][j] = 0;
	//		}
	//	}
	//}
	//for (int h = 0; h < HEIGHT; h++)

	//{
	//	for (int w = 0; w < WIDTH; w++)

	//	{
	//		Img_squre.at<unsigned char>(h, w) = temp[h][w];
	//	}
	//}
	//imshow("Img_squre_white of squre", Img_squre);


	//for (int i = 0; i < HEIGHT; i++)
	//{
	//	for (int j = 0; j < WIDTH; j++)
	//	{
	//		/*if ((i<60 && i>HEIGHT > 60 )&& (j<WIDTH - 60 && j> 60))*/
	//		if (((double)HEIGHT / 2 - i) * ((double)HEIGHT / 2 - i) + ((double)WIDTH / 2 - j) * ((double)WIDTH / 2 - j)
	//			<= 100 * 100)


	//		{
	//			temp[i][j] = 255;
	//		}
	//		else
	//		{
	//			temp[i][j] = 0;
	//		}
	//	}
	//}
	//for (int h = 0; h < HEIGHT; h++)

	//{
	//	for (int w = 0; w < WIDTH; w++)

	//	{
	//		Img_squre.at<unsigned char>(h, w) = temp[h][w];
	//	}
	//}
	//imshow("Circle Image", Img_squre);
	//double dScaleRate = 0;

	//string output = "oh"; //결과를 raw 이미지로 출력합니다.
	//output.append(".raw");
	//FILE* outfile = fopen(output.c_str(), "w+b");
	//for (int i = 0; i < HEIGHT; i++)
	//{
	//	fwrite(temp[i], sizeof(char), WIDTH, outfile);
	//}
	//fclose(outfile);
	waitKey();
}
