// Copyright No Bright Shadows. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UE5Coro.h"
#include "UE5CoroOSS_Shared.h"
#include "Interfaces/IMessageSanitizerInterface.h"
#include "UE5Coro_MessageSanitizer.generated.h"

DEFINE_LOG_CATEGORY_STATIC(LogUE5CoroMessageSanitizer, Log, All);

UCLASS()
class UE5COROOSS_API UAsyncMessageSanitizer final : public ULocalPlayerSubsystem
{
	GENERATED_BODY()
	
public:
	
	UAsyncMessageSanitizer();

	//~UGameInstanceSubsystem Interface Begin
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	
	virtual void Deinitialize() override;
	//~UGameInstanceSubsystem Interface End
	
	static UAsyncMessageSanitizer* Get(const ULocalPlayer* LocalPlayer);

	void SetInterfaceName(const FName& Name = FName(TEXT("RedpointEOS")));

	FString GetExcludePlatform();

	/**
	 * @brief	Sanitizes an array of display names/messages. Should be used with platform OSS.
	 * 
	 * @param DisplayNames			Array of display names/messages to sanitize.
	 * @param ForceLatentCoroutine	Forces latent coroutine. Do not set.
	 *
	 * @return	See: FOnMessageArrayProcessed
	 */
	template <typename T = TTuple<bool /*bSuccess*/, const TArray<FString>& /*SanitizedMessages*/>>
	TCoroutine<TOptional<T>> SanitizeDisplayNames(const TArray<FString>& DisplayNames, const FForceLatentCoroutine ForceLatentCoroutine = {});
	
	/**
	 * @brief	Query for a blocked user status between a local and remote user.
	 *
	 * @param LocalUserNum			Local user making the query.
	 * @param FromUserId			Platform-specific user id of the remote user.
	 * @param FromPlatform			Platform for remote user.
	 * @param ForceLatentCoroutine	Forces latent coroutine. Do not set.
	 *
	 * @return	See: FOnQueryUserBlockedResponse
	 */
	template <typename T = TTuple<const FBlockedQueryResult&>>
	TCoroutine<TOptional<T>> QueryBlockedUser(const FPlatformUserId LocalUserNum, const FString& FromUserId, const FString& FromPlatform,
		const FForceLatentCoroutine ForceLatentCoroutine = {});
	
private:
	
	TWeakPtr<IMessageSanitizer> MessageSanitizer;

	FOnMessageArrayProcessed SanitizeDisplayNamesDelegate;

	FOnQueryUserBlockedResponse QueryBlockedUserDelegate;

	FString ExcludePlatform;
};

template <typename T>
TCoroutine<TOptional<T>> UAsyncMessageSanitizer::SanitizeDisplayNames(const TArray<FString>& DisplayNames, const FForceLatentCoroutine)
{
	const TCoroutine<> SanitizeDisplayNamesCoroutine = []
		(const FLatentLambda This, const TArray<FString>& ThisDisplayNames) -> TCoroutine<>
	{
		co_await Latent::Until([This]
			() -> bool
		{
			return This->SanitizeDisplayNamesDelegate.IsBound();
		});
		
		This->MessageSanitizer.Pin()->SanitizeDisplayNames(ThisDisplayNames, This->SanitizeDisplayNamesDelegate);

		co_await Async::PlatformSeconds(UE5CoroOSS::GetTimeout());
	}(FLambdaParam(this), DisplayNames);

	bool bSuccess = false;
	TArray<FString> SanitizedDisplayNames;
	
	const TCoroutine<> DoSanitizeDisplayNames = []
		(const FLatentLambda This, bool& OutSuccess, TArray<FString>& OutDisplayNames) -> TCoroutine<>
	{
		auto [InnerSuccess, InnerDisplayNames] = co_await This->SanitizeDisplayNamesDelegate;
		
		OutSuccess = InnerSuccess;
		OutDisplayNames = InnerDisplayNames;
	}(FLambdaParam(this), bSuccess, SanitizedDisplayNames);
	
	co_return !co_await Race(SanitizeDisplayNamesCoroutine, DoSanitizeDisplayNames) ? TOptional<T>() : T(bSuccess, SanitizedDisplayNames);
}

template <typename T>
TCoroutine<TOptional<T>> UAsyncMessageSanitizer::QueryBlockedUser(const FPlatformUserId LocalUserNum, const FString& FromUserId,
	const FString& FromPlatform, const FForceLatentCoroutine)
{
	const TCoroutine<> QueryBlockedUserCoroutine = []
		(const FLatentLambda This, const FPlatformUserId ThisLocalUserNum, const FString& ThisFromUserId, const FString& ThisFromPlatform) -> TCoroutine<>
	{
		co_await Latent::Until([This]
			() -> bool
		{
			return This->QueryBlockedUserDelegate.IsBound();
		});
		
		This->MessageSanitizer.Pin()->QueryBlockedUser(ThisLocalUserNum, ThisFromUserId, ThisFromPlatform,
			This->QueryBlockedUserDelegate);

		co_await Async::PlatformSeconds(UE5CoroOSS::GetTimeout());
	}(FLambdaParam(this), LocalUserNum, FromUserId, FromPlatform);

	FBlockedQueryResult Result;
	
	const TCoroutine<> DoQueryBlockedUser = []
		(const FLatentLambda This, FBlockedQueryResult& ThisResult) -> TCoroutine<>
	{
		auto [InnerResult] = co_await This->QueryBlockedUserDelegate;
		
		ThisResult = InnerResult;
	}(FLambdaParam(this), Result);
	
	co_return !co_await Race(QueryBlockedUserCoroutine, DoQueryBlockedUser) ? TOptional<T>() : T(Result);
}
