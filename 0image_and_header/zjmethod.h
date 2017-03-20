#ifndef __ZJMETHOD_HPP__
#define __ZJMETHOD_HPP__

#include <cmath>
#include <opencv2/opencv.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>

using namespace cv;
using namespace std;

class ZjMethod
{
public:
	//-----------------------------------【expandLine( )函数】----------------------------------  
	//      描述：将二值化图像的白色部分进行扩大
	//---------------------------------------------------------------------------------------------- 
	static Mat expandLine(const Mat &image)
	{
		Mat expImage = Mat::zeros( image.rows, image.cols, image.type() );//初始化一张与image同类型的空白（全黑）图像
		int sum=0;//统计目标方格（如2×2）内是否有白色点

		for (int i=1;i<image.rows-1;i++)
		{
			for (int j=1;j<image.cols-1;j++,sum=0)//遍历图像中的点
			{
				if (image.at<uchar>(i,j)==255)//如果点(i,j)为白色，则将(i+1,j)与(i,j+1)也涂成白色
				{
					expImage.at<uchar>(i+1,j)=255;
					expImage.at<uchar>(i,j+1)=255;
				}
			}

		}
		return expImage;
	}

	//-----------------------------------【thin( )函数】----------------------------------  
	//      描述：将 DEPTH_8U型二值图像进行细化  经典的Zhang并行快速细化算法
	//		输入：输入图像、输出图像、迭代次数
	//---------------------------------------------------------------------------------------------- 
	static void thin(const Mat &src, Mat &dst, const int iterations)
	{
		const int height =src.rows -1;
		const int width  =src.cols -1;

		//拷贝一个数组给另一个数组
		if(src.data != dst.data)
		{
			src.copyTo(dst);
		}


		int n = 0,i = 0,j = 0;
		Mat tmpImg;
		uchar *pU, *pC, *pD;
		bool isFinished = false;

		for(n=0; n<iterations; n++)
		{
			dst.copyTo(tmpImg); 
			isFinished = false;   //一次 先行后列扫描 开始
			//扫描过程一 开始
			for(i=1; i<height;  i++) 
			{
				pU = tmpImg.ptr<uchar>(i-1);
				pC = tmpImg.ptr<uchar>(i);
				pD = tmpImg.ptr<uchar>(i+1);
				for(int j=1; j<width; j++)
				{
					if(pC[j] > 0)
					{
						int ap=0;
						int p2 = (pU[j] >0);
						int p3 = (pU[j+1] >0);
						if (p2==0 && p3==1)
						{
							ap++;
						}
						int p4 = (pC[j+1] >0);
						if(p3==0 && p4==1)
						{
							ap++;
						}
						int p5 = (pD[j+1] >0);
						if(p4==0 && p5==1)
						{
							ap++;
						}
						int p6 = (pD[j] >0);
						if(p5==0 && p6==1)
						{
							ap++;
						}
						int p7 = (pD[j-1] >0);
						if(p6==0 && p7==1)
						{
							ap++;
						}
						int p8 = (pC[j-1] >0);
						if(p7==0 && p8==1)
						{
							ap++;
						}
						int p9 = (pU[j-1] >0);
						if(p8==0 && p9==1)
						{
							ap++;
						}
						if(p9==0 && p2==1)
						{
							ap++;
						}
						if((p2+p3+p4+p5+p6+p7+p8+p9)>1 && (p2+p3+p4+p5+p6+p7+p8+p9)<7)
						{
							if(ap==1)
							{
								if((p2*p4*p6==0)&&(p4*p6*p8==0))
								{                           
									dst.ptr<uchar>(i)[j]=0;
									isFinished = true;                            
								}

								//   if((p2*p4*p8==0)&&(p2*p6*p8==0))
								//    {                           
								//         dst.ptr<uchar>(i)[j]=0;
								//         isFinished =TRUE;                            
								//    }

							}
						}                    
					}

				} //扫描过程一 结束


				dst.copyTo(tmpImg); 
				//扫描过程二 开始
				for(i=1; i<height;  i++)  //一次 先行后列扫描 开始
				{
					pU = tmpImg.ptr<uchar>(i-1);
					pC = tmpImg.ptr<uchar>(i);
					pD = tmpImg.ptr<uchar>(i+1);
					for(int j=1; j<width; j++)
					{
						if(pC[j] > 0)
						{
							int ap=0;
							int p2 = (pU[j] >0);
							int p3 = (pU[j+1] >0);
							if (p2==0 && p3==1)
							{
								ap++;
							}
							int p4 = (pC[j+1] >0);
							if(p3==0 && p4==1)
							{
								ap++;
							}
							int p5 = (pD[j+1] >0);
							if(p4==0 && p5==1)
							{
								ap++;
							}
							int p6 = (pD[j] >0);
							if(p5==0 && p6==1)
							{
								ap++;
							}
							int p7 = (pD[j-1] >0);
							if(p6==0 && p7==1)
							{
								ap++;
							}
							int p8 = (pC[j-1] >0);
							if(p7==0 && p8==1)
							{
								ap++;
							}
							int p9 = (pU[j-1] >0);
							if(p8==0 && p9==1)
							{
								ap++;
							}
							if(p9==0 && p2==1)
							{
								ap++;
							}
							if((p2+p3+p4+p5+p6+p7+p8+p9)>1 && (p2+p3+p4+p5+p6+p7+p8+p9)<7)
							{
								if(ap==1)
								{
									//   if((p2*p4*p6==0)&&(p4*p6*p8==0))
									//   {                           
									//         dst.ptr<uchar>(i)[j]=0;
									//         isFinished =TRUE;                            
									//    }

									if((p2*p4*p8==0)&&(p2*p6*p8==0))
									{                           
										dst.ptr<uchar>(i)[j]=0;
										isFinished = true;                            
									}

								}
							}                    
						}

					}

				} //一次 先行后列扫描完成          
				//如果在扫描过程中没有删除点，则提前退出
				if(isFinished == false)
				{
					break; 
				}
			}

		}
	}
	//-----------------------------------【showregion( )函数】----------------------------------  
	//      描述：将输入图像分成10幅显示
	//---------------------------------------------------------------------------------------------- 
	static void showRegion(Mat &thresholdImage)
	{
		Mat imageROI[10];
		imageROI[0]=thresholdImage(Rect(1,1,thresholdImage.cols/2-1,thresholdImage.rows/5-1));
		imageROI[1]=thresholdImage(Rect(thresholdImage.cols/2-1,1,thresholdImage.cols/2-1,thresholdImage.rows/5-1));
		imageROI[2]=thresholdImage(Rect(1,thresholdImage.rows/5-1,thresholdImage.cols/2-1,thresholdImage.rows/5-1));
		imageROI[3]=thresholdImage(Rect(thresholdImage.cols/2-1,thresholdImage.rows/5-1,thresholdImage.cols/2-1,thresholdImage.rows/5-1));
		imageROI[4]=thresholdImage(Rect(1,(thresholdImage.rows/5-1)*2,thresholdImage.cols/2-1,thresholdImage.rows/5-1));
		imageROI[5]=thresholdImage(Rect(thresholdImage.cols/2-1,(thresholdImage.rows/5-1)*2,thresholdImage.cols/2-1,thresholdImage.rows/5-1));
		imageROI[6]=thresholdImage(Rect(1,(thresholdImage.rows/5-1)*3,thresholdImage.cols/2-1,thresholdImage.rows/5-1));
		imageROI[7]=thresholdImage(Rect(thresholdImage.cols/2-1,(thresholdImage.rows/5-1)*3,thresholdImage.cols/2-1,thresholdImage.rows/5-1));
		imageROI[8]=thresholdImage(Rect(1,(thresholdImage.rows/5-1)*4,thresholdImage.cols/2-1,thresholdImage.rows/5-1));
		imageROI[9]=thresholdImage(Rect(thresholdImage.cols/2-1,(thresholdImage.rows/5-1)*4,thresholdImage.cols/2-1,thresholdImage.rows/5-1));
		imshow("【局部图0】", imageROI[0]); 
		imshow("【局部图1】", imageROI[1]); 
		imshow("【局部图2】", imageROI[2]); 
		imshow("【局部图3】", imageROI[3]); 
		imshow("【局部图4】", imageROI[4]); 
		imshow("【局部图5】", imageROI[5]); 
		imshow("【局部图6】", imageROI[6]); 
		imshow("【局部图7】", imageROI[7]); 
		imshow("【局部图8】", imageROI[8]); 
		imshow("【局部图9】", imageROI[9]);
	}

	//-----------------------------------【modeling()函数】----------------------------------  
	//      描述：根据行提取线与投影规则建立纸面模型
	//		输入：输入点集（行提取线）、纸张所占行数、水平基准线所在行数、摄影高度、纸张高度
	//		输出：输出点集：x为输入点集的x，y为模型高度（加位置偏置）
	//----------------------------------------------------------------------------------------------
	static vector<vector<Point3f>> modeling(const vector<vector<Point3f>> &input, int pagerows, int linerows, float height, float pagehight)
	{
		float angle;//行与摄像机的连线与垂直投影线的夹角
		vector<vector<Point3f>> output(input);//输出点集
		for (int i=0;i<input.size();i++)//每个提取行
		{
			angle = -atan((pagehight / pagerows) * (input[i][0].y - linerows) / height );//计算行的角度
				//cout<<"angle"<<i<<":"<<angle<<endl;

			for (int j=0;j<input[i].size();j++)//每行的元素
			{
				//input[i][j].y = (input[i][j].y - input[i][0].y) / sin(angle);//计算行的实际形状，不加偏置
				output[i][j].y = (input[i][j].y - input[i][0].y) / sin(angle) + input[i][0].y;//计算行的实际形状，加上偏置
			}
		}
		return output;
	}

	//-----------------------------------【lengthCalculation()函数】----------------------------------  
	//      描述：计算输入点的长度信息并返回
	//		输入：无长度点集、模型信息、长度基准行
	//		输出：有长度点集
	//---------------------------------------------------------------------------------------------- 
	static vector<vector<Point3f>> lengthCalculation(const vector<vector<Point3f>> &input, const vector<vector<Point3f>> &model,int baseline)
	{
		vector<vector<Point3f>> output(input);//输出点集

		//起始点的长度定义为点所在的列数
		for (int i=0;i<input.size();i++)//每个提取行
		{
			output[i][0].z = input[i][0].x;
		}

		//其他点的长度为慢慢累积所得
		for (int i=0;i<input.size();i++)//每个提取行
		{
			for (int j=1;j<input[i].size();j++)//每行的元素
			{
				if (j < input[baseline-1].size())//以baseline为参照
				{
					//以baseline为参照
					float dx = model[baseline-1][j].x - model[baseline-1][j-1].x;
					float dy = model[baseline-1][j].y - model[baseline-1][j-1].y;
					output[i][j].z = output[i][j-1].z + sqrt(pow(dx,float(2))+pow(dy,float(2)));
				} 
				else
				{
					output[i][j].z = output[i][j-1].z+1;
				}
			}
		}
		return output;
	}
	//-----------------------------------【paperCorrection()函数】----------------------------------  
	//      描述：矫正图像
	//		输入：矫正源图像、空白输出图像、矫正信息（含长度的行提取点集）
	//		输出：矫正的图像
	//---------------------------------------------------------------------------------------------- 
	static void paperCorrection(const Mat &inputImage, Mat &outputImage, const vector<vector<Point3f>> &massage)
	{
		int ref_cols = massage[0].size()/5;//参考列（最后一列1/5长度处的y）
		int height0 = (massage[0][0].y - massage[massage.size()-1][0].y) / (massage.size()-1);//行平均高度
		int thisx;//本行的参考列
		int nextx;//下一行的参考列
		int height ;//参照同一个行中心线的块高度
		int bias;//调整行对应的窗口y方向上的位置

		//其余行
		for (int i = massage.size()-1;i >= 0;i--)//参照哪个提取行（从上往下）
		{
			//保持高度信息为同一参考列
			thisx = massage[i][0].x - massage[0][0].x + ref_cols;
			if (thisx < 0)
			{
				thisx = 0;
			}
			//计算块高度
			if (i == massage.size()-1)//第一块的高度为平均高度的两倍
			{
				height = 2 * height0;
			}
			else//其他块高度为上一行参考列的y减去本行参考列的y
			{
				//height = massage[i][0].y - massage[i+1][0].y;
				//保持高度信息为同一参考列
				nextx = massage[i+1][0].x - massage[0][0].x + ref_cols;
				if (nextx < 0)
				{
					nextx = 0;
				}
				height = massage[i][thisx].y - massage[i+1][nextx].y;
			}
			//调整行对应的窗口y方向上的位置
			if (height >1.5 * height0)//默认与行平均高度成比例
			{
				bias = height0 * 0.2;
			} 
			else//一般与本块平均高度成比例
			{
				bias = height * 0.2;
			}

			int j =0;//从提取行第一列开始计算
			for (int io = massage[i][0].x;io < massage[i][massage[i].size()-2].z;io++)//块中的列计数
			{
				for (int jo = massage[i][thisx].y, height_count = 0;height_count < height;jo--,height_count++)//块中的行计数
				{
					for (;massage[i][j+1].z < io;j++){}//找到可以插值的点
					int k = (io - massage[i][j].z) / (massage[i][j+1].z - massage[i][j].z);//插值系数
					uchar B,G,R;//矫正
					Point	targetPoint1 = Point(massage[i][j].x,massage[i][j].y - height_count + bias),
							targetPoint2 = Point(massage[i][j+1].x,massage[i][j+1].y - height_count + bias);//插值两端的点坐标
					if (targetPoint1.inside(Rect(0,0,inputImage.cols,inputImage.rows)) && targetPoint2.inside(Rect(0,0,inputImage.cols,inputImage.rows)))
					{//防止所需点超过输入图像范围
						B = k * inputImage.at<Vec3b>(targetPoint1)[0] + (1 - k)*inputImage.at<Vec3b>(targetPoint2)[0];
						G = k * inputImage.at<Vec3b>(targetPoint1)[1] + (1 - k)*inputImage.at<Vec3b>(targetPoint2)[1];
						R = k * inputImage.at<Vec3b>(targetPoint1)[2] + (1 - k)*inputImage.at<Vec3b>(targetPoint2)[2];
						outputImage.at<Vec3b>(jo,io) = Vec3b(B,G,R);
					}
				}
			}
		}

		//增加末行，防止末行太短
		int i = 0;//末行
		int j =0;//从提取行第一列开始计算
		height = (massage[0][0].y - massage[massage.size()-1][0].y) / (massage.size()-1);//参照同一个行中心线的块高度，最后一块的高度为平均高度
		thisx = massage[i][0].x - massage[0][0].x + ref_cols;
		bias = height*0.2;//调整行对应的窗口y方向上的位置
		for (int io = massage[i][0].x;io < massage[i][massage[i].size()-2].z;io++)//块中的列计数
		{
			for (int jo = massage[i][thisx].y, height_count = 0;height_count < height;jo++,height_count++)//块中的行计数
			{
				for (;massage[i][j+1].z < io;j++){}//找到可以插值的点
				int k = (io - massage[i][j].z) / (massage[i][j+1].z - massage[i][j].z);//插值系数
				uchar B,G,R;//矫正
				Point	targetPoint1 = Point(massage[i][j].x,massage[i][j].y + height_count + bias),
					targetPoint2 = Point(massage[i][j+1].x,massage[i][j+1].y + height_count + bias);//插值两端的点坐标
				if (targetPoint1.inside(Rect(0,0,inputImage.cols,inputImage.rows)) && targetPoint2.inside(Rect(0,0,inputImage.cols,inputImage.rows)))
				{//防止所需点超过输入图像范围
					B = k * inputImage.at<Vec3b>(targetPoint1)[0] + (1 - k)*inputImage.at<Vec3b>(targetPoint2)[0];
					G = k * inputImage.at<Vec3b>(targetPoint1)[1] + (1 - k)*inputImage.at<Vec3b>(targetPoint2)[1];
					R = k * inputImage.at<Vec3b>(targetPoint1)[2] + (1 - k)*inputImage.at<Vec3b>(targetPoint2)[2];
					outputImage.at<Vec3b>(jo,io) = Vec3b(B,G,R);
				}
			}
		}
	}
	//-----------------------------------【smooth()函数】----------------------------------  
	//      描述：点集滤波
	//		输入：未滤波点集，均值滤波长度
	//		输出：滤波后点集
	//---------------------------------------------------------------------------------------------- 
	static vector<vector<Point3f>> smooth(const vector<vector<Point3f>> &src,int range)
	{
		vector<vector<Point3f>> dst(src);//输出点集
		int sum = 0,//均值范围y总和
			average = 0;//均值范围y平均

		for (int i = 0; i < src.size(); i++)//提取每一行
		{
			//range/2~src[i].size()-range/2个点，取范围为rang的均值
			for (int j = 0; j <= src[i].size() - range; j++)//j为累加起始点
			{
				for (int k = 0; k < range; k++)//rang范围内累加
				{
					sum += src[i][j + k].y;
				}
				average = sum / range;
				dst[i][j + range/2].y = average;//j+range/2为被赋值点
				sum = 0;
			}
			//0~range/2-1个点
			for (int j = 0;j < range/2;j++)//j为被赋值点
			{
				for (int k = 0;k < 2*j + 1;k++)//累加范围为0~2j
				{
					sum += src[i][k].y;
				}
				average = sum / (2*j + 1);
				dst[i][j].y = average;
				sum = 0;
			}
			//range/2+1~src[i].size()-1个点
			for (int j = src[i].size() - range + range/2 + 1;j < src[i].size();j++)//j为被赋值点
			{
				for (int k = 2*j - src[i].size() + 1;k < src[i].size();k++)//累加范围为2*j-src[i].size()+1~src[i].size()-1
				{
					sum += src[i][k].y;
				}
				average = sum / (2*src[i].size() - 2*j - 1);
				dst[i][j].y = average;
				sum = 0;
			}
		}
		return dst;

	}
	//-----------------------------------【showAndSaveImage()函数】----------------------------------  
	//      描述：显示与输出图像
	//		输入：图像，名称，储存位置
	//		输出：无
	//---------------------------------------------------------------------------------------------- 
	static void  showAndSaveImage(const Mat &inputImage,string nameI,string placeI)
	{
		namedWindow(nameI,WINDOW_NORMAL);//显示原始图像
		imshow(nameI, inputImage);  
		imwrite(placeI+nameI+".jpg",inputImage);
	}

	//-----------------------------------【resizeMassage()函数】----------------------------------  
	//      描述：改变模型信息的尺寸
	//		输入：原始点集，缩放比例
	//		输出：改变比例的点集
	//---------------------------------------------------------------------------------------------- 
	static vector<vector<Point3f>>  resizeMassage(const vector<vector<Point3f>> &src,float proportion)
	{
		vector<vector<Point3f>> dst;//输出点集
		int outputSize = (src[0].size()-1)*proportion+1;//输出点集长度

		for (int i = 0;i<src.size();i++)//i:行计数
		{
			vector<Point3f> thisline;//此行
			for (int jout =0;jout<outputSize;jout++ )//j:输入点集列计数
			{
				
				int xout,yout;//当前处理的输出点的xy（x-j，y-i）
				float zout;//当前处理的输出点的长度信息

				if (jout == 0)//第0点信息计算
				{
					yout = src[i][0].y * proportion;
					xout = src[i][0].x * proportion;
					zout = src[i][0].z * proportion;
				} 
				else//第0点以外的信息计算
				{
					if ( (jout/proportion) == int(jout/proportion))
						//如果输出点对应整数下标输出点的话，不差值。防止最后点刚好为整数下标时出错。
					{
						yout = src[i][int(jout/proportion)].y * proportion;
						xout = src[i][int(jout/proportion)].x * proportion;
						zout = src[i][int(jout/proportion)].z * proportion;
					} 
					else//输出点对应非整数下标输出点的话，差值。
					{
						float jin = jout/proportion;//对应原图上的下标
						int jin0 = jin,jin1 = jin+1;
						float k0 = jin1-jin;
						float k1 = jin - jin0;

						yout = (k0*src[i][jin0].y + k1*src[i][jin1].y) * proportion;
						xout = (k0*src[i][jin0].x + k1*src[i][jin1].x) * proportion;
						zout = (k0*src[i][jin0].z + k1*src[i][jin1].z) * proportion;

					}
				}
				thisline.push_back(Point3f(xout,yout,zout));//推入此点
			}
			dst.push_back(thisline);//推入本行
		}
		return dst;
	}



};
#endif