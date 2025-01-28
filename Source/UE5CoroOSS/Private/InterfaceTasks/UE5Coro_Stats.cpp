// Copyright No Bright Shadows. All Rights Reserved.

#include "InterfaceTasks/UE5Coro_Stats.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"

UAsyncStats::UAsyncStats() = default;

void UAsyncStats::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	Stats = Online::GetSubsystem(GetWorld(), FName(TEXT("RedpointEOS")))->GetStatsInterface().ToWeakPtr();
}

UAsyncStats* UAsyncStats::Get()
{
	return GEngine->GameViewport->GetGameInstance()->GetSubsystem<UAsyncStats>();
}

void UAsyncStats::SetInterfaceName(FName Name)
{
	Stats = Online::GetSubsystem(Get()->GetWorld(), MoveTemp(Name))->GetStatsInterface().ToWeakPtr();
}


