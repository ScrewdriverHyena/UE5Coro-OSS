// Copyright No Bright Shadows. All Rights Reserved.

#include "InterfaceTasks/UE5Coro_Presence.h"
#include "OnlineSubsystemUtils.h"

UAsyncPresence::UAsyncPresence()
{
}

void UAsyncPresence::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	[](const FLatentLambda) -> TCoroutine<>
	{
		co_await UGameViewportClient::OnViewportCreated();

		Presence = Online::GetSubsystem(Get()->GetWorld())->GetPresenceInterface().ToWeakPtr();
	}(FLambdaParam(this));
}

void UAsyncPresence::Deinitialize()
{
	Presence.Reset();
	
	Super::Deinitialize();
}

UAsyncPresence* UAsyncPresence::Get()
{
	return GEngine->GameViewport->GetGameInstance()->GetSubsystem<UAsyncPresence>();
}