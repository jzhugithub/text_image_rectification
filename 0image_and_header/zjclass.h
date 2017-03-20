#ifndef __ZJCLASS_HPP__
#define __ZJCLASS_HPP__

#include <cmath>
#include <opencv2/opencv.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>



#include <vector>  
#include<complex>  
#include<bitset>  
#include <iterator>  
#include <algorithm>  
#include <iostream>  
#include <sstream>  
const double pai = 3.1415926535897932384626433832795028841971; // 圆周率  

//typedef vector<vector<complex<double> > >VVCD;  
//typedef vector<vector<double> >VVD;  



using namespace cv;
using namespace std;
//-----------------------------------【VerBaseLine类】---------------------------------------------  
//      描述：寻找输入线段中的垂直基准线：接近竖直且最长
//---------------------------------------------------------------------------------------------- 

class VerBaseLine
{
public:
	int baseLiN;//最长线段编号
	int baseL;//最长线段长度
	Vec4i baseSeg;//最长线段向量
	Mat image;//输入的图像
	vector<Vec4i> lines;//输入的线段集合
	VerBaseLine(){}//默认构造函数
	VerBaseLine(vector<Vec4i> lines)//初始化，仅输入线段集合
	{
		baseLiN = -1;//基准线段编号
		baseL = -1;//基准线段长度
		this->lines = lines;//输入的线段集合

		for( size_t i = 0; i < lines.size(); i++ )//寻找基准线段
		{
			Vec4i l = lines[i];
			double thisLine = sqrt(pow((double)(l[0]-l[2]),2)+pow((double)(l[1]-l[3]),2));//此线长度
			double angle = atan(abs(double(l[2]-l[0])/double(l[3]-l[1])))/CV_PI*180;//此线与y轴所呈角度的绝对值
			if (thisLine > baseL && angle < 45)
			{
				baseL = thisLine;
				baseLiN = i;
				baseSeg = l;
			}
		}
	}
	VerBaseLine(vector<Vec4i> lines,Mat image)//初始化，输入线段集合与图像
	{
		baseLiN = -1;//最长线段编号
		baseL = -1;//最长线段长度
		this->lines = lines;//输入的线段集合
		this->image = image;//初始化图像

		for( size_t i = 0; i < lines.size(); i++ )//寻找基准线段
		{
			Vec4i l = lines[i];
			double thisLine = sqrt(pow((double)(l[0]-l[2]),2)+pow((double)(l[1]-l[3]),2));//此线长度
			double angle = atan(abs(double(l[2]-l[0])/double(l[3]-l[1])))/CV_PI*180;//此线与y轴所呈角度的绝对值
			if (thisLine > baseL && angle < 45)
			{
				baseL = thisLine;
				baseLiN = i;
				baseSeg = l;
			}
		}
	}
	int baseLineNumber()
	{
		if (baseLiN != -1)
		{
			return baseLiN;
		} 
		else
		{
			cout<<"没有找到轴线"<<endl;getchar(); return -1;
		}
	}
	int baseLength()
	{
		if (baseL != -1)
		{
			return baseL;
		} 
		else
		{
			cout<<"没有找到轴线"<<endl;getchar(); return -1;
		}
	}
	Vec4i baseSegment()
	{
		if (baseLiN != -1)
		{
			return baseSeg;
		} 
		else
		{
			cout<<"没有找到轴线"<<endl;getchar(); return -1;
		}
	}
	void drawOnImage(const Scalar& color = Scalar(0,0,255))
	{
		if (!image.empty())
		{
			line( image, Point(baseSeg[0], baseSeg[1]), Point(baseSeg[2], baseSeg[3]), color, 3, CV_AA);
		} 
		else
		{
			cout<<"请先输入图像"<<endl;getchar();
		}

	}
};
//-----------------------------------【HorBaseLine类】---------------------------------------------  
//      描述：寻找输入线段中的水平基准线：接近水平且最长
//---------------------------------------------------------------------------------------------- 

class HorBaseLine
{
public:
	int baseLiN;//最长线段编号
	int baseL;//最长线段长度
	Vec4i baseSeg;//最长线段向量
	Mat image;//输入的图像
	vector<Vec4i> lines;//输入的线段集合
	HorBaseLine(){}//默认构造函数
	HorBaseLine(vector<Vec4i> lines)//初始化，仅输入线段集合
	{
		baseLiN = -1;//基准线段编号
		baseL = -1;//基准线段长度
		this->lines = lines;//输入的线段集合

		for( size_t i = 0; i < lines.size(); i++ )//寻找基准线段
		{
			Vec4i l = lines[i];
			double thisLine = sqrt(pow((double)(l[0]-l[2]),2)+pow((double)(l[1]-l[3]),2));//此线长度
			double angle = atan(abs(double(l[2]-l[0])/double(l[3]-l[1])))/CV_PI*180;//此线与y轴所呈角度的绝对值
			if (thisLine > baseL && angle > 60)
			{
				baseL = thisLine;
				baseLiN = i;
				baseSeg = l;
			}
		}
	}
	HorBaseLine(vector<Vec4i> lines,Mat image)//初始化，输入线段集合与图像
	{
		baseLiN = -1;//最长线段编号
		baseL = -1;//最长线段长度
		this->lines = lines;//输入的线段集合
		this->image = image;//初始化图像

		for( size_t i = 0; i < lines.size(); i++ )//寻找基准线段
		{
			Vec4i l = lines[i];
			double thisLine = sqrt(pow((double)(l[0]-l[2]),2)+pow((double)(l[1]-l[3]),2));//此线长度
			double angle = atan(abs(double(l[2]-l[0])/double(l[3]-l[1])))/CV_PI*180;//此线与y轴所呈角度的绝对值
			if (thisLine > baseL && angle > 60)
			{
				baseL = thisLine;
				baseLiN = i;
				baseSeg = l;
			}
		}
	}
	int baseLineNumber()
	{
		if (baseLiN != -1)
		{
			return baseLiN;
		} 
		else
		{
			cout<<"没有找到水平基准线"<<endl;getchar(); return -1;
		}
	}
	int baseLength()
	{
		if (baseL != -1)
		{
			return baseL;
		} 
		else
		{
			cout<<"没有找到水平基准线"<<endl;getchar(); return -1;
		}
	}
	Vec4i baseSegment()
	{
		if (baseLiN != -1)
		{
			return baseSeg;
		} 
		else
		{
			cout<<"没有找到水平基准线"<<endl;getchar(); return -1;
		}
	}
	void drawOnImage(const Scalar& color = Scalar(0,0,255))
	{
		if (!image.empty())
		{
			line( image, Point(baseSeg[0], baseSeg[1]), Point(baseSeg[2], baseSeg[3]), color, 2, CV_AA);
		} 
		else
		{
			cout<<"请先输入图像"<<endl;getchar();
		}

	}
};
//-----------------------------------【RotateImage类】------------------------------------------  
//      描述：将图片旋转指定角度都输出
//---------------------------------------------------------------------------------------------- 
class RotateImage
{
public:
	Mat inputImage;//输入图像
	Vec4i inputLine;//输入的线段
	Mat outputImage;//输出图像
	int outpulLineCol;//变换后的线段列坐标
	double angle;//旋转角度
	Point center;//旋转中心点
	RotateImage(){}//默认构造函数
	RotateImage(Mat inputImage, Vec4i inputLine)//初始化类，输入图像与垂直基准线
	{
		this->inputImage = inputImage;
		this->inputLine = inputLine;
	}

	Mat trans1()//旋转方式1
	{
		Mat rotMat( 2, 3, CV_32FC1 );//初始化旋转矩阵

		//初始化旋转角度
		angle = -atan((double)(inputLine[2]-inputLine[0])/(double)(inputLine[3]-inputLine[1]))/CV_PI*180;

		//初始化目标图像的大小
		outputImage = Mat::ones(inputImage.rows, inputImage.cols, inputImage.type());

		//对图像进行旋转
		int x = (inputLine[2]-inputLine[0])*(inputImage.rows/2-inputLine[1])/(inputLine[3]-inputLine[1])+inputLine[0];
		int y = inputImage.rows/2;
		center = Point( x, y );
		outpulLineCol = x;
		double scale = 1;
		// 通过上面的旋转细节信息求得旋转矩阵
		rotMat = getRotationMatrix2D( center, angle, scale );
		// 旋转已缩放后的图像
		//对源图像应用刚刚求得的仿射变换
		if (inputImage.channels() == 3)
		{
			warpAffine( inputImage, outputImage, rotMat, outputImage.size(), 1, 0, Scalar(255,255,255) );
			/*	参数7调整底色*/
		}
		if (inputImage.channels() == 1)
		{
			warpAffine( inputImage, outputImage, rotMat, outputImage.size(), INTER_NEAREST, 0, Scalar(0) );
		}
		return outputImage;
	}
};
//-----------------------------------【HorAdjustImage类】------------------------------------------  
//      描述：依据水平基准线将图片进行水平调整，使得水平基准线变水平
//---------------------------------------------------------------------------------------------- 
class HorAdjustImage
{
public:
	Mat inputImage;//输入图像
	Vec4i inputLine;//输入的线段
	Mat outputImage;//输出图像
	int outpulLineRow;//变换后的线段行坐标
	HorAdjustImage(){}//默认构造函数
	HorAdjustImage(Mat inputImage, Vec4i inputLine)//初始化类，输入图像与水平基准线
	{
		this->inputImage = inputImage;
		this->inputLine = inputLine;
	}

	Mat trans()//进行行基准线调整
	{
		outpulLineRow = inputLine[1];

		//定义两组点，代表两个三角形
		Point2f srcTriangle[3];
		Point2f dstTriangle[3];

		//设置源图像和目标图像上的三组点以计算仿射变换
		srcTriangle[0] = Point2f( inputLine[0],inputLine[1] );
		srcTriangle[1] = Point2f( inputLine[2], inputLine[3] );
		srcTriangle[2] = Point2f( inputLine[0], inputLine[1]+1 );

		dstTriangle[0] = Point2f( inputLine[0],inputLine[1] );
		dstTriangle[1] = Point2f( inputLine[2], inputLine[1] );
		dstTriangle[2] = Point2f( inputLine[0], inputLine[1]+1 );

		//定义旋转矩阵
		Mat rotMat( 2, 3, CV_32FC1 );

		//计算仿射变换
		rotMat = getAffineTransform( srcTriangle, dstTriangle );

		//对源图像应用刚刚求得的仿射变换
		if (inputImage.channels() == 3)
		{
			warpAffine( inputImage, outputImage, rotMat, outputImage.size(), 1, 0, Scalar(255,255,255) );
		}
		if (inputImage.channels() == 1)
		{
			warpAffine( inputImage, outputImage, rotMat, outputImage.size(), INTER_NEAREST, 0, Scalar(0) );
		}
		return outputImage;
	}
};
//-----------------------------------【WordCorrection类】------------------------------------------  
//      描述：在水平方向上矫正字的宽度
//---------------------------------------------------------------------------------------------- 
class WordCorrection
{
public:
	vector<int> x;//字间间隔位置x坐标在输入图像上的位置
	vector<int> wordInterval;//两个相邻x值的间距（文字宽度,包括前一个间隔值，不包括后一个间隔值）
	int yh;//处理区域y上限
	int yl;//处理区域y下限
	Mat inputImage;//输入的图像
	Mat outputImage;//输出的图像
	int targetInterval;//目标x值间距（文字宽度,包括前一个间隔值，不包括后一个间隔值）
	int targetImageWidth;//目标图像的宽度
	int targetImageHeight;//目标图像的高度
	int num;//x的元素个数
	WordCorrection(){}//默认构造函数
	WordCorrection(Mat inputImage0,vector<int> a,int yh0,int yl0)//初始化类，输入图像、字间间隔位置a、处理区域y上限、处理区域y下限
	{
		//为类中的变量赋初值
		num = sizeof(a)/sizeof(a[0]);
		inputImage = inputImage0;
		if (yh0>yl0)
		{
			yh = yh0;
			yl = yl0;
		}
		else
		{
			yh = yl0;
			yl = yh0;
		}
		if (a[0]<a[1])//判断输入x数组的排列方式,让x数组以从小到大顺序排列
		{
			for (int i=0;i<num;i++)//为字间隔位置x坐标数值赋值
			{
				x.push_back(a[i]);
			}
			for (int i = 1;i<num;i++)//为两个相邻x值的间距数值赋值
			{
				wordInterval.push_back(a[i]-a[i-1]);
			}
		}
		else
		{
			for (int i=num-1;i>=0;i--)//为字间隔位置x坐标数值赋值
			{
				x.push_back(a[i]);
			}
			for (int i = num-1;i>0;i--)//为两个相邻x值的间距数值赋值
			{
				wordInterval.push_back(a[i-1]-a[i]);
			}
		}

		targetInterval = wordInterval.at(int(num/2+1));//将最中间的间隔作为目标间隔
		targetImageWidth = (num-1)*targetInterval;//输出图像的宽度
		targetImageHeight = yh-yl+1;//输出图像的高度
		outputImage = Mat::zeros(Size(targetImageWidth,targetImageHeight),inputImage.type());//输出图像初始化
	}
	Mat trans()//进行横向畸变矫正
	{
		for (int i=1;i<num;i++)
		{
			//定义两组点，代表两个三角形
			Point2f srcTriangle[3];
			Point2f dstTriangle[3];

			//设置源图像和目标图像上的三组点以计算仿射变换
			srcTriangle[0] = Point2f( x.at(i-1),0 );
			srcTriangle[1] = Point2f( static_cast<float>(x.at(i) - 1), 0 );
			srcTriangle[2] = Point2f( 0, static_cast<float>(inputImage.cols - 1 ));

			dstTriangle[0] = Point2f( static_cast<float>(targetInterval*(i-1)),0 );
			dstTriangle[1] = Point2f( static_cast<float>(targetInterval*i), 0);
			dstTriangle[2] = Point2f( 0, static_cast<float>(outputImage.cols - 1 ));

			//定义旋转矩阵
			Mat rotMat( 2, 3, CV_32FC1 );

			//计算仿射变换
			rotMat = getAffineTransform( srcTriangle, dstTriangle );

			//对源图像应用刚刚求得的仿射变换
			warpAffine( inputImage, outputImage, rotMat, outputImage.size() );
		}
	}

};

#endif
