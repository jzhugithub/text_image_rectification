#ifndef __ZDMETHOD_HPP__
#define __ZDMETHOD_HPP__

#include <cmath>
#include <opencv2/opencv.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>

using namespace cv;
using namespace std;

//-----------------------------------¡¾horizon( ) class¡¿----------------------------------  
//      description: horizontal integration to get the spacing.
//-----------------------------------------------------------------------------------------

class zdmethod{
public:
	 static Mat horizon(Mat &src1)
	{
		Mat src;
		cvtColor(src1, src, CV_BGR2GRAY); //thresholding to get the binary image.
		Mat painty(src.rows, src.cols, CV_8U, Scalar(100));
		int* v = new int[src.cols];
		int* h = new int[src.rows];
		memset(h, 0, src.rows * 4);
		int x, y;
		uchar * data;
		//CvScalar s, t;   //array with four double numbers. 

		for (y = 0; y<src.rows; y++)  
		{
			data = src.ptr<uchar>(y);
			for (x = 0; x<src.cols; x++)
			{
				if (data[x] == 0)   //larger h means higher possibility of the existence of the words.
					h[y]++;
			}
		}

		for (y = 0; y<src.rows; y++)
		{
			data = painty.ptr<uchar>(y);
			for (x = 0; x<h[y]; x++)
			{
				data[x] = 255;  //paint the integration image.
			}
		}

		int count = 0;
		int flag0 = 0;
		int flag1 = 0;
		int flag2 = 0;

		/*for (y = 0; y < src->height; y++){  //to get the position of each line.
		if (h[y]>500){
		line((Mat)(src), Point(0, y), Point(50, y), Scalar(255,255,255));
		}
		}*/

		for (y = 0; y < src.rows; y++){  //to get the position of each line.
			if (h[y] <500){
				flag0 = 1;  //it means that the first line occurs.
			}
			if (flag0 == 1 && h[y] >500){      //achieve the spacing.
				for (int m = y; m<src.rows; m++){
					if (h[m] > 500){
						flag1 = 1;   //achieve the next line.
						count++;
					}
					if (flag1 == 1 && h[m] <500){
						flag2 = 1;  //get the location of line.
						flag0 = 0;
						flag1 = 0;
						break;
					}
				}
				if (flag2 == 1){
					line(src, Point(0, y + (count / 2)), Point(50, y + (count / 2)), Scalar(255, 255, 255));
					count = 0;
					flag2 = 0;
				}
			}
			//line((Mat)(painty), Point(0, y), Point(50, y), Scalar(0, 0, 0));  //threshold changes with the peak
		}
		return painty;
		//return src;
	}
};
#endif