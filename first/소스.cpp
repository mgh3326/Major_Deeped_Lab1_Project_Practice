#define _CRT_SECURE_NO_WARNINGS
#include "opencv2\opencv.hpp"

#include <iostream>
#include <cstdlib>
#include <vector>
#include <string>
#include <sstream>
#include <istream>
#include <fstream>

using namespace cv;

using namespace std;


std::vector<std::string> csv_read_row(std::istream &in, char delimiter);
std::vector<std::string> csv_read_row(std::string &in, char delimiter);


typedef struct License {
	string path;
	Point pos;
};
/// <summary>
/// Rect 안에 Point가 있는지 확인하는 함수 입니다.
/// </summary>
/// <param name="rect"></param>
/// <param name="p"></param>
/// <returns></returns>
bool IsInside(Rect rect, Point p) {
	//등호 있으면 경계 포함
	if (rect.x <= p.x&&rect.y <= p.y&&rect.x + rect.width >= p.x&&rect.y + rect.height >= p.y) {
		return true;
	}
	else {
		return false;

	}
}
int main(int argc, char *argv[])
{
	std::ifstream in("test.csv");
	if (in.fail()) return (cout << "File not found" << endl) && 0;
	std::vector<License> license_row;//license_row


	while (in.good())//csv read
	{
		std::vector<std::string> row = csv_read_row(in, ',');
		License temp;
		if (row.size() >= 3)
		{
			temp.path = row[0];
			/*temp.pos.x = stoi(row[1]);*/
			temp.pos = { stoi(row[1]), stoi(row[2]) };
			license_row.push_back(temp);
		}
	}
	in.close();

	for (int i = 0, leng = license_row.size(); i < leng; i++)//csv print
	{
		cout << "[" << license_row[i].path << "]" << "\t";
		cout << "[" << license_row[i].pos << "]" << "\t";
		cout << endl;

	}
	Point pTopLeft(1, 10);
	Point pBottomRight(5, 5);
	cv::Rect rRect(pTopLeft, pBottomRight);
	cout << "Test Rect pTopLeft, pBottomRight" << rRect.tl() << rRect.br() << endl;
	for (int i = 0, leng = license_row.size(); i < leng; i++)//IsInside print
	{
		cout << "[" << license_row[i].path << "]" << "\t";
		cout << "[" << std::boolalpha << IsInside(rRect, license_row[i].pos) << "]" << "\t";


		cout << endl;

	}
	return 0;
}

std::vector<std::string> csv_read_row(std::string &line, char delimiter)
{
	std::stringstream ss(line);
	return csv_read_row(ss, delimiter);
}

std::vector<std::string> csv_read_row(std::istream &in, char delimiter)
{
	std::stringstream ss;
	bool inquotes = false;
	std::vector<std::string> row;//relying on RVO
	while (in.good())
	{
		char c = in.get();
		if (!inquotes && c == '"') //beginquotechar
		{
			inquotes = true;
		}
		else if (inquotes && c == '"') //quotechar
		{
			if (in.peek() == '"')//2 consecutive quotes resolve to 1
			{
				ss << (char)in.get();
			}
			else //endquotechar
			{
				inquotes = false;
			}
		}
		else if (!inquotes && c == delimiter) //end of field
		{
			row.push_back(ss.str());
			ss.str("");
		}
		else if (!inquotes && (c == '\r' || c == '\n'))
		{
			if (in.peek() == '\n') { in.get(); }
			row.push_back(ss.str());
			return row;
		}
		else
		{
			ss << c;
		}
	}
}