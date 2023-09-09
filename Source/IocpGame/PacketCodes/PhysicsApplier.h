// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PacketApplier.h"
#include "RovenhellGameInstance.h"

/**
 * 
 */
class IOCPGAME_API PhysicsApplier : public PacketApplier
{
public:
	PhysicsApplier();
	virtual ~PhysicsApplier();

	bool Init(TSharedPtr<NetSession> session, UGameInstance* gameInstance);
	bool ApplyPacket(TSharedPtr<RecvBuffer> packet) override;
};
