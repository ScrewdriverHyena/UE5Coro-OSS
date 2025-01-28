// Copyright No Bright Shadows. All Rights Reserved.

#include "InterfaceTasks/UE5Coro_Identity.h"
#include "OnlineSubsystemUtils.h"

UAsyncIdentity::UAsyncIdentity()
{
}

void UAsyncIdentity::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	[](const FLatentLambda) -> TCoroutine<>
	{
		co_await UGameViewportClient::OnViewportCreated();

		Identity = Online::GetSubsystem(Get()->GetWorld())->GetIdentityInterface().ToWeakPtr();
	}(FLambdaParam(this));
}

void UAsyncIdentity::Deinitialize()
{
	Super::Deinitialize();

	Identity.Reset();
}

UAsyncIdentity* UAsyncIdentity::Get()
{
	return GEngine->GameViewport->GetGameInstance()->GetSubsystem<UAsyncIdentity>();
}

void UAsyncIdentity::SetInterfaceName(const FName& Name)
{
	Identity = Online::GetSubsystem(Get()->GetWorld(), Name)->GetIdentityInterface().ToWeakPtr();
}
