// Copyright No Bright Shadows. All Rights Reserved.

#include "InterfaceTasks/UE5Coro_Session.h"
#include "OnlineSubsystemUtils.h"

UAsyncSession::UAsyncSession() = default;

void UAsyncSession::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	[](const FLatentLambda) -> TCoroutine<>
	{
		co_await UGameViewportClient::OnViewportCreated();

		Session = Online::GetSubsystem(Get()->GetWorld())->GetSessionInterface().ToWeakPtr();
	}(FLambdaParam(this));
}

void UAsyncSession::Deinitialize()
{
	Session.Reset();
	
	Super::Deinitialize();
}

UAsyncSession* UAsyncSession::Get(const UObject* WorldContext)
{
	return WorldContext->GetWorld()->GetGameInstance()->GetSubsystem<UAsyncSession>();
}
