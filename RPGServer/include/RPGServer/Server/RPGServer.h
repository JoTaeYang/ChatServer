#pragma once

#include "Library/Server/CServer.h"

#include <limits>

class RPGServer : public CServer
{
public:
	bool Start(const int InSessionCount, const class CSetting& InSetting);

protected:
	virtual void OnJoin(int Index) override;
	virtual void OnRecv(int Index, class CMessageBuffer* Buffer) override;
	virtual void OnLeave(int Index) override;
private:
	class User* users;	

private:
	void CreatePlayer(int Index, CMessageBuffer* buffer);

private:
	void MakePacketCreateOtherPlayer(int Index, CMessageBuffer* buffer);

private:
	void SendAround(int Index);

private:
	// 양자화 적용
	void PacketProc_Move_Client(int Index, class CMessageBuffer* Buffer);

	// 양자화 미적용
	void PacketProc_Move_ClientV1(int Index, class CMessageBuffer* Buffer);


private:
	float DequantizeInt16ToFloat(short quantizedValue) {
		return (quantizedValue - SHRT_MIN) * (X_MAX - X_MIN) / (SHRT_MAX - SHRT_MIN) + X_MIN;
	}

	// 양자화할 최소/최대 범위 설정
	const float X_MIN = -1000.0f;
	const float X_MAX = 1000.0f;
};