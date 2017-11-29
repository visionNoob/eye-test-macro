#include "use_opencv.h"
#include <Windows.h>
#include <iostream>
#include "Histogram1D.hpp"

using namespace std;
using namespace cv;

bool isBegin_Macro = false;

//모니터내 게임판 위치
Rect ROI = Rect(617, 112, 891, 891);

const double resizedInputWidth = 640.0;
const double resizedInputHeight = 480.0;

//모니터화면 받아와서 Mat으로 저장
Mat hwnd2mat(HWND hwnd)
{
	HDC hwindowDC, hwindowCompatibleDC;

	int height, width, srcheight, srcwidth;
	HBITMAP hbwindow;
	Mat src;
	BITMAPINFOHEADER  bi;

	hwindowDC = GetDC(hwnd);
	hwindowCompatibleDC = CreateCompatibleDC(hwindowDC);
	SetStretchBltMode(hwindowCompatibleDC, COLORONCOLOR);

	RECT windowsize;    // get the height and width of the screen
	GetClientRect(hwnd, &windowsize);

	srcheight = windowsize.bottom;
	srcwidth = windowsize.right;
	height = windowsize.bottom / 1;  //change this to whatever size you want to resize to
	width = windowsize.right / 1;

	src.create(height, width, CV_8UC4);

	// create a bitmap
	hbwindow = CreateCompatibleBitmap(hwindowDC, width, height);
	bi.biSize = sizeof(BITMAPINFOHEADER);    //http://msdn.microsoft.com/en-us/library/windows/window/dd183402%28v=vs.85%29.aspx
	bi.biWidth = width;
	bi.biHeight = -height;  //this is the line that makes it draw upside down or not
	bi.biPlanes = 1;
	bi.biBitCount = 32;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrUsed = 0;
	bi.biClrImportant = 0;

	// use the previously created device context with the bitmap
	SelectObject(hwindowCompatibleDC, hbwindow);
	// copy from the window device context to the bitmap device context
	StretchBlt(hwindowCompatibleDC, 0, 0, width, height, hwindowDC, 0, 0, srcwidth, srcheight, SRCCOPY); //change SRCCOPY to NOTSRCCOPY for wacky colors !
	GetDIBits(hwindowCompatibleDC, hbwindow, 0, height, src.data, (BITMAPINFO *)&bi, DIB_RGB_COLORS);  //copy from hwindowCompatibleDC to hbwindow

																									   // avoid memory leak
	DeleteObject(hbwindow);
	DeleteDC(hwindowCompatibleDC);
	ReleaseDC(hwnd, hwindowDC);

	return src;
}


int main(int argc, char **argv)
{
	HWND hwndDesktop = GetDesktopWindow();
	namedWindow("output");
	int key = 0;
	Mat src, gray, roiMat, tempRoiMat;
	Histogram1D h; // 히스토그램 객체


	while (key != 60){
		src = hwnd2mat(hwndDesktop);
		cvtColor(src, gray, CV_RGB2GRAY);
		roiMat = gray(ROI);
		cv::resizeWindow("output", 640, 480);
		resize(roiMat, roiMat, Size(640, 480));

		roiMat.copyTo(tempRoiMat);
		cv::MatND histo = h.getHistogram(roiMat); // 히스토그램 계산

		double maxVal = 0; // 최대 빈도수 가져오기
		double minVal = 0; // 최소 빈도수 가져오기
		Point maxLoc = 0; // 최대 빈도수 가져오기
		Point minLoc = 0; // 최소 빈도수 가져오기

		cv::minMaxLoc(h.getHistogram(roiMat), &minVal, &maxVal, &minLoc, &maxLoc);


		for (int j = 0; j < 480; j++) {
			for (int i = 0; i < 640; i++) {
				if (tempRoiMat.at<uchar>(j, i) == maxLoc.y - 1) {
					tempRoiMat.at<uchar>(j, i) = 255;
				}
			}
		}


		//모폴로지
		cv::Mat element5(15, 15, CV_8U, cv::Scalar(255));
		
		cv::Mat closed;
		closed = cv::Scalar(255) - tempRoiMat;
		
		threshold(tempRoiMat, closed, 250, 255, ThresholdTypes::THRESH_BINARY);

		cv::morphologyEx(closed, closed, cv::MORPH_DILATE, element5);
		cv::morphologyEx(closed, closed, cv::MORPH_DILATE, element5);
		cv::morphologyEx(closed, closed, cv::MORPH_DILATE, element5);

		cv::morphologyEx(closed, closed, cv::MORPH_ERODE, element5);
		cv::morphologyEx(closed, closed, cv::MORPH_ERODE, element5);

		//레이블링
		Mat labels;
		Mat stats;
		Mat centroids;
		cv::connectedComponentsWithStats(closed, labels, stats, centroids);

		int x = stats.at<int>(Point(0, 0));
		int y = stats.at<int>(Point(1, 0));
		int w = stats.at<int>(Point(2, 0));
		int h = stats.at<int>(Point(3, 0));

		if (isBegin_Macro) {

			POINT real;
			real.x = x + w / 2.0;
			real.y = y + h / 2.0;

			POINT origin;
			POINT current;
			GetCursorPos(&origin);

			SetCursorPos(real.x * 891.0 / 640.0 + 617, real.y* 891.0 / 480 + 112);
			Sleep(0.1);
			GetCursorPos(&current);
			mouse_event(MOUSEEVENTF_LEFTDOWN, current.x, current.y - 15, 0, 0); // 좌표에 맞게 왼쪽버튼 누르기
			Sleep(0.1);
			mouse_event(MOUSEEVENTF_LEFTUP, current.x, current.y - 15, 0, 0);
			Sleep(0.1);
			SetCursorPos(origin.x, origin.y);

		}

		imshow("image", closed);
		imshow("output", roiMat);
		key = waitKey(1); // you can change wait time

		if (key == 's') {
			isBegin_Macro == true ? isBegin_Macro = false : isBegin_Macro = true;
		}
	}

}