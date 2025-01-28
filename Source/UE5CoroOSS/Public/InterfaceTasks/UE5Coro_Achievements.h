// Copyright No Bright Shadows. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/OnlineAchievementsInterface.h"
#include "UE5Coro.h"
#include "UE5CoroOSS_Shared.h"
#include "UE5Coro_Achievements.generated.h"

UCLASS()
class UE5COROOSS_API UAsyncAchievements final : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:

	UAsyncAchievements();
	
	//~UGameInstanceSubsystem Interface Begin
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	//~UGameInstanceSubsystem Interface End
	
	static UAsyncAchievements* Get();

	static void SetInterfaceName(const FName& Name = FName(TEXT("RedpointEOS")));

	template <typename T = TTuple<FUniqueNetIdRepl /*PlayerId*/, bool /*bWasSuccessful*/>>
	TCoroutine<TOptional<T>> WriteAchievements(const FUniqueNetId& PlayerId, FOnlineAchievementsWriteRef& WriteObject);

	/**
	 * @brief	Read achievement descriptions from the server for displaying achievements in game.
	 * 
	 * @see		FOnlineAchievementDesc
	 *
	 * @param PlayerId	The id of the player we are reading achievements for.
	 *
	 * @return	Coroutine.
	 */
	template <typename T = TTuple<FUniqueNetIdRepl /*PlayerId*/, bool /*bWasSuccessful*/>>
	TCoroutine<TOptional<T>> QueryAchievementDescriptions(const FUniqueNetId& PlayerId);

	/**
	 * @brief	Read achievement ids and progress from the server.
	 * 
	 * @note	You must use QueryAchievementDescriptions to get more information about the achievements.
	 * 
	 * @see		FOnlineAchievement
	 * @see		QueryAchievementDescriptions
	 *
	 * @param PlayerId	The id of the player we are reading achievements for.
	 *
	 * @return	Coroutine.
	 */
	template <typename T = TTuple<FUniqueNetIdRepl /*PlayerId*/, bool /*bWasSuccessful*/>>
	TCoroutine<TOptional<T>> QueryAchievements(const FUniqueNetId& PlayerId);

	

private:

	static inline TWeakPtr<IOnlineAchievements> Achievements;
};

template <typename T>
TCoroutine<TOptional<T>> UAsyncAchievements::WriteAchievements(const FUniqueNetId& PlayerId, FOnlineAchievementsWriteRef& WriteObject)
{
	static FUniqueNetIdRepl StaticLocalUserId;
	static bool bStaticWasSuccessful;

	FOnAchievementsWrittenDelegate AchievementsWrittenDelegate;

	const TCoroutine<> DoWriteAchievements = []
		(const FLatentLambda, FOnAchievementsWrittenDelegate& ThisAchievementsWrittenDelegate,
			const FUniqueNetId& ThisLocalUserId, FOnlineAchievementsWriteRef& ThisWriteObject) -> TCoroutine<>
	{
		auto [InnerPlayerId, bInnerWasSuccessful] = co_await ThisAchievementsWrittenDelegate;

		StaticLocalUserId = InnerPlayerId;
		bStaticWasSuccessful = bInnerWasSuccessful;
	}(FLambdaParam(this), AchievementsWrittenDelegate, PlayerId, WriteObject);

	const TCoroutine<> WriteAchievementsCoroutine = []
		(const FLatentLambda, FOnAchievementsWrittenDelegate& ThisAchievementsWrittenDelegate,
			const FUniqueNetId& ThisLocalUserId, FOnlineAchievementsWriteRef& ThisWriteObject) -> TCoroutine<>
	{
		co_await Latent::Until([ThisAchievementsWrittenDelegate]
		{
			return ThisAchievementsWrittenDelegate.IsBound();
		});

		if (!Achievements.IsValid())
		{
			co_return;
		}

		Achievements.Pin()->WriteAchievements(ThisLocalUserId, ThisWriteObject, ThisAchievementsWrittenDelegate);

		co_await Async::PlatformSeconds(UE5CoroOSS::GetTimeout());
	}(FLambdaParam(this), AchievementsWrittenDelegate, PlayerId, WriteObject);

	co_return !co_await Race(WriteAchievementsCoroutine, DoWriteAchievements) ? T() : T(StaticLocalUserId, bStaticWasSuccessful);
}

template <typename T>
TCoroutine<TOptional<T>> UAsyncAchievements::QueryAchievementDescriptions(const FUniqueNetId& PlayerId)
{
	static FUniqueNetIdRepl StaticLocalUserId;
	static bool bStaticWasSuccessful;

	FOnQueryAchievementsCompleteDelegate QueryAchievementDescriptionsDelegate;

	const TCoroutine<> DoQueryAchievementDescriptions = []
		(const FLatentLambda, FOnQueryAchievementsCompleteDelegate& ThisQueryAchievementDescriptionsDelegate,
			const FUniqueNetId& ThisLocalUserId) -> TCoroutine<>
	{
		auto [InnerPlayerId, bInnerWasSuccessful] = co_await ThisQueryAchievementDescriptionsDelegate;

		StaticLocalUserId = InnerPlayerId;
		bStaticWasSuccessful = bInnerWasSuccessful;
	}(FLambdaParam(this), QueryAchievementDescriptionsDelegate, PlayerId);

	const TCoroutine<> QueryAchievementDescriptionsCoroutine = []
		(const FLatentLambda, FOnQueryAchievementsCompleteDelegate& ThisQueryAchievementDescriptionsDelegate,
			const FUniqueNetId& ThisLocalUserId) -> TCoroutine<>
	{
		co_await Latent::Until([ThisQueryAchievementDescriptionsDelegate]
		{
			return ThisQueryAchievementDescriptionsDelegate.IsBound();
		});

		if (!Achievements.IsValid())
		{
			co_return;
		}

		Achievements.Pin()->QueryAchievementDescriptions(ThisLocalUserId, ThisQueryAchievementDescriptionsDelegate);

		co_await Async::PlatformSeconds(UE5CoroOSS::GetTimeout());
	}(FLambdaParam(this), QueryAchievementDescriptionsDelegate, PlayerId);

	co_return !co_await Race(QueryAchievementDescriptionsCoroutine, DoQueryAchievementDescriptions)
		? T() : T(StaticLocalUserId, bStaticWasSuccessful);
}

template <typename T>
TCoroutine<TOptional<T>> UAsyncAchievements::QueryAchievements(const FUniqueNetId& PlayerId)
{
	static FUniqueNetIdRepl StaticLocalUserId;
	static bool bStaticWasSuccessful;

	FOnQueryAchievementsCompleteDelegate QueryAchievementsDelegate;

	const TCoroutine<> DoQueryAchievements = []
		(const FLatentLambda, FOnQueryAchievementsCompleteDelegate& ThisQueryAchievementsDelegate,
			const FUniqueNetId& ThisLocalUserId) -> TCoroutine<>
	{
		auto [InnerPlayerId, bInnerWasSuccessful] = co_await ThisQueryAchievementsDelegate;

		StaticLocalUserId = InnerPlayerId;
		bStaticWasSuccessful = bInnerWasSuccessful;
	}(FLambdaParam(this), QueryAchievementsDelegate, PlayerId);

	const TCoroutine<> QueryAchievementsCoroutine = []
		(const FLatentLambda, FOnQueryAchievementsCompleteDelegate& ThisQueryAchievementsDelegate,
			const FUniqueNetId& ThisLocalUserId) -> TCoroutine<>
	{
		co_await Latent::Until([ThisQueryAchievementsDelegate]
		{
			return ThisQueryAchievementsDelegate.IsBound();
		});

		if (!Achievements.IsValid())
		{
			co_return;
		}

		Achievements.Pin()->QueryAchievements(ThisLocalUserId, ThisQueryAchievementsDelegate);

		co_await Async::PlatformSeconds(UE5CoroOSS::GetTimeout());
	}(FLambdaParam(this), QueryAchievementsDelegate, PlayerId);

	co_return !co_await Race(QueryAchievementsCoroutine, DoQueryAchievements)
		? T() : T(StaticLocalUserId, bStaticWasSuccessful);
}
