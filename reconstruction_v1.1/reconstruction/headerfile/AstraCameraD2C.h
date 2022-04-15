#pragma once
#include "d2cSwapper.h"
#include "ObCommon.h"
#include "Visualizer.h"
#include <Open3D/Open3D.h>

using namespace cv;
using namespace open3d;

#define SAMPLE_READ_WAIT_TIMEOUT 2000 //2000ms
#define MIN_DISTANCE 200  //��λ����
#define MAX_DISTANCE 2000 //��λ����
#define RESOULTION_X 640.0  //�궨ʱ�ķֱ���
#define RESOULTION_Y 480.0  //�궨ʱ�ķֱ���
#define MAX_FRAME_COUNT 50

typedef struct xnIntrinsic_Params
{
	xnIntrinsic_Params() :
		c_x(320.0), c_y(240.0), f_x(480.0), f_y(480.0)
	{}

	xnIntrinsic_Params(float c_x_, float c_y_, float f_x_, float f_y_) :
		c_x(c_x_), c_y(c_y_), f_x(f_x_), f_y(f_y_)
	{}

	float c_x; //u���ϵĹ�һ������
	float c_y; //v���ϵĹ�һ������
	float f_x; //����x����
	float f_y; //����y����
}xIntrinsic_Params;


class AstraCameraD2C :public d2cSwapper
{
public:
	AstraCameraD2C();
	virtual ~AstraCameraD2C();

	//�������ܣ������ʼ��
	int CameraInit(int d2cType);
	//�������ܣ��������ʼ��
	int CameraUnInit(void);

	//�������ܣ���ȡ���������
	int GetColorAndDepthImage(cv::Mat& colorImage, cv::Mat& depthImage);
	int GetColorImage(cv::Mat& colorImage);
	int GetDepthImage(cv::Mat& depthImage);

	//ת��ͼ����������굽��������
	void convertDepthToCamrera(const ushort& u, const ushort& v, ushort& d, double& worldX, double& worldY, double& worldZ);
	//ʵʱ��ʾ����ͷ�ɼ��ĵ�������
	void RealtimeVisualPointCloud();

private:
	int DepthInit(void);
	int DepthUnInit();
	/****start depth swapper****/
	int Depthstart(int width, int height);
	int Depthstop();
	int WaitDepthStream(VideoFrameRef &frame);

	/****start color swapper****/
	int ColorStart(int width, int height);
	int ColorStop();
	int WaitColorStream(VideoFrameRef &frame);
	
	//�����ȡ����ڲ���
	void getCameraParams(openni::Device& Device, xIntrinsic_Params& IrParam);

	//�������ܣ�ֹͣ��
	int StreamStop(void);
	//��������: ��ȡ����������
	int GetCameraParam(OBCameraParams& cameraParam);
	//�������ܣ���ȡDepth�ֱ���
	int GetCameraResolution(int& nImageWidth, int& nImageHeight);
	//�������ܣ���ȡ�豸��pid
	uint16_t GetDevicePid(void);

	bool IsLunaDevice(void);


	VideoStream m_ColorStream;
	VideoStream m_depthStream;

	bool m_bDepthInit;			//�豸�Ƿ��ʼ��
	bool m_bDepthStart;			//����������Ƿ���
	bool m_bDepStreamCreate;	//����������Ƿ񴴽�
	//float* m_histogram;			//
	bool m_bColorStart;			//��ɫ�������Ƿ���
	bool m_bColorStreamCreate;	//��ɫ�������Ƿ񴴽�

private:
	//depth data
	Device m_device;

	//�洢����ڲε�ȫ�ֱ���
	xIntrinsic_Params m_IntrinsicParam;
	double fdx, fdy, u0, v0;

	int m_ImageWidth;	//ͼ��Ŀ�
	int m_ImageHeight;	//ͼ��ĸ�

	std::shared_ptr<geometry::PointCloud> pointcloud_ptr;
	Visualizer* visualizer;	//������ʾ��
	bool isAddGeometry;
};

