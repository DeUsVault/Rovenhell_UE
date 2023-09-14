// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorSyncComponent.h"

// Sets default values for this component's properties
UActorSyncComponent::UActorSyncComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void UActorSyncComponent::BeginPlay()
{
	Super::BeginPlay();

	HostTypeEnum hostType = Cast<URovenhellGameInstance>(GetWorld()->GetGameInstance())->GetExecType()->GetHostType();
	if (hostType == HostTypeEnum::CLIENT || hostType == HostTypeEnum::CLIENT_HEADLESS)
	{
		StartTicking = true;
		for (int i = 0; i < MAX_PHYSICS_HISTORY_SIZE; ++i)
		{
			PhysicsHistory[i] = { GetOwner()->GetTransform(), GetOwner()->GetRootComponent()->ComponentVelocity };
		}
	}
}

// Called every frame
void UActorSyncComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (StartTicking)
	{
		// 커서 이동
		MoveOne();

		ActorPhysics actorPhysics = { GetOwner()->GetTransform(), GetOwner()->GetRootComponent()->ComponentVelocity }; // TODO: Velocity 점검 필요
		PhysicsHistory[Head] = actorPhysics; // 노드 생성
	}
}

bool UActorSyncComponent::IsActorInSyncWith(const FTransform& Transform, const FVector& Velocity)
{
	// 틱 히스토리 내부에 주어진 입력과 싱크가 맞는 기록이 있는지 확인한다
	uint32 currentIndex = Head;
	for (uint32 i = 0; i < (uint32)MAX_PHYSICS_HISTORY_SIZE - (uint32)IGNORE_RECENT_HISTORY_SIZE; ++i)
	{
		// 물리 정보가 서버와 일치하는 틱이 있는지 판정
		if(IsValidPhysicsData(currentIndex, Transform, Velocity))
		{
			return true;
		}
		currentIndex = (currentIndex + 1) % MAX_PHYSICS_HISTORY_SIZE;
	}
	return false;
}

void UActorSyncComponent::AdjustActorPhysics(float ServerDeltaTime, const FTransform& Transform, const FVector& Velocity)
{
	// 파라미터로 전달되는 서버 델타는 기존에 추측 항법 계산을 위해 사용했으나, 현재는 사용하지 않는다
	// 다만 삭제는 아직 하지 않고 일단 보류한다

	//// TESTING
	DrawDebugPoint(GetWorld(), GetOwner()->GetTransform().GetLocation(), 5, FColor(255, 0, 0), true, 5.0);
	DrawDebugPoint(GetWorld(), Transform.GetLocation(), 5, FColor(0, 255, 0), true, 5.0);

	// 마지막으로 수신한 서버측 위치로 이동한다
	GetOwner()->GetRootComponent()->ComponentVelocity = Velocity;
	GetOwner()->SetActorTransform(Transform, false, nullptr, ETeleportType::None);
	UE_LOG(LogTemp, Log, TEXT("ActorSyncComponent - 위치 보정 완료"));
}

bool UActorSyncComponent::IsValidPhysicsData(uint32 index, const FTransform& Transform, const FVector& Velocity)
{
	// 서버에서의 이동 연산 결과는 결국 클라이언트 틱에서의 연산결과 중 하나와 매우 근접하다 
	// (클라와 똑같은 정보를 처리하며 처리 rate만 다른 것이기 때문에)
	// 따라서 클라이언트의 틱n, n+1 에서의 위치 사이에 서버 틱이 위치하면 정상으로 간주한다.

	// 두 점 사이에 한 점이 있는지 판정
	double AB = FVector::Dist(PhysicsHistory[index].transform.GetLocation(), Transform.GetLocation());
	double BC = FVector::Dist(Transform.GetLocation(), PhysicsHistory[(index + 1) % MAX_PHYSICS_HISTORY_SIZE].transform.GetLocation());
	double AC = FVector::Dist(PhysicsHistory[index].transform.GetLocation(), PhysicsHistory[(index + 1) % MAX_PHYSICS_HISTORY_SIZE].transform.GetLocation());
	return (FMath::Abs(AB + BC - AC) <= ALLOWED_LOCATION_DIFFERENCE_WITH_SERVER); // Velocity 고려 X
}
