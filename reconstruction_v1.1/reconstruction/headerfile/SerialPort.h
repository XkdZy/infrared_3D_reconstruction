#ifndef __SERIALPORT_H__
#define __SERIALPORT_H__

#include <windows.h>
#include <iostream>
#include <string>
#include <string>

using namespace std;

//string readMessageBuff;
//bool readComplication = false;
class SerialPort
{
public:
	SerialPort(void);
	~SerialPort(void);
	//�򿪴���
	
	bool SerialOpen(LPCSTR COMx, int BaudRate);
	//��������
	int SerialWriteString(string& Buf, int size);
	int SerialWriteChar(unsigned char* Buf, int size);
	//��������
	int SerialReadString(string &OutBuf,int maxSize);
	int SerialReadChar(unsigned char* OutBuf,int maxSize);
	//������������
	void ListenSerial();
private:
	HANDLE m_hComm;//���ھ��
};
#endif
