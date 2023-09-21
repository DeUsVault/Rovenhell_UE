// Fill out your copyright notice in the Description page of Project Settings.


#include "NetPlayerMovementComponent.h"
#include "PlayerPawn.h"

void UNetPlayerMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    BeginTick();

    if (!PawnOwner || !UpdatedComponent || ShouldSkipUpdate(DeltaTime))
    {
        return;
    }

    // 이동 처리
    // 서버의 경우 단일 폰에 대해 1틱에 여러 인풋을 처리할 수 있는데,
    // 각 인풋별로 발송받은 델타 타임을 적용해 연산한다.
    FVector DesiredMovementThisFrame(0, 0, 0);
    for (const MoveInputData& DataPerInput : MoveDatas)
    {
        //UE_LOG(LogTemp, Warning, TEXT("이동전: %f, %f, %f"), PawnOwner->GetActorLocation().X, PawnOwner->GetActorLocation().Y, PawnOwner->GetActorLocation().Z);

        DesiredMovementThisFrame = DataPerInput.MoveVector.GetClampedToMaxSize(1.0f) * 300.0f * DataPerInput.DeltaTime; // 델타 타임 반영
        PawnOwner->SetActorLocation(PawnOwner->GetActorLocation() + DesiredMovementThisFrame, false, nullptr, ETeleportType::ResetPhysics);
    }

    EndTick();
}

APlayerPawn* UNetPlayerMovementComponent::GetPlayerOwner()
{
    return (APlayerPawn*)(PawnOwner.Get()); // risky...
}

ANetHandler* UNetPlayerMovementComponent::GetNetHandler()
{
	if (APlayerPawn* playerOwner = GetPlayerOwner())
	{
		return playerOwner->GetNetHandler();
	}
	return nullptr;
}

void UNetPlayerMovementComponent::BeginTick()
{
}

void UNetPlayerMovementComponent::EndTick()
{
    MoveDatas.Empty();
}

void UNetPlayerMovementComponent::AddMovementData(FVector MoveVector, float DeltaTime)
{
    MoveDatas.Add({ MoveVector, DeltaTime });
}
