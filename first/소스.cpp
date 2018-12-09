#include <opencv2/opencv.hpp>  //  Include 'OpenCV' library.
#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>
#include <algorithm>
#include <iterator>
#include "dirent.h"
using namespace cv;
using namespace std;
int smallcount = 0;
Mat CannyEdge(Mat img_gray, int th_high, int th_low);

Mat FindPlate(String path);

int main()
{
	DIR *dir;
	struct dirent *ent;
	string folder_name = "test/";
	int total = 0;
	int success = 0;
	if ((dir = opendir(folder_name.c_str())) != NULL) {
		/* print all the files and directories within directory */
		while ((ent = readdir(dir)) != NULL) {
			{
				if (ent->d_type != 16384)//폴더 경로
				{
					printf("%s\n", ent->d_name);

					Mat Exac_Img;
					string FileName = folder_name+ ent->d_name;
					// Exac_Img = 번호판추출하는함수();
					Exac_Img = FindPlate(FileName);
					total++;
					if (Exac_Img.size().area() == 0)
					{
						cout << FileName << "plate detect false" << endl;
						cout << "False" << endl;
					}
					else {
						string resultFileName = "result/";
						resultFileName += ent->d_name;

						imwrite(resultFileName, Exac_Img);
						cout << "True" << endl;
						success++;
					}
				}

			}
		}
		closedir(dir);
	}
	else {
		/* could not open directory */
		perror("");
		return EXIT_FAILURE;
	}
	cout << "Total : " << total << endl;
	cout << "Success : " << success << endl;
	cout << "Small : " << smallcount << endl;
	return 0;
}
Mat FindPlate(String path)
{

	Mat out;
	Mat image, image2, image3, drawing;  //  Make images.
	Rect rect, temp_rect;  //  Make temporary rectangles.
	vector<vector<Point> > contours;  //  Vectors for 'findContours' function.
	vector<Vec4i> hierarchy;

	double ratio, delta_x, delta_y, gradient;  //  Variables for 'Snake' algorithm.
	int number_select = -1, plate_select = -1, plate_width, count, friend_count = 0, number_count = 0, plate_count = 0;

	image = imread(path);  //  Load an image file.   // 4가 조금 안됨

	if ( image.cols < 500)
	{
		cout << "Image is too small" << endl;
		out.resize(0);
		smallcount++;
		return out;
	}
	if (image.rows > 1000)
	{
		resize(image, image, Size(), 1000.0 / (double)image.rows, 1000.0 / (double)image.rows);
	}

	//imshow("Original", image);

	//waitKey(0);

	image.copyTo(image2);  //  Copy to temporary images.
	image.copyTo(image3);  //  'image2' - to preprocessing, 'image3' - to 'Snake' algorithm.

	cvtColor(image2, image2, CV_BGR2GRAY);  //  Convert to gray image.


	///////////////////////////////////////


	vector<vector<Point> > tempContours;  //  Vectors for 'findContours' function.
	vector<Vec4i> tempHierarchy;
	Mat tempImage, drawing2;
	image2.copyTo(tempImage);
	drawing2 = Mat::zeros(tempImage.size(), CV_8UC3);
	tempImage = CannyEdge(tempImage, 300, 100);
	//Canny(tempImage, tempImage, 100, 300, 3);  //  Getting edges by Canny algorithm.

	findContours(tempImage, tempContours, tempHierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point());
	vector<vector<Point> > tempContours_poly(tempContours.size());
	vector<Rect> tempBoundRect(tempContours.size());
	vector<Rect> tempBoundRect2(tempContours.size());

	for (int i = 0; i < tempContours.size(); i++) {
		approxPolyDP(Mat(tempContours[i]), tempContours_poly[i], 1, true);
		tempBoundRect[i] = boundingRect(Mat(tempContours_poly[i]));
	}

	for (int i = 0; i < tempContours.size(); i++) {
		ratio = (double)tempBoundRect[i].height / (double)tempBoundRect[i].width;

		//  Filtering rectangles height/width ratio, and size.

		//if ((ratio <= 4) && (ratio >= 0.5) && (tempBoundRect[i].area() >= 100)) {
		drawContours(drawing2, tempContours, i, Scalar(0, 255, 255), 1, 8, tempHierarchy, 0, Point());
		rectangle(drawing2, tempBoundRect[i].tl(), tempBoundRect[i].br(), Scalar(255, 0, 0), 1, 8, 0);


		//  Include only suitable rectangles.
		tempBoundRect2[number_count++] = tempBoundRect[i];
		//}
	}
	//imshow("temp", drawing2);
	if (tempBoundRect2.size() > 2800)
		blur(image2, image2, Size(5, 5));
	else
		blur(image2, image2, Size(3, 3));
	cout << tempBoundRect2.size() << " < con size" << endl;
	//imshow("Original->Gray", image2);
	//waitKey(0);

	//////////////////////////////////////


	number_count = 0;
	//Canny(image2, image2, 300, 300, 3);  //  Getting edges by Canny algorithm.
	image2 = CannyEdge(image2, 300, 100);
	//imshow("Original->Gray->Canny", image2);
	//waitKey(0);


	//  Finding contours.

	findContours(image2, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point());
	vector<vector<Point> > contours_poly(contours.size());
	vector<Rect> boundRect(contours.size());
	vector<Rect> boundRect2(contours.size());
	vector<Rect> boundRect3(contours.size());

	//  Bind rectangle to every rectangle.

	for (int i = 0; i < contours.size(); i++) {
		approxPolyDP(Mat(contours[i]), contours_poly[i], 1, true);
		boundRect[i] = boundingRect(Mat(contours_poly[i]));
	}

	drawing = Mat::zeros(image2.size(), CV_8UC3);

	for (int i = 0; i < contours.size(); i++) {
		ratio = (double)boundRect[i].height / boundRect[i].width;

		//  Filtering rectangles height/width ratio, and size.

		if ((ratio <= 4) && (ratio >= 0.5) && (boundRect[i].area() >= 100)) {
			drawContours(drawing, contours, i, Scalar(0, 255, 255), 1, 8, hierarchy, 0, Point());
			rectangle(drawing, boundRect[i].tl(), boundRect[i].br(), Scalar(255, 0, 0), 1, 8, 0);


			//  Include only suitable rectangles.
			boundRect2[number_count++] = boundRect[i];
		}

		// find candidate plate
		if ((ratio <= 0.5) && (ratio >= 0.1) && (boundRect[i].area() >= 100)) {
			drawContours(drawing, contours, i, Scalar(255, 255, 0), 1, 8, hierarchy, 0, Point());
			rectangle(drawing, boundRect[i].tl(), boundRect[i].br(), Scalar(255, 255, 0), 1, 8, 0);


			//  Include only suitable rectangles.
			boundRect3[plate_count++] = boundRect[i];
		}
	}

	boundRect2.resize(number_count);  //  Resize refinery rectangle array.
	boundRect3.resize(plate_count);  //  Resize refinery rectangle array.
	//imshow("Original->Gray->Canny->Contours&Rectangles", drawing);
	//waitKey(0);

	//  Bubble Sort accordance with X-coordinate.

	for (int i = 0; i < boundRect2.size() - 1; i++) {
		for (int j = 0; j < (boundRect2.size() - i - 1); j++) {
			if (boundRect2[j].tl().x > boundRect2[j + 1].tl().x) {
				temp_rect = boundRect2[j];
				boundRect2[j] = boundRect2[j + 1];
				boundRect2[j + 1] = temp_rect;
			}
		}
	}

	for (int i = 0; i < boundRect2.size(); i++) {
		rectangle(image3, boundRect2[i].tl(), boundRect2[i].br(), Scalar(0, 255, 0), 1, 8, 0);
		count = 0;

		//  Snake moves to right, for eating his freind.

		for (int j = i + 1; j < boundRect2.size(); j++) {
			delta_x = abs(boundRect2[j].tl().x - boundRect2[i].tl().x);
			if (delta_x > 150)  //  Can't eat snake friend too far ^-^.
				break;
			delta_y = abs(boundRect2[j].tl().y - boundRect2[i].tl().y);
			//  If delta length is 0, it causes a divide-by-zero error.
			if (delta_x == 0) {
				delta_x = 1;
			}
			if (delta_y == 0) {
				delta_y = 1;
			}

			gradient = delta_y / delta_x;  //  Get gradient.
			//cout << gradient << endl;

			if (gradient < 0.25) {  //  Can eat friends only on straight line.
				count += 1;
			}

		}

		//  Find the most full snake.

		if (count > friend_count) {
			bool isInPlate = false;
			for (int k = 0; k < boundRect3.size(); k++)
			{
				if ((boundRect3[k].tl().x <= boundRect2[i].tl().x) && ((boundRect3[k].tl().x + boundRect3[k].width) >= (boundRect2[i].tl().x + boundRect2[i].width))
					&& (boundRect3[k].tl().y <= boundRect2[i].tl().y) && ((boundRect3[k].tl().y + boundRect3[k].height) >= (boundRect2[i].tl().y + boundRect2[i].height)))
				{
					plate_select = k;
					isInPlate = true;
				}
			}

			if (isInPlate)
			{
				number_select = i;  //  Save most full snake number.
				friend_count = count;  //  Renewal number of friends hunting.
				rectangle(image3, boundRect2[number_select].tl(), boundRect2[number_select].br(), Scalar(255, 0, 0), 1, 8, 0);
				plate_width = delta_x;  //  Save the last friend ate position.
			}

		}                           //  It's similar to license plate width, Right?

	}
	//waitKey(0);
	//  Drawing most full snake friend on the image.
	if (number_select == -1)
	{
		cout << "not find the license plate" << endl;
		out.resize(0);
		return out;
	}

	rectangle(image3, boundRect2[number_select].tl(), boundRect2[number_select].br(), Scalar(0, 0, 255), 2, 8, 0);
	line(image3, boundRect2[number_select].tl(), Point(boundRect2[number_select].tl().x + plate_width, boundRect2[number_select].tl().y), Scalar(0, 0, 255), 1, 8, 0);
	//imshow("Rectangles on original", image3);

	//waitKey(0);

	// find optimized plate
	int minArea = 210000000;
	for (int k = 0; k < boundRect3.size(); k++)
	{
		if ((boundRect3[k].tl().x <= boundRect2[number_select].tl().x) && ((boundRect3[k].tl().x + boundRect3[k].width) >= (boundRect2[number_select].tl().x + boundRect2[number_select].width))
			&& (boundRect3[k].tl().y <= boundRect2[number_select].tl().y) && ((boundRect3[k].tl().y + boundRect3[k].height) >= (boundRect2[number_select].tl().y + boundRect2[number_select].height)))
		{
			if (boundRect3[k].area() < minArea)
			{
				plate_select = k;
				minArea = boundRect3[k].area();
			}
		}
	}


	//  Shows license plate, and save image file.

	if (plate_select < 0)
	{
		cout << "not find the license plate" << endl;
		out.resize(0);
		return out;
	}
		//imshow("Region of interest", image(boundRect3[plate_select]));
	//imshow("Region of interest", image(Rect(boundRect2[number_select].tl().x - 20, boundRect2[number_select].tl().y - 20, plate_width + 40, plate_width*0.3)));
	//waitKey(0);



	//imwrite("/home/ubuntu01/Desktop/Plates/1-1.JPG", image(Rect(boundRect2[number_select].tl().x - 20, boundRect2[number_select].tl().y - 20, plate_width + 40, plate_width*0.3)));
	//PlateRect = boundRect3[plate_select];
	image.copyTo(out);
	rectangle(out, boundRect3[plate_select].tl(), boundRect3[plate_select].br(), Scalar(0, 0, 255), 3, 8, 0);
	//imshow("dd", out);
	//waitKey(0);

	return out;

}
Mat CannyEdge(Mat img_gray, int th_high, int th_low)
{
	Mat img_edge;
	img_edge.create(img_gray.size(), img_gray.type());



	int i, j;

	int width = img_gray.cols;
	int height = img_gray.rows;

	unsigned char *pEdge = new uchar[width*height];

	int dx, dy, mag, slope, direction;
	int index, index2;

	const int fbit = 10;
	const int tan225 = 424;       // tan25.5 << fbit, 0.4142
	const int tan675 = 2472;      // tan67.5 << fbit, 2.4142

	const int CERTAIN_EDGE = 255;
	const int PROBABLE_EDGE = 100;

	bool bMaxima;

	int *mag_tbl = new int[width*height];
	int *dx_tbl = new int[width*height];
	int *dy_tbl = new int[width*height];

	unsigned char **stack_top, **stack_bottom;
	stack_top = new unsigned char*[width*height];
	stack_bottom = stack_top;
#define CANNY_PUSH(p)    *(p) = CERTAIN_EDGE, *(stack_top++) = (p)
#define CANNY_POP()      *(--stack_top)

	for (i = 0; i < width*height; i++) {
		mag_tbl[i] = 0;
	}

	// Sobel Edge Detection
	for (i = 1; i < height - 1; i++) {
		index = i * width;
		for (j = 1; j < width - 1; j++) {
			index2 = index + j;
			// -1 0 1
			// -2 0 2
			// -1 0 1
			dx = ((int)img_gray.at<uchar>(i - 1, j - 1) * (-1)) + ((int)img_gray.at<uchar>(i - 1, j + 1) * (1))
				+ ((int)img_gray.at<uchar>(i, j - 1) * (-2)) + ((int)img_gray.at<uchar>(i, j + 1) * (2))
				+ ((int)img_gray.at<uchar>(i + 1, j - 1) * (-1)) + ((int)img_gray.at<uchar>(i + 1, j + 1) * (1));

			// -1 -2 -1
			//  0  0  0
			//  1  2  1
			dy = ((int)img_gray.at<uchar>(i - 1, j - 1) * (-1)) + ((int)img_gray.at<uchar>(i - 1, j) * (-2)) + ((int)img_gray.at<uchar>(i - 1, j + 1) * (-1))
				+ ((int)img_gray.at<uchar>(i + 1, j - 1) * (1)) + ((int)img_gray.at<uchar>(i + 1, j) * (2)) + ((int)img_gray.at<uchar>(i + 1, j + 1) * (1));

			mag = abs(dx) + abs(dy);     // magnitude
			//mag = sqrtf(dx*dx + dy*dy);

			dx_tbl[index2] = dx;
			dy_tbl[index2] = dy;
			mag_tbl[index2] = mag;              // store the magnitude in the table
		}   // for(j)
	}   // for(i)

	for (i = 1; i < height - 1; i++) {
		index = i * width;
		for (j = 1; j < width - 1; j++) {
			index2 = index + j;

			mag = mag_tbl[index2];              // retrieve the magnitude from the table

			// if the magnitude is greater than the lower threshold
			if (mag > th_low) {

				// determine the orientation of the edge
				dx = dx_tbl[index2];
				dy = dy_tbl[index2];

				if (dx != 0) {
					slope = (dy << fbit) / dx;

					if (slope > 0) {
						if (slope < tan225)
							direction = 0;
						else if (slope < tan675)
							direction = 1;
						else
							direction = 2;
					}
					else {
						if (-slope > tan675)
							direction = 2;
						else if (-slope > tan225)
							direction = 3;
						else
							direction = 0;
					}
				}
				else
					direction = 2;

				bMaxima = true;
				// perform non-maxima suppression
				if (direction == 0) {
					if (mag < mag_tbl[index2 - 1] || mag < mag_tbl[index2 + 1])
						bMaxima = false;
				}
				else if (direction == 1) {
					if (mag < mag_tbl[index2 + width + 1] || mag < mag_tbl[index2 - width - 1])
						bMaxima = false;
				}
				else if (direction == 2) {
					if (mag < mag_tbl[index2 + width] || mag < mag_tbl[index2 - width])
						bMaxima = false;
				}
				else { // if(direction == 3)
					if (mag < mag_tbl[index2 + width - 1] || mag < mag_tbl[index2 - width + 1])
						bMaxima = false;
				}

				if (bMaxima) {
					if (mag > th_high) {
						pEdge[index2] = CERTAIN_EDGE;           // the pixel does belong to an edge
						*(stack_top++) = (unsigned char*)(pEdge + index2);
					}
					else
						pEdge[index2] = PROBABLE_EDGE;          // the pixel might belong to an edge
				}
			}

		}   // for(j)
	}   // for(i)

	while (stack_top != stack_bottom) {
		unsigned char* p = CANNY_POP();

		if (*(p + 1) == PROBABLE_EDGE)
			CANNY_PUSH(p + 1);
		if (*(p - 1) == PROBABLE_EDGE)
			CANNY_PUSH(p - 1);
		if (*(p + width) == PROBABLE_EDGE)
			CANNY_PUSH(p + width);
		if (*(p - width) == PROBABLE_EDGE)
			CANNY_PUSH(p - width);
		if (*(p - width - 1) == PROBABLE_EDGE)
			CANNY_PUSH(p - width - 1);
		if (*(p - width + 1) == PROBABLE_EDGE)
			CANNY_PUSH(p - width + 1);
		if (*(p + width - 1) == PROBABLE_EDGE)
			CANNY_PUSH(p + width - 1);
		if (*(p + width + 1) == PROBABLE_EDGE)
			CANNY_PUSH(p + width + 1);
	}

	for (i = 0; i < width*height; i++)
		if (pEdge[i] != CERTAIN_EDGE)
			pEdge[i] = 0;

	for (i = 0; i < height; i++) {
		index = i * width;
		for (j = 0; j < width; j++) {
			index2 = index + j;
			img_edge.at<uchar>(i, j) = pEdge[index2];
		}
	}

	delete[] mag_tbl;
	delete[] dx_tbl;
	delete[] dy_tbl;
	delete[] stack_bottom;

	return img_edge;
}
