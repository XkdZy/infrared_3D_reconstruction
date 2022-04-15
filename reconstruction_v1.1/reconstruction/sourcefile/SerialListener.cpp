#include "SerialListener.h"

SerialListener::SerialListener()
{

}

SerialListener::~SerialListener()
{
}

void SerialListener::Run()
{
	this->robotSerial = new SerialPort;		//��е�ۿ��������ڴ���ʵ��������
	if (!this->robotSerial->SerialOpen("COM3", 115200))
		return;
	while (true)
	{
		_mutex.lock();
		if (sendMessageBuff.size())
		{
			robotSerial->SerialWriteString(sendMessageBuff, sendMessageBuff.size());
			sendMessageBuff.clear();
		}
		_mutex.unlock();
		robotSerial->SerialReadString()
	}
}