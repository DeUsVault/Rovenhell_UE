// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Misc/SpinLock.h"
#include "../NetCodes/NetSession.h"
#include "../NetCodes/NetBufferManager.h"
#include "../PacketCodes/PacketHeader.h"
#include "../PacketCodes/InputApplier.h"
#include "../PacketCodes/ChatPacketApplier.h"
#include "../PacketCodes/PhysicsApplier.h"
#include "../PacketCodes/GameStateApplier.h"
#include "../PacketCodes/MiddlemanPacketApplier.h"
#include "../PacketCodes/SerializeManager.h"
#include "RovenhellGameInstance.h"
#include "GameFramework/Actor.h"
#include "NetHandler.generated.h"

UCLASS()
class IOCPGAME_API ANetHandler : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ANetHandler();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void EndPlay(const EEndPlayReason::Type EndPlayReason);

	TSharedPtr<NetSession> GetSessionShared() { return Session; }
	TSharedPtr<SerializeManager> GetSerializerShared() { return Serializer; }
	TSharedPtr<SerializeManager> GetDeserializerShared() { return Deserializer; }
	HostTypeEnum GetHostType() { return HostType; }
	void FillPacketSenderTypeHeader(TSharedPtr<SendBuffer> buffer);

private:
	void Init();
	void InitGameHostType();
	void PacketDebug(float DeltaTime);
	bool DistributePendingPacket(); // RecvPending 패킷을 적절한 Applier에게 전달하고, 처리가 완료되면 버퍼 풀에 버퍼를 반환한다
	
	void Tick_UEClient(float DeltaTime);
	void Tick_UEServer(float DeltaTime);
private:
	TSharedPtr<NetSession> Session;
	TSharedPtr<RecvBuffer> RecvPending = nullptr; // 처리를 대기중인 패킷
	TQueue<TSharedPtr<RecvBuffer>> SortedRecvPendings; // 틱 순서대로 정렬된, 처리를 대기중인 패킷들
	HostTypeEnum HostType = HostTypeEnum::NONE;

	TUniquePtr<InputApplier> InApplier = nullptr;
	TUniquePtr<ChatPacketApplier> ChatApplier = nullptr;
	TUniquePtr<PhysicsApplier> PhysApplier = nullptr;
	TUniquePtr<GameStateApplier> GameApplier = nullptr;
	TUniquePtr<MiddlemanPacketApplier> MiddleApplier = nullptr;

	TSharedPtr<SerializeManager> Serializer = nullptr;
	TSharedPtr<SerializeManager> Deserializer = nullptr;

	uint32 lastAppliedTick = 0; // 마지막으로 처리된 틱 번호; 순서 보장이 필요한 틱을 처리할 때 사용함

private:
	/* UE_SERVER */
	const float SERVER_TICK_INTERVAL = 0.2f; // ms
	float AccumulatedTickTime = 0.0f; // ms
};
