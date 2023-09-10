// Fill out your copyright notice in the Description page of Project Settings.


#include "GameStateApplier.h"
#include "../IocpGameCharacter.h"

GameStateApplier::GameStateApplier()
{
}

GameStateApplier::~GameStateApplier()
{
}

bool GameStateApplier::Init(TSharedPtr<NetSession> session, UGameInstance* gameInstance)
{
	if (!PacketApplier::Init(session, gameInstance)) return false;
	return true;
}

bool GameStateApplier::ApplyPacket(TSharedPtr<RecvBuffer> packet, TSharedPtr<SerializeManager> deserializer)
{
	URovenhellGameInstance* gameInstance = Cast<URovenhellGameInstance>(GameInstance);
	if (!gameInstance) return false;

	bool applied = true;
	switch (gameInstance->GetExecType()->GetHostType())
	{
		case HostTypeEnum::CLIENT:
		case HostTypeEnum::CLIENT_HEADLESS:
			{
				applied &= ApplyPacket_UEClient(packet, deserializer);
				break;
			}
		case HostTypeEnum::LOGIC_SERVER:
		case HostTypeEnum::LOGIC_SERVER_HEADLESS:
			{
				applied &= ApplyPacket_UEServer(packet, deserializer);
				break;
			}
	}
    return applied;
}

bool GameStateApplier::ApplyPacket_UEClient(TSharedPtr<RecvBuffer> packet, TSharedPtr<SerializeManager> deserializer)
{
	uint32 tick = ((PacketHeader*)(packet->GetBuf()))->tick;
	Cast<URovenhellGameInstance>(GameInstance)->TickCounter->SetServerTick_UEClient(tick); // 서버 틱과 동기화


	////////// TEST
	deserializer->Clear();
	SD_ActorPhysics* physicsData = new SD_ActorPhysics();
	deserializer->ReadDataFromBuffer(packet);
	deserializer->Deserialize((SD_Data*)physicsData);
	//UE_LOG(LogTemp, Warning, TEXT("%f %f %f / %f %f %f / %f %f %f"), transformData->Transform.GetLocation().X, transformData->Transform.GetLocation().Y, transformData->Transform.GetLocation().Z, transformData->Transform.GetRotation().X, transformData->Transform.GetRotation().Y, transformData->Transform.GetRotation().Z);

	// 테스트
	// 클라 연산 결과와 비교하는 로직이 필요
	for (TActorIterator<AIocpGameCharacter> iter(GameInstance->GetWorld()); iter; ++iter)
	{
		FVector velocity(physicsData->xVelocity, physicsData->yVelocity, physicsData->zVelocity);
		uint32 syncTick = (*iter)->GetSyncComp()->IsActorInSyncWith(tick, physicsData->Transform, velocity);
		if (!syncTick)
		{
			(*iter)->GetSyncComp()->AdjustActorPhysics(((PacketHeader*)(packet->GetBuf()))->deltaTime, tick, physicsData->Transform, velocity);
		}
		break;
	}
	return true;
}

bool GameStateApplier::ApplyPacket_UEServer(TSharedPtr<RecvBuffer> packet, TSharedPtr<SerializeManager> deserializer)
{
	return true;
}
