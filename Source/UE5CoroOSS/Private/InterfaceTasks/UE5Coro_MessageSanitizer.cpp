// Copyright No Bright Shadows. All Rights Reserved.

#include "InterfaceTasks/UE5Coro_MessageSanitizer.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"

UAsyncMessageSanitizer::UAsyncMessageSanitizer() = default;

void UAsyncMessageSanitizer::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	MessageSanitizer = Online::GetSubsystem(GetWorld())->GetMessageSanitizer(GetLocalPlayerChecked()->GetPlatformUserIndex(),
		ExcludePlatform).ToWeakPtr();
}

void UAsyncMessageSanitizer::Deinitialize()
{
	if (MessageSanitizer.IsValid())
	{
		MessageSanitizer.Reset();
	}
	
	Super::Deinitialize();
}

UAsyncMessageSanitizer* UAsyncMessageSanitizer::Get(const ULocalPlayer* LocalPlayer)
{
	return LocalPlayer->GetSubsystem<ThisClass>();
}

void UAsyncMessageSanitizer::SetInterfaceName(const FName& Name)
{
	MessageSanitizer = Online::GetSubsystem(GetLocalPlayerChecked()->GetWorld(), Name)->GetMessageSanitizer(
		GetLocalPlayerChecked()->GetPlatformUserIndex(), ExcludePlatform).ToWeakPtr();
}

FString UAsyncMessageSanitizer::GetExcludePlatform()
{
	return ExcludePlatform;
}
