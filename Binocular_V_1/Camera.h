#pragma once
//纯相机类 用于相机的唯一标识 参数设置 以及信息通讯
#include "PvApi.h"

#include <string>
//在主函数里 可以弹出窗口 用来显示 显示相关的函数 放在显示的DLG中
//也可以直接执行相机类的方法，不显示图像，只显示相机的基本信息

#define MAX_FRAMES 100 

enum AttrType
{
	Enum = 1,
	Uint32 = 2,
	Float32 = 3,
	Int64 = 4,
	Bool = 5,
	String = 6
};

class Camera
{
//其所有信息都是开放的
public:
	//相机固定参数 
	tPvHandle H_Camera;		//相机句柄				获得
	tPvUint32 IpAddr;		//IP地址				输入
	tPvUint32 Width;		//相机宽度				获得
	tPvUint32 Height;		//相机高度				获得
	tPvUint32 BytesPerFrame; //单帧字节数			获得
	std::string CameraName;	//相机名称				输入
	tPvUint32 MaxSize;		//相机数据包大小        获得  当相机能采集到图像 但是都是黑的时候 可以看这个

	tPvFrame* pFrames;		//相机帧数据

	//状态变量
	tPvUint32 Expose;		//相机曝光值			获得
	int FrameRate;			//采集帧率				输入/初始化为30
	bool isStreaming;		//是否正在采集			初始为否
	int TrigerMode;			//触发模式				0 为自由模式 1硬件中断1 2硬件中断通道2 3为软件中断 初始为0;

	unsigned int FrameCount;	//采图数量计数			初始为0 调用Save方法的时候+1
	std::string outLog;		//错误输出
	//标准构造函数和析构函数
	//构造函数里要包含构造申请
	//析构函数里要有对应的空间释放
	Camera();
	~Camera();		
	Camera(tPvUint32 ip_addr, std::string cam_name);	//用网口地址创建一个相机，并设定默认的参数

	//基本的方法
	bool AttrSet(AttrType attrtype, const char* name, const char* value);			//单独设定各种参数 从value中获取 bool时输入0 1
	bool AttrGet(AttrType attrtype, const char* name, char* value);			//单独获取各种参数 存在value中 bool存入0 1
	bool Open();			//打开相机
	bool Close();			//关闭相机
	bool StartCapture();	//开始采集图像
	bool CloseCapture();	//停止采集图像
	bool ChangeTrigerMode(int TrrigerMode);			//改变触发模式
	bool ChangeExposeValue(unsigned long evalue);	//改变曝光值
};

//相机回调函数的函数指针（void*) 此函数将pFrame作为输入 pFrame有内置参数，可以确定是哪一个相机拍摄的图像
void __stdcall FrameDoneCB(tPvFrame* pFrame);