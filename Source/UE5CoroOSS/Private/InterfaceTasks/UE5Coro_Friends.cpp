// Copyright No Bright Shadows. All Rights Reserved.

#include "InterfaceTasks/UE5Coro_Friends.h"
#include "OnlineSubsystemUtils.h"

UAsyncFriends::UAsyncFriends()
{
}

void UAsyncFriends::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	[](const FLatentLambda) -> TCoroutine<>
	{
		co_await UGameViewportClient::OnViewportCreated();

		Friends = Online::GetSubsystem(Get()->GetWorld())->GetFriendsInterface().ToWeakPtr();
	}(FLambdaParam(this));
}

void UAsyncFriends::Deinitialize()
{
	Friends.Reset();
	
	Super::Deinitialize();
}

UAsyncFriends* UAsyncFriends::Get()
{
	return GEngine->GameViewport->GetGameInstance()->GetSubsystem<UAsyncFriends>();
}

void UAsyncFriends::SetInterfaceName(const FName& Name)
{
	Friends = Online::GetSubsystem(Get()->GetWorld(), Name)->GetFriendsInterface().ToWeakPtr();
}
