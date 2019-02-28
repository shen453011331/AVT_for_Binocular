#include "stdafx.h"
#include "Camera.h"
#include <string>

using namespace std;
//��׼�Ŀհ׹��캯��
Camera::Camera()
{
	H_Camera = 0;
	IpAddr = 0;
	Width = 0;
	Height = 0;
	BytesPerFrame = 0;
	CameraName = "";
	MaxSize = 0;
	pFrames = NULL;
	Expose = 0;
	FrameRate = 30;
	isStreaming = false;
	TrigerMode = 0;
	FrameCount = 0;
	outLog = "Black data of Camera";
	//��ʼ��������ҪInitialized Protection
}


Camera::~Camera()
{
	if (isStreaming)
	{
		Close();
	}
}

Camera::Camera(tPvUint32 ip_addr, std::string cam_name)
{
	H_Camera = 0;
	IpAddr = ip_addr;
	Width = 0;
	Height = 0;
	BytesPerFrame = 0;
	CameraName = cam_name;
	MaxSize = 0;
	pFrames = NULL;
	Expose = 0;
	FrameRate = 30;
	isStreaming = false;
	TrigerMode = 0;
	FrameCount = 0;
	outLog = "Camera " + CameraName + " has been created.";
}

bool Camera::AttrSet(AttrType attrtype, const char* name, const char* value)
{
	if (!H_Camera)
	{
		outLog = "Error: Attribute of Camera Setting Error: Camera is not open.";
		return false;
	}
	else
	{
		tPvErr Err;
		switch (attrtype)
		{
		case Enum:
		{
			Err = PvAttrEnumSet(H_Camera, name, value);
			break;
		}
		case Uint32:
		{
			Err = PvAttrUint32Set(H_Camera, name, atoi(value));
			break;
		}
		case Float32:
		{
			Err = PvAttrFloat32Set(H_Camera, name, atof(value));
			break;
		}
		case Int64:
		{
			Err = PvAttrInt64Set(H_Camera, name, atol(value));
			break;
		}
		case Bool:
		{
			Err = PvAttrBooleanSet(H_Camera, name, atoi(value));
			break;
		}
		case String:
		{
			Err = PvAttrStringSet(H_Camera, name, value);
			break;
		}
		default:
			break;
		}
		if (Err != ePvErrSuccess)
		{
			outLog = "Warnning: Arrtibute of Camera Setting failed in Camera of value " + string(name) + " in camera " + CameraName;
		}
	}
}

bool Camera::AttrGet(AttrType attrtype, const char* name, char* value_s)
{
	//��������
	outLog = "";
	if (!H_Camera)
	{
		outLog = "Error: Attribute of Camera Getting Error: Camera is not open.";
		return false;
	}
	else
	{
		tPvErr Err;
		switch (attrtype)
		{
		case Enum:
		{
			unsigned long buffer_size;
			Err = PvAttrEnumGet(H_Camera, name, value_s,50,&buffer_size);
			if (buffer_size > 50)
			{
				outLog = "Warnning: to long of attr size of value " + string(name);
			}
			break;
		}
		case Uint32:
		{
			unsigned long value;
			Err = PvAttrUint32Get(H_Camera, name, &value);
			_ltoa(value, value_s, 10);
			break;
		}
		case Float32:
		{
			//float value;
			//Err = PvAttrFloat32Get(H_Camera, name, &value);
			break;
		}
		case Int64:
		{
			long long value;
			Err = PvAttrInt64Get(H_Camera, name, &value);
			_ltoa(value, value_s, 20);
			break;
		}
		case Bool:
		{
			unsigned char flag;
			Err = PvAttrBooleanGet(H_Camera, name, &flag);
			if (flag == 0)
			{
				value_s = new char;
				value_s = "0";
			}
			else
			{
				value_s = new char;
				value_s = "1";
			}
			break;
		}
		case String:
		{
			unsigned long buffer_size;
			Err = PvAttrStringGet(H_Camera, name, value_s, 50, &buffer_size);
			if (buffer_size > 50)
			{
				outLog = "Warnning: to long of attr size of value " + string(name);
			}
			break;
		}
		default:
			break;
		}

		if (Err != ePvErrSuccess)
		{
			outLog += "Warnning: Arrtibute of Camera Getting failed in Camera of value " + string(name) + " in camera " + CameraName;
			return false;
		}
		else
		{
			return true;
		}
	}
}

bool Camera::Open()
{
	tPvErr Err;
	Err = PvCameraOpenByAddr(IpAddr, ePvAccessMaster, &H_Camera);
	if (Err != ePvErrSuccess)
	{
		Err = PvCameraOpenByAddr(IpAddr, ePvAccessMaster, &H_Camera);
		if (Err != ePvErrSuccess)
		{
			outLog = "Error: Can't Open Camera "+CameraName+" with IP";
			return false;
		}
	}

	int ErrCode = 0;
	//��ȡ����Ļ������Բ���
	ErrCode |= PvAttrUint32Get(H_Camera, "Width", &Width);
	ErrCode |= PvAttrUint32Get(H_Camera, "Height", &Height);
	ErrCode |= PvAttrUint32Get(H_Camera, "PacketSize", &MaxSize);
	ErrCode |= PvAttrUint32Get(H_Camera, "ExposureValue", &Expose);

	//��������
	ErrCode |= PvAttrEnumSet(H_Camera, "ExposureMode", "Manual");				//�趨�ֶ��ع�ģʽ
	ErrCode |= PvAttrEnumSet(H_Camera, "PixelFormat", "Mono8");					//���ظ�ʽΪMono8
	ErrCode |= PvAttrEnumSet(H_Camera, "AcquisitionMode", "Continuous");		//�ɼ�ģʽΪ����
	
	ErrCode |= PvCaptureAdjustPacketSize(H_Camera, 8228);						//�趨�������ݴ�����
	if (ErrCode != 0)
	{
		outLog = "Warnning: Can't Get&Set Camera's Attribues in Camera" + CameraName;
		return false;
	}
	return true;
}

bool Camera::Close()
{
	if (isStreaming)
	{
		if (!CloseCapture())
		{
			return false;
		}
	}
	tPvErr Err;
	Err = PvCameraClose(H_Camera);
	if (Err != ePvErrSuccess)
	{
		outLog = "Warnning: Failed to close camera " + CameraName;
	}
	else
	{
		H_Camera = NULL;
		outLog = CameraName + " closed successful!";
	}
	return false;
}

bool Camera::StartCapture()
{
	if (isStreaming)
	{
		outLog = "Warnning: Camera: " + CameraName + " is Capturing now.";
		return false;
	}

	//���Ŵ������ȴ����
	if (H_Camera == NULL)
	{
		if (!Open())
		{
			return false;
		}
	}

	//��ʼ�м����
	tPvErr Err = ePvErrSuccess;
	int ErrCode = 0;
	int i, j;
	//����ռ�
	pFrames = new tPvFrame[MAX_FRAMES];
	//����ʹ��ZeroMemory
	ZeroMemory(pFrames, sizeof(tPvFrame)*MAX_FRAMES);

	//���ô���ģʽ �ڲ�ͬ������¸ı� ���в�ͬ���趨��ʽ ����ģʽѡ��Ӵ��ڻ��
	switch (TrigerMode)
	{
	case 0:
		ErrCode |= PvAttrEnumSet(H_Camera, "FrameStartTriggerMode", "Freerun");
		ErrCode |= PvAttrEnumSet(H_Camera, "FrameStartTriggerEvent", "EdgeFalling");//�½��ش���
		break;
	case 1:
		ErrCode |= PvAttrEnumSet(H_Camera, "FrameStartTriggerMode", "SyncIn1");
		ErrCode |= PvAttrEnumSet(H_Camera, "FrameStartTriggerEvent", "EdgeFalling");//�½��ش���
		break;
	case 2:
		ErrCode |= PvAttrEnumSet(H_Camera, "FrameStartTriggerMode", "SyncIn2");
		ErrCode |= PvAttrEnumSet(H_Camera, "FrameStartTriggerEvent", "EdgeFalling");//�½��ش���
		break;
	case 3:
		ErrCode |= PvAttrEnumSet(H_Camera, "FrameStartTriggerMode", "FixedRate");
		ErrCode |= PvAttrFloat32Set(H_Camera, "FrameRate", FrameRate);
		ErrCode |= PvAttrEnumSet(H_Camera, "FrameStartTriggerEvent", "EdgeFalling");//�½��ش���
		break;
	default:
		break;
	}
	if (ErrCode != ePvErrSuccess)
	{
		outLog = "Error: Attribute setting Problem in " + CameraName + " ErrorCode: " + to_string(ErrCode);
		delete[] pFrames;
		return false;
	}
	//��ʼ����ͼ��
	if (PvCaptureStart(H_Camera) == ePvErrSuccess)
	{
		//�ڴ����
		if (ePvErrSuccess == PvAttrUint32Get(H_Camera, "TotalBytesPerFrame", &BytesPerFrame) && BytesPerFrame)
		{
			for (i = 0; i < MAX_FRAMES; i++)
			{
				if (pFrames[i].ImageBufferSize != BytesPerFrame)
				{
					delete pFrames[i].ImageBuffer;
					pFrames[i].ImageBuffer = new BYTE[BytesPerFrame];
					pFrames[i].ImageBufferSize = BytesPerFrame;
				}
				if (!pFrames[i].ImageBuffer)
				{
					Err = ePvErrResources;
				}
				else
				{
					pFrames[i].Context[0] = this;				//���������ָ��
					pFrames[i].Context[1] = (void*)H_Camera;	//���������ʶ
				}
				if (Err != ePvErrSuccess)
				{
					for (j = 0; j < i; j++)
					{
						//���������ѷ��������ͼ�����ݻ���
						delete pFrames[j].ImageBuffer;
						pFrames[j].ImageBuffer = NULL;
					}
					PvCameraClose(H_Camera);
					H_Camera = NULL;
					outLog = "Error: No RAM for ImgBuffer of Camera " + CameraName;
					delete[] pFrames;
					return false;
				}
			}
		}
		else
		{
			outLog = "Error: BytesPerFrame cant get or it is 0 in " + CameraName;
			delete[] pFrames;
			return false;
		}

		//�ڴ������� ��������
		int ErrCode = 0;
		for (i = 0; i < MAX_FRAMES; i++)
		{
			ErrCode |= PvCaptureQueueFrame(H_Camera, &pFrames[i], FrameDoneCB);
		}

		//���й������ ��ʼ��ͼ
		if (ErrCode == ePvErrSuccess)
		{
			Err = PvCommandRun(H_Camera, "AcquisitionStart");//this Action will trigger one acquisition
			isStreaming = true;
			outLog = "Camera Open Successful in Camera:" + CameraName;
			return true;
		}
		else
		{
			PvCaptureQueueClear(H_Camera);
			PvCaptureEnd(H_Camera);
			H_Camera = NULL;
			outLog = "Error: Queue making Error in " + CameraName;
			for (i = 0; i < MAX_FRAMES; i++)
			{
				delete pFrames[i].ImageBuffer;
				pFrames[i].ImageBuffer = NULL;
			}
			delete[] pFrames;
			return false;
		}
	}
	else
	{
		outLog = "Error: Capture Start Error in " + CameraName;
		delete[] pFrames;
		return false;
	}
}

bool Camera::CloseCapture()
{
	int i = 0;
	tPvErr Err;
	//����ֹͣ����
	if (isStreaming)
	{
		Err = PvCommandRun(H_Camera, "AcquisitionStop");
		Sleep(200);
		if (Err == ePvErrSuccess)
		{
			Err = PvCaptureQueueClear(H_Camera);		//2��CaptureQueueClear
			if (Err == ePvErrSuccess)
			{
				
				Err = PvCaptureEnd(H_Camera);
				if (Err == ePvErrSuccess)
				{
					isStreaming = false;
					outLog = CameraName + " stop Capturing.";
				}
				else
				{
					outLog = "Warnning: Failed to stop the capture of camera " + CameraName;
				}
			}
			else
			{
				outLog = "Warnning: Camera Queue Clear not finished in camera " + CameraName;
			}
		}
		else
		{
			outLog = "Warnning: Failed to stop the acquisition of camera " + CameraName;
		}

		//���������Ƿ�ֹͣ�ɹ� ��Ҫ��������
		for (i = 0; i<MAX_FRAMES; i++)
		{
			delete pFrames[i].ImageBuffer;
			pFrames[i].ImageBuffer = NULL;
		}
		if (!pFrames)
		{
			delete[]pFrames;
			pFrames = NULL;
		}

		if (Err == ePvErrSuccess)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		outLog = "Camera is not on Capturing. Camera " + CameraName;
	}
}

bool Camera::ChangeTrigerMode(int triger_mode)
{
	int ErrCode = 0;
	switch (triger_mode)
	{
	case 0:
		ErrCode |= PvAttrEnumSet(H_Camera, "FrameStartTriggerMode", "Freerun");
		ErrCode |= PvAttrEnumSet(H_Camera, "FrameStartTriggerEvent", "EdgeFalling");//�½��ش���
		break;
	case 1:
		ErrCode |= PvAttrEnumSet(H_Camera, "FrameStartTriggerMode", "SyncIn1");
		ErrCode |= PvAttrEnumSet(H_Camera, "FrameStartTriggerEvent", "EdgeFalling");//�½��ش���
		break;
	case 2:
		ErrCode |= PvAttrEnumSet(H_Camera, "FrameStartTriggerMode", "SyncIn2");
		ErrCode |= PvAttrEnumSet(H_Camera, "FrameStartTriggerEvent", "EdgeFalling");//�½��ش���
		break;
	case 3:
		ErrCode |= PvAttrEnumSet(H_Camera, "FrameStartTriggerMode", "FixedRate");
		ErrCode |= PvAttrFloat32Set(H_Camera, "FrameRate", FrameRate);
		ErrCode |= PvAttrEnumSet(H_Camera, "FrameStartTriggerEvent", "EdgeFalling");//�½��ش���
		break;
	default:
		break;
	}
	if (ErrCode != ePvErrSuccess)
	{
		outLog = "Error: Attribute setting Problem in " + CameraName + " ErrorCode: " + to_string(ErrCode);
		return false;
	}
	else
	{
		outLog = CameraName + " Camera has changed triggerMode to Index " + to_string(triger_mode);
		TrigerMode = triger_mode;
		return true;
	}
		
}

bool Camera::ChangeExposeValue(unsigned long evalue)
{
	int ErrCode = 0;
	ErrCode|= PvAttrUint32Set(H_Camera, "ExposureValue", evalue);
	ErrCode |= PvAttrUint32Get(H_Camera, "ExposureValue", &Expose);
	if (ErrCode == 0 && Expose == evalue)
	{
		outLog = CameraName + " expose has set to " + to_string(evalue);
		return true;
	}
	else
	{
		outLog = "Warnning: Expose time set failed in " + CameraName;
		return false;
	}
}

//���ÿһ֡�ɼ���ɺ� ��ÿһ֡���ݽ��еĲ��� ���������ǽ�����������������Ż�֡������ ��������������
//���50hz��� 20msһ֡�Ĵ����ٶ�
void __stdcall FrameDoneCB(tPvFrame * pFrame)
{
	Camera* this_cam = (Camera*)pFrame->Context[0];
	this_cam->FrameCount = pFrame->FrameCount;
	PvCaptureQueueFrame(tPvHandle(pFrame->Context[1]), pFrame, FrameDoneCB);
}
