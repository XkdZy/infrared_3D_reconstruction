#pragma once
#include "SerialPort.h"
#include <thread>
#include <string>
#include <mutex>

string sendMessageBuff;

class SerialListener
{
public:
	SerialListener();
	~SerialListener();
	void Run();
private:
	SerialPort* robotSerial;		//��е�ۿ��������ڴ���ʵ��������
	mutex _mutex;
};

