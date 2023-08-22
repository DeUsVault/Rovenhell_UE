// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "Engine.h"
#include "Sockets.h"
#include "Networking.h"
#include "SocketSubsystem.h"
#include "NetSocket.h"
#include "NetBuffer.h"
#include "NetAddress.h"
#include "SendHandler.h"
#include "RecvHandler.h"

class SendHandler;
class RecvHandler;

/**
 * 이 host와 다른 host 하나와의 연결을 관리한다.
 */
class IOCPGAME_API NetSession
{
public:
	NetSession();
	~NetSession();

	bool Init(); // 내부 요소를 초기화하고 성공 여부를 반환한다
	bool Start(); // 세션 작동을 개시하고 성공 여부를 반환한다
	bool StartSender(); // Sender 스레드를 생성하고 동작을 개시한다
	bool StartReceiver(); // Receiver 스레드를 생성하고 동작을 개시한다
	bool Kill(); // 세션 작동을 완전히 정지한다
	bool KillSend(); // Sender 스레드를 Kill한다
	bool KillRecv(); // Receiver 스레드를 Kill한다

	bool Send(const NetBuffer& sendBuffer); // l or r value
	bool Recv();
	bool TryConnect(NetAddress connectAddr, int32 minutes, int32 seconds); // waitForMs 밀리세컨드 동안 Connect를 시도하고 결과를 반환한다 (Blocking)
	bool Connect(NetAddress connectAddr); // 논블로킹 Connect I/O의 결과를 반환한다
	void Disconnect();

	const NetAddress& GetPeerAddr();

private:
	NetAddress PeerAddr;
	TUniquePtr<SendHandler> Sender = nullptr;
	TUniquePtr<RecvHandler> Receiver = nullptr;
	TUniquePtr<NetSocket> NetSock = nullptr;

private:
	int32 bytesSent; // Send()에서 사용; 현재 소켓이 Send 중일 경우, 몇 바이트를 발송 완료했는지
};
