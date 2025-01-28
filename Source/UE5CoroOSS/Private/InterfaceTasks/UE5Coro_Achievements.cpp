// Copyright No Bright Shadows. All Rights Reserved.

#include "InterfaceTasks/UE5Coro_Achievements.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"

UAsyncAchievements::UAsyncAchievements() = default;

void UAsyncAchievements::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	Achievements = Online::GetSubsystem(GetWorld(), FName(TEXT("RedpointEOS")))->GetAchievementsInterface().ToWeakPtr();
}

UAsyncAchievements* UAsyncAchievements::Get()
{
	return GEngine->GameViewport->GetGameInstance()->GetSubsystem<UAsyncAchievements>();
}

void UAsyncAchievements::SetInterfaceName(const FName& Name)
{
	Achievements = Online::GetSubsystem(Get()->GetWorld(), Name)->GetAchievementsInterface().ToWeakPtr();
}


