#pragma once

#include <memory>
#include <vector>

class CMessageBuffer
{
public:
	CMessageBuffer();
	CMessageBuffer(int InBufferSize);
	~CMessageBuffer();

	void Clear(void);

public:
	int PutData(char* chpSrc, int iSrcSize);

public:
	//�ֱ�
	CMessageBuffer& operator <<(char chValue);
	CMessageBuffer& operator <<(unsigned char uchValue);

	CMessageBuffer& operator <<(short shValue);
	CMessageBuffer& operator <<(unsigned short ushValue);

	CMessageBuffer& operator <<(int iValue);
	CMessageBuffer& operator <<(unsigned int uiValue);

	CMessageBuffer& operator <<(float fValue);

	CMessageBuffer& operator <<(long ilValue);
	CMessageBuffer& operator <<(unsigned long uilValue);

	CMessageBuffer& operator <<(long long iValue);
	CMessageBuffer& operator <<(double dValue);


	//����
	CMessageBuffer& operator >>(char& chValue);
	CMessageBuffer& operator >>(unsigned char& uchValue);


	CMessageBuffer& operator >>(short& shValue);
	CMessageBuffer& operator >>(unsigned short& ushValue);

	CMessageBuffer& operator >>(int& iValue);
	CMessageBuffer& operator >>(unsigned int& uiValue);

	CMessageBuffer& operator >>(float& fValue);

	CMessageBuffer& operator >>(long& ilValue);
	CMessageBuffer& operator >>(unsigned long& uilValue);

	CMessageBuffer& operator >>(double& dValue);
	CMessageBuffer& operator >>(long long& iValue);

private:
	unsigned char* Buffer;			// ����				

	int ReadPos;										// �б� ��ġ
	int WritePos;										// ���� ��ġ
	int BufferSize;										// ������ ũ��
	int UseLength;										// �߰��� �������� ����

private:
	enum eMessageBuffer
	{
		eDefault_Buffer_Size = 1024
	};
};