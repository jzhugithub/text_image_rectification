/*线性校正与行非线性校正
程序说明：输入图片后，缩小图片进行处理，再恢复信息进行矫正
日期：2016.4.29

变量代号：
_o:原始图像衍生
_e:经过加粗的图像衍生
_r:旋转图像衍生
_h:水平调整图像衍生
_s:经过行滤波点集衍生
_c:去除两端干扰段点集衍生
_i:经过插值的点集衍生


基本RGB通道颜色：
Scalar(255,0,0)		：蓝色
Scalar(0,255,0)		：绿色
Scalar(0,0,255)		：红色
Scalar(0,255,255)	：黄色
Scalar(255,0,255)	：粉色
Scalar(255,255,0)	：淡蓝
Scalar(255,255,255)	：白色

自定义数据类型说明：
vector<vector<Point3f>> RES;
*      RES为2维容器，RES.size()返回行数，RES[i].size()返回每行点数目
*      RES[i][j]为第i行第j个点，数据类型Point,可用 RES[i][j].x、  
*      RES[i][j].y 、RES[i][j].z获得x,y坐标以及自定义长度z。
*      一般，RES内数据均按顺序排列：行为倒序，列为顺序

*/
#include <cmath>
#include <opencv2/opencv.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include "zjclass.h"
#include "zjmethod.h"
#include "jlclass.h"

using namespace cv;
using namespace std;

//-----------------------------------【部分参数声明部分】---------------------------------------
//		描述：包含程序所使用的部分参数
//-----------------------------------------------------------------------------------------------
#define figureName "../0image_and_header/1.jpg"//处理图像的名称
#define targetHeight 800.0//处理图像的高度//修改
#define Kpaperheight 0.8//纸张高度与图片高度的比例
#define CameraHeight 0.3//摄影高度（m）
#define PaperHeight 0.25//纸张高度（m）
#define BaselineNumber 4//行长度计算基准行
#define max(a,b) (((a)>(b))?(a):(b))//修改

//-----------------------------------【main( )函数】--------------------------------------------
//		描述：控制台应用程序的入口函数，我们的程序从这里开始
//-----------------------------------------------------------------------------------------------
int main( )
{
	//-----------------------------------【线性校正部分】--------------------------------------------
	//		描述：对原图进行线性校正
	//-----------------------------------------------------------------------------------------------

	//【1】变量定义
	Mat origImage_o,		//原始图像
		resizeImage_o,		//改变尺寸的图像//fixed5.2
		grayImage_o,		//灰度图
		thresholdImage_o,	//二值图
		medianBlurImage_o,	//中值滤波图像
		medianBlurImage_o_e,//扩展后中值滤波图像
		lineateImage_o,		//旋转前效果图（含轴线）
		rotatedImage_r,		//旋转后效果图
		rotatedImage_r_o,	//旋转后的原图//fixed5.2
		medianBlurImage_r;	//旋转与中值滤波后二值图
	float resizeProportion;//图像改变比例//fixed5.2
	int i;				//公用计数变量
	int blockSize;		//局部二值化阈值计算窗口边长，必须为奇数
	int ksize;//中值滤波窗口边长，必须为奇数
	vector<Vec4i> vlines;//为了找轴线，定义一个矢量结构lines用于存放得到的线段矢量集合(列，行，列，行)
	VerBaseLine vbasel;	//寻找垂直轴线段的类
	Vec4i	vlin,			//resize图片的垂直轴线段
			vlin_o;			//原始图片的垂直轴线段//fixed5.2
	RotateImage rotatedIm,	//图像旋转类
				rotatedIm_o,//原始图像旋转类//fixed5.2
				rotatedIm_t;//二值图像旋转类
	int disp_flag1;		//【线性校正部分】显示与输出标志

	//【2】载入原始图
	origImage_o = imread(figureName);
	if(!origImage_o.data ) { cout<<"读取图片错误，请确定目录下是否有imread函数指定的图片存在~"<<endl;getchar(); return false; }

	//【2.1】改变图像大小//fixed5.2
	resizeProportion = targetHeight/origImage_o.rows;//图像改变比例

	Size outputSize = Size(origImage_o.cols*resizeProportion,targetHeight);
	resize(origImage_o, resizeImage_o, outputSize);//fixed5.2

	cout<<"图片列数："<<resizeImage_o.cols<<endl;//2445
	cout<<"图片行数："<<resizeImage_o.rows<<endl;//3264

	//【3】转化为灰度图、均衡化
	cvtColor( resizeImage_o, grayImage_o, CV_BGR2GRAY );//转换为灰度图
	equalizeHist( grayImage_o, grayImage_o );//灰度图均衡化

	//【4】进行局部二值化
	blockSize = (grayImage_o.cols+grayImage_o.rows)*0.5*0.05;//阈值计算窗口边长，必须为奇数
	if (blockSize%2==0){blockSize++;}
		//cout<<"blockSize:"<<blockSize<<endl;
	adaptiveThreshold(grayImage_o,thresholdImage_o,255,CV_ADAPTIVE_THRESH_MEAN_C,THRESH_BINARY_INV, blockSize, 20);
		/*	参数5
			For the method ADAPTIVE_THRESH_MEAN_C , the threshold value T(x,y) is a mean of the neighborhood of (x, y) minus C .
			For the method ADAPTIVE_THRESH_GAUSSIAN_C , the threshold value T(x, y) is a weighted sum (cross-correlation with a 
			Gaussian window) of the neighborhood of (x, y) minus C . The default sigma (standard deviation) is used for the specified blockSize . */

	//【5】进行中值滤波（去除椒盐噪声）
	ksize = (thresholdImage_o.cols+thresholdImage_o.rows)*0.5/800;//滤波窗口边长，必须为奇数
	if (ksize%2==0){ksize++;}
	if (ksize<1){ksize = 1;}//修改
	if (ksize>7){ksize = 7;}
	//cout<<"ksize:"<<ksize<<endl;
	if (ksize>1)//修改
	{
		medianBlur(thresholdImage_o,medianBlurImage_o,ksize);
	} 
	else
	{
		thresholdImage_o.copyTo(medianBlurImage_o);
	}
	

	//【6】进行霍夫线变换与二值图像扩展
	i=0;//清零公用计数变量
	medianBlurImage_o.copyTo(medianBlurImage_o_e);
	do 
	{
		//修改 参数3
		HoughLinesP(medianBlurImage_o_e, vlines, medianBlurImage_o_e.rows*0.02, CV_PI/360, medianBlurImage_o_e.rows*0.1, medianBlurImage_o_e.rows*0.3, medianBlurImage_o_e.rows*0.006 );//进行霍夫变换
		/*	第五个参数，Accumulator threshold parameter. Only those lines are returned that get enough votes (  ).
			第六个参数，double类型的minLineLength，有默认值0，表示最低线段的长度，比这个设定参数短的线段就不能被显现出来。
			第七个参数，double类型的maxLineGap，有默认值0，允许将同一行点与点之间连接起来的最大的距离*/
		if (!vlines.data() && i<5)
		{
			medianBlurImage_o_e = ZjMethod::expandLine(medianBlurImage_o_e);//如果没有找到合适的轴线，就扩展二值图像
			i++;
		}
	} while (!vlines.data() && i<5);
	if (i==5)
	{
		cout<<"二值图像扩展无效，没有找到轴线"<<endl;
		getchar();
		return -1;
	}
		//cout<<"二值图像扩展次数："<<i<<endl;

	//【7】依次在图中绘制出每条线段
	resizeImage_o.copyTo(lineateImage_o);//将缩放图复制给效果图
	for( size_t i = 0; i < vlines.size(); i++ )
	{
		Vec4i l = vlines[i];
		line( lineateImage_o, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0,255,255), 2, CV_AA);//point:(列，行)
	}

	//【8】找出轴线
	vbasel = VerBaseLine(vlines,lineateImage_o);//初始化寻找基准线段的类
	vbasel.drawOnImage(Scalar(0,0,255));//在图中画出基准线段

	//【9】对*原图与二值图*进行仿射变换
	vlin = vbasel.baseSegment();//找到原始图片的垂直轴线段
	rotatedIm = RotateImage(resizeImage_o,vlin);//定义旋转类
	rotatedIm_t = RotateImage(medianBlurImage_o,vlin);//定义旋转类
	rotatedImage_r = rotatedIm.trans1();//作旋转变换
	medianBlurImage_r = rotatedIm_t.trans1();//作旋转变换

	//---------------------原图旋转------------//fixed5.2
	vlin_o = Vec4i(vlin[0]/resizeProportion,vlin[1]/resizeProportion,vlin[2]/resizeProportion,vlin[3]/resizeProportion);
	rotatedIm_o = RotateImage(origImage_o,vlin_o);
	rotatedImage_r_o = rotatedIm_o.trans1();
	
	//【10】显示图像
	cout<<"是否显示与输出【线性校正部分】（1是,0否）:";
	cin>>disp_flag1;
	if (disp_flag1 ==1)
	{
		ZjMethod::showAndSaveImage(resizeImage_o,"【1原始图】","../0image_and_header/");//显示原始图像
		ZjMethod::showAndSaveImage(grayImage_o,"【1灰度图】","../0image_and_header/");//显示直方图均衡化后的灰度图像
		ZjMethod::showAndSaveImage(thresholdImage_o,"【1局部二值化图像】","../0image_and_header/");//显示局部二值化后的图像
		ZjMethod::showAndSaveImage(medianBlurImage_o,"【1中值滤波图像】","../0image_and_header/");//显示中值滤波图像
		ZjMethod::showAndSaveImage(lineateImage_o,"【1旋转前效果图】","../0image_and_header/");//显示旋转前效果图
		ZjMethod::showAndSaveImage(rotatedImage_r,"【1旋转后效果图】","../0image_and_header/");//显示最终效果图
		ZjMethod::showAndSaveImage(medianBlurImage_r,"【1旋转后二值图】","../0image_and_header/");//显示最终二值效果图

	}

	cout<<"正在继续，请稍等"<<endl;
	waitKey(0);

	//-----------------------------------【行非线性校正部分-细化图像】----------------------------------------
	//		描述：从线性校正后的图像中得到细化图像
	//-----------------------------------------------------------------------------------------------

	//【1】旋转后变量定义
	Mat floodFillImage_r,			//去除轴线后的二值图像
		morphologyImage_r,			//形态学滤波图像
		thinImage_r,				//细化二值图像
		rotatedWithThinImage_r;		//细化彩色图像
	float unit;//形态学滤波与图片高度相关联的单位长度//修改
	int disp_flag2;//【非线性校正-细化图像部分】显示与输出标志

	//【2】去除二值图中的轴线
	medianBlurImage_r.copyTo(floodFillImage_r);
	floodFill(floodFillImage_r, rotatedIm.center, Scalar(0));
	/*	第一个参数，InputOutputArray类型的image, 输入/输出1通道或3通道，8位或浮点图像，具体参数由之后的参数具体指明。
		第二个参数， InputOutputArray类型的mask，这是第二个版本的floodFill独享的参数，表示操作掩模,。它应该为单通道、8位、长和宽上都比输入图像 image 大两个像素点的图像。第二个版本的floodFill需要使用以及更新掩膜，所以这个mask参数我们一定要将其准备好并填在此处。需要注意的是，漫水填充不会填充掩膜mask的非零像素区域。例如，一个边缘检测算子的输出可以用来作为掩膜，以防止填充到边缘。同样的，也可以在多次的函数调用中使用同一个掩膜，以保证填充的区域不会重叠。另外需要注意的是，掩膜mask会比需填充的图像大，所以 mask 中与输入图像(x,y)像素点相对应的点的坐标为(x+1,y+1)。
		第三个参数，Point类型的seedPoint，漫水填充算法的起始点。
		第四个参数，Scalar类型的newVal，像素点被染色的值，即在重绘区域像素的新值。
		第五个参数，Rect*类型的rect，有默认值0，一个可选的参数，用于设置floodFill函数将要重绘区域的最小边界矩形区域。
		第六个参数，Scalar类型的loDiff，有默认值Scalar( )，表示当前观察像素值与其部件邻域像素值或者待加入该部件的种子像素之间的亮度或颜色之负差（lower brightness/color difference）的最大值。 
		第七个参数，Scalar类型的upDiff，有默认值Scalar( )，表示当前观察像素值与其部件邻域像素值或者待加入该部件的种子像素之间的亮度或颜色之正差（lower brightness/color difference）的最大值。
		第八个参数，int类型的flags，操作标志符，此参数包含三个部分，比较复杂，我们一起详细看看。

		低八位（第0~7位）用于控制算法的连通性，可取4 (4为缺省值) 或者 8。如果设为4，表示填充算法只考虑当前像素水平方向和垂直方向的相邻点；如果设为 8，除上述相邻点外，还会包含对角线方向的相邻点。
		高八位部分（16~23位）可以为0 或者如下两种选项标识符的组合：     

		FLOODFILL_FIXED_RANGE - 如果设置为这个标识符的话，就会考虑当前像素与种子像素之间的差，否则就考虑当前像素与其相邻像素的差。也就是说，这个范围是浮动的。
		FLOODFILL_MASK_ONLY - 如果设置为这个标识符的话，函数不会去填充改变原始图像 (也就是忽略第三个参数newVal), 而是去填充掩模图像（mask）。这个标识符只对第二个版本的floodFill有用，因第一个版本里面压根就没有mask参数。

		中间八位部分，上面关于高八位FLOODFILL_MASK_ONLY标识符中已经说的很明显，需要输入符合要求的掩码。Floodfill的flags参数的中间八位的值就是用于指定填充掩码图像的值的。但如果flags中间八位的值为0，则掩码会用1来填充。
		而所有flags可以用or操作符连接起来，即“|”。例如，如果想用8邻域填充，并填充固定像素值范围，填充掩码而不是填充源图像，以及设填充值为38，那么输入的参数是这样：
		*/

	//【3】进行形态学滤波
	unit = floodFillImage_r.rows*0.00025;//与图片高度相关联的单位长度//修改



		//cout<<"unit:"<<unit<<endl;
	dilate(floodFillImage_r,morphologyImage_r,getStructuringElement(MORPH_RECT, Size(max(70*unit,1), max(2*unit,1))));//膨胀//修改
	erode(morphologyImage_r,morphologyImage_r,getStructuringElement(MORPH_RECT, Size(max(1*unit,1), max(7*unit,1))));//腐蚀//修改

	//【4】进行细化
	ZjMethod::thin(morphologyImage_r,thinImage_r,max(100*unit,1));//修改

	//【5】显示细化图像
	rotatedImage_r.copyTo(rotatedWithThinImage_r);
	jlmethod::showline(rotatedWithThinImage_r,thinImage_r,Scalar(255,0,0));//将细化图像画到旋转后的图像上

	cout<<"是否显示与输出【非线性校正-细化图像部分】（1是,0否）:";
	cin>>disp_flag2;
	if (disp_flag2 ==1)
	{
		ZjMethod::showAndSaveImage(floodFillImage_r,"【2去除轴线后的二值图像】","../0image_and_header/");//显示去除轴线后的二值图像
		ZjMethod::showAndSaveImage(morphologyImage_r,"【2形态学滤波图像】","../0image_and_header/");//显示形态学滤波图像
		ZjMethod::showAndSaveImage(thinImage_r,"【2细化二值图像】","../0image_and_header/");//显示形态学滤波图像
		ZjMethod::showAndSaveImage(rotatedWithThinImage_r,"【2细化彩色图像】","../0image_and_header/");//显示细化彩色图像
	}
	cout<<"正在继续，请稍等"<<endl;
	waitKey(0);

	//-----------------------------------【行非线性校正部分-行提取】----------------------------------------
	//		描述：从细化的图像中提取行中心线
	//-----------------------------------------------------------------------------------------------

	//【1】行提取变量定义
	Mat lineres_r,				//行提取二值图像
		rotatedWithLineImage_r;	//行提取彩色图像
		
	vector<vector<Point3f>> RES_r;//旋转图像行中心线
	int disp_flag3;//【非线性校正-行提取部分】显示与输出标志

	//【2】行中心线提取
	RES_r=jlmethod::findline(thinImage_r);

	//【3】显示行提取图像
	lineres_r = Mat(thinImage_r.rows,thinImage_r.cols,CV_8U,Scalar(0));//创建空白图像
	jlmethod::showline(lineres_r,RES_r,255);//将行提取结果画到空白图像上

	rotatedWithThinImage_r.copyTo(rotatedWithLineImage_r);
	jlmethod::showline(rotatedWithLineImage_r,RES_r,Scalar(0,0,255));//将提取的行画到旋转后的图像上

	cout<<"是否显示与输出【非线性校正-行提取部分】（1是,0否）:";
	cin>>disp_flag3;
	if (disp_flag3 ==1)
	{
		ZjMethod::showAndSaveImage(lineres_r,"【3行提取二值图像】","../0image_and_header/");//显示行提取图像
		ZjMethod::showAndSaveImage(rotatedWithLineImage_r,"【3行提取彩色图像】","../0image_and_header/");//显示形态学滤波图像
	}
	cout<<"正在继续，请稍等"<<endl;
	waitKey(0);

	//-----------------------------------【行非线性校正部分-寻找水平基准线】----------------------------------------
	//		描述：根据提取的行中心线寻找水平基准线
	//-----------------------------------------------------------------------------------------------
	//【1】变量定义
	Mat lineres_r_e,				//扩展后的行提取二值图像
		horizontalBaselineImage_r;	//水平基准线提取彩色图像
	vector<Vec4i> hlines;//为了找水平基准线，定义一个矢量结构lines2用于存放得到的线段矢量集合(列，行，列，行)
	Vec4i hlin;			//水平基准线段
	HorBaseLine hbasel;//寻找水平基准线的类
	int disp_flag4;//【非线性校正-寻找水平基准线】显示与输出标志

	//【2】对行提取二值图像进行霍夫线变换与提取线扩展
	i=0;//清零公用计数变量
	lineres_r.copyTo(lineres_r_e);
	do 
	{
		HoughLinesP(lineres_r_e, hlines, 1, CV_PI/720, lineres_r_e.cols*0.1, lineres_r_e.cols*0.3, lineres_r_e.cols*0.006 );//进行霍夫变换
		/*	第五个参数，Accumulator threshold parameter. Only those lines are returned that get enough votes (  ).
			第六个参数，double类型的minLineLength，有默认值0，表示最低线段的长度，比这个设定参数短的线段就不能被显现出来。
			第七个参数，double类型的maxLineGap，有默认值0，允许将同一行点与点之间连接起来的最大的距离*/
		if (!hlines.data() && i<5)
		{
			lineres_r_e = ZjMethod::expandLine(lineres_r_e);//如果没有找到合适的水平基准线，就扩展提取线
			i++;
		}
	} while (!hlines.data() && i<5);
	if (i==5)
	{
		cout<<"二值图像扩展无效，没有找到水平基准线"<<endl;
		getchar();
		return -1;
	}
		//cout<<"提取线扩展次数："<<i<<endl;

	//【3】依次在图中绘制出每条线段
	rotatedWithLineImage_r.copyTo(horizontalBaselineImage_r);;//将行提取彩色图复制给水平基准线图
	for( size_t i = 0; i < hlines.size(); i++ )
	{
		Vec4i l = hlines[i];
		line( horizontalBaselineImage_r, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(255,255,0), 2, CV_AA);//point:(列，行)
	}

	//【4】找出水平基准线
	hbasel = HorBaseLine(hlines,horizontalBaselineImage_r);//初始化寻找水平基准线的类
	hlin = hbasel.baseSegment();//找到旋转图像的水平基准线段
	hbasel.drawOnImage(Scalar(0,255,255));//在图中画出基准线段

	//【5】显示图像
	cout<<"是否显示与输出【非线性校正-寻找水平基准线与矫正】（1是,0否）:";
	cin>>disp_flag4;
	if (disp_flag4 ==1)
	{
		ZjMethod::showAndSaveImage(horizontalBaselineImage_r,"【4水平基准线提取彩色图像】","../0image_and_header/");//显示水平基准线提取彩色图像
	}
	cout<<"正在继续，请稍等"<<endl;
	waitKey(0);

	//-----------------------------------【行非线性校正部分-页面建模与矫正】----------------------------------------
	//		描述：根据提取的行中心线进行页面建模与矫正
	//-----------------------------------------------------------------------------------------------

	//【1】变量定义
	Mat modImage_r,			//旋转后的模型图像
		cutAndSmoothImage,	//去除行中心线两端干扰段并进行滤波后的图像
		interpolationImage,	//插值后图像
		correctedImage;		//矫正后所得图片
	vector<vector<Point3f>> RES_h_c,//去除行中心线两端干扰段的点集
							RES_h_c_s,//去除行中心线两端干扰段并结果滤波的点集
							RES_h_e,//扩展后的标准点集
							RES_h_i,//经过插值后的点集
							RES_mod_h,//包含模型信息的点集，参数说明:1.x；2.模型高度；3.空
							RES_mod_h_s,//包含模型信息、经过滤波的点集
							RES_leng_h;//包含长度学习的点集，参数说明:1.x；2.y；3.length
	int disp_flag5;//【非线性校正-页面建模与矫正】显示与输出标志
	int minX,maxX;//标准点集的端点x坐标


	//【2】去除行中心线两端干扰段
	RES_h_c = jlmethod::cutEnds(RES_r,lineres_r.cols * 0.02);

	//【3】去除干扰端后进行滤波
	RES_h_c_s = ZjMethod::smooth(RES_h_c,lineres_r.cols*0.02);

	//【4】扩展为标准的行中心线
	minX = RES_r[0][0].x;
	maxX = RES_r[0][RES_r[0].size()-1].x;
	for (int i = 1;i<RES_r.size();i++)
	{
		if (RES_r[i][0].x < minX)//找minX
		{
			minX = RES_r[i][0].x;
		}
		if (RES_r[i][RES_r[i].size()-1].x > maxX)//找maxX
		{
			maxX = RES_r[i][RES_r[i].size()-1].x;
		}
	}

	RES_h_e = jlmethod::extendEnds(RES_h_c_s,minX,maxX);
	
	//【5】中心线扩展端二次函数插值
	RES_h_i = jlmethod::lineInterpolation(RES_h_e,lineres_r.cols * 0.05);

	//【6】计算模型信息
	RES_mod_h = ZjMethod::modeling(RES_h_i,lineres_r.rows*Kpaperheight,hlin[1],CameraHeight,PaperHeight);
		//cout<<"RES_r.size() "<<RES_r.size()<<endl;
		//cout<<"lineres_r.rows*0.8 "<<lineres_r.rows*0.8<<endl;
		//cout<<"hlin[0]"<<hlin[0]<<endl;

	//【7】模型信息滤波
	RES_mod_h_s = ZjMethod::smooth(RES_mod_h,lineres_r.cols*0.06);//fixed5.2

	//【8】计算行长度
	RES_leng_h = ZjMethod::lengthCalculation(RES_h_i,RES_mod_h_s,BaselineNumber);
		//cout<<RES_leng_h[0]<<endl;


	//【8.1】恢复矫正信息------------------------------------------//fixed5.2
	vector<vector<Point3f>> RES_leng_h_ext;
	RES_leng_h_ext = ZjMethod::resizeMassage(RES_leng_h,1.0/resizeProportion);
	
	//【9】矫正页面
	correctedImage = Mat(rotatedImage_r_o.rows,rotatedImage_r_o.cols*1.5,rotatedImage_r_o.type(),Scalar(255,255,255));//创建空白图像
	ZjMethod::paperCorrection(rotatedImage_r_o,correctedImage,RES_leng_h_ext);

	//------------------------------------------------

	////【9】矫正页面
	//correctedImage = Mat(rotatedImage_r.rows,rotatedImage_r.cols*1.5,rotatedImage_r.type(),Scalar(255,255,255));//创建空白图像
	//ZjMethod::paperCorrection(rotatedImage_r,correctedImage,RES_leng_h);

	//【10】显示图像
	modImage_r = Mat(lineres_r.rows,lineres_r.cols,CV_8U,Scalar(0));//创建空白图像
	jlmethod::showline(modImage_r,RES_mod_h_s,255);//将行提取结果画到空白图像上

	rotatedImage_r.copyTo(cutAndSmoothImage);
	jlmethod::showline(cutAndSmoothImage,RES_h_c_s,Scalar(0,255,0));//去除行中心线两端干扰段并进行滤波后的图像

	cutAndSmoothImage.copyTo(interpolationImage);
	jlmethod::showline(interpolationImage,RES_h_i,Scalar(255,0,0));//经过插值后的图像

	cout<<"是否显示与输出【非线性校正-页面建模与矫正】（1是,0否）:";
	cin>>disp_flag5;
	if (disp_flag5 ==1)
	{
		ZjMethod::showAndSaveImage(modImage_r,"【5滤波后模型二值图像】","../0image_and_header/");//显示模型图像
		ZjMethod::showAndSaveImage(cutAndSmoothImage,"【5去除两端干扰段并滤波的图像】","../0image_and_header/");//显示去除两端干扰段并滤波的图像
		ZjMethod::showAndSaveImage(interpolationImage,"【5经过插值后的图像】","../0image_and_header/");//显示经过插值后的图像
		ZjMethod::showAndSaveImage(correctedImage,"【5矫正图像】","../0image_and_header/");//显示矫正后图像
	}
	cout<<"正在继续，请稍等"<<endl;
	waitKey(0);

	return 0;  
}
