#pragma once

/*
* Header 에서 2byte 사용 중
* TODO :: 구현하는 서버 쪽에 패킷 타입이 있어야함. 따라서 추후에 필터 수정 필요함. 지금 구현하는 거에서 대다수는 라이브러리로 빼기
* 
*/
enum EPacketType {

	CS_ECHO = 1000,
	SC_ECHO = 1001
};