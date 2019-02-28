#pragma once
//������� ���������Ψһ��ʶ �������� �Լ���ϢͨѶ
#include "PvApi.h"

#include <string>
//���������� ���Ե������� ������ʾ ��ʾ��صĺ��� ������ʾ��DLG��
//Ҳ����ֱ��ִ�������ķ���������ʾͼ��ֻ��ʾ����Ļ�����Ϣ

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
//��������Ϣ���ǿ��ŵ�
public:
	//����̶����� 
	tPvHandle H_Camera;		//������				���
	tPvUint32 IpAddr;		//IP��ַ				����
	tPvUint32 Width;		//�������				���
	tPvUint32 Height;		//����߶�				���
	tPvUint32 BytesPerFrame; //��֡�ֽ���			���
	std::string CameraName;	//�������				����
	tPvUint32 MaxSize;		//������ݰ���С        ���  ������ܲɼ���ͼ�� ���Ƕ��Ǻڵ�ʱ�� ���Կ����

	tPvFrame* pFrames;		//���֡����

	//״̬����
	tPvUint32 Expose;		//����ع�ֵ			���
	int FrameRate;			//�ɼ�֡��				����/��ʼ��Ϊ30
	bool isStreaming;		//�Ƿ����ڲɼ�			��ʼΪ��
	int TrigerMode;			//����ģʽ				0 Ϊ����ģʽ 1Ӳ���ж�1 2Ӳ���ж�ͨ��2 3Ϊ�����ж� ��ʼΪ0;

	unsigned int FrameCount;	//��ͼ��������			��ʼΪ0 ����Save������ʱ��+1
	std::string outLog;		//�������
	//��׼���캯������������
	//���캯����Ҫ������������
	//����������Ҫ�ж�Ӧ�Ŀռ��ͷ�
	Camera();
	~Camera();		
	Camera(tPvUint32 ip_addr, std::string cam_name);	//�����ڵ�ַ����һ����������趨Ĭ�ϵĲ���

	//�����ķ���
	bool AttrSet(AttrType attrtype, const char* name, const char* value);			//�����趨���ֲ��� ��value�л�ȡ boolʱ����0 1
	bool AttrGet(AttrType attrtype, const char* name, char* value);			//������ȡ���ֲ��� ����value�� bool����0 1
	bool Open();			//�����
	bool Close();			//�ر����
	bool StartCapture();	//��ʼ�ɼ�ͼ��
	bool CloseCapture();	//ֹͣ�ɼ�ͼ��
	bool ChangeTrigerMode(int TrrigerMode);			//�ı䴥��ģʽ
	bool ChangeExposeValue(unsigned long evalue);	//�ı��ع�ֵ
};

//����ص������ĺ���ָ�루void*) �˺�����pFrame��Ϊ���� pFrame�����ò���������ȷ������һ����������ͼ��
void __stdcall FrameDoneCB(tPvFrame* pFrame);