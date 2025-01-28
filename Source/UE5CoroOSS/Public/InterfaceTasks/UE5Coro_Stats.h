// Copyright No Bright Shadows. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "OnlineError.h"
#include "Interfaces/OnlineStatsInterface.h"
#include "UE5Coro.h"
#include "UE5CoroOSS_Shared.h"
#include "UE5Coro_Stats.generated.h"

UCLASS()
class UE5COROOSS_API UAsyncStats final : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:

	UAsyncStats();

	//~UGameInstanceSubsystem Interface Begin
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	//~UGameInstanceSubsystem Interface End

	static UAsyncStats* Get();
	
	static void SetInterfaceName(FName Name);

	/**
	 * @brief	Query one or more users' stats.
	 *
	 * @param LocalUserId	User to query as (if applicable)
	 * @param StatUsers		Users to get stats for.
	 * @param StatNames		Stats to get stats for all specified users.
	 *
	 * @return	Coroutine.
	 */
	template <typename T = TTuple<const FOnlineError /*ResultError*/, const TArray<TSharedRef<const FOnlineStatsUserStats>> /*UsersStatsResult*/>>
	TCoroutine<TOptional<T>> QueryStats(const FUniqueNetIdRef LocalUserId, const TArray<FUniqueNetIdRef>& StatUsers,
		const TArray<FString>& StatNames);

	/**
	 * @brief	Asynchronously update one or more user's stats.
	 *
	 * @param LocalUserId The user to update the stats as (if applicable).
	 * @param UpdatedStats The array of user to stats pairs to update the backend with.
	 *
	 * @return	Coroutine.
	 */
	template <typename T = TTuple<const FOnlineError /*ResultError*/>>
	TCoroutine<TOptional<T>> UpdateStats(const FUniqueNetIdRef LocalUserId, const TArray<FOnlineStatsUserUpdatedStats>& UpdatedStats);

private:

	static inline TWeakPtr<IOnlineStats> Stats;
};

template <typename T>
TCoroutine<TOptional<T>> UAsyncStats::QueryStats(const FUniqueNetIdRef LocalUserId, const TArray<FUniqueNetIdRef>& StatUsers,
	const TArray<FString>& StatNames)
{
	static FOnlineError StaticResultError;
	static TArray<TSharedRef<const FOnlineStatsUserStats>> StaticUsersStatsResult;

	FOnlineStatsQueryUsersStatsComplete QueryUsersStatsComplete;

	const TCoroutine<> DoQueryStats = []
		(const FLatentLambda, FOnlineStatsQueryUsersStatsComplete& ThisQueryUsersStatsComplete, FOnlineError& ThisResultError,
			TArray<TSharedRef<const FOnlineStatsUserStats>>& ThisUsersStatsResult) -> TCoroutine<>
	{
		auto [InnerResultError, InnerUsersStatsResult] = co_await ThisQueryUsersStatsComplete;
		
		ThisResultError = InnerResultError;
		ThisUsersStatsResult = InnerUsersStatsResult;
	}(FLambdaParam(this), QueryUsersStatsComplete, StaticResultError, StaticUsersStatsResult);

	const TCoroutine<> QueryStatsCoroutine = []
		(const FLatentLambda, FOnlineStatsQueryUsersStatsComplete& ThisQueryUsersStatsComplete, const FUniqueNetIdRef& ThisLocalUserId,
			const TArray<FUniqueNetIdRef>& ThisStatUsers, const TArray<FString>& ThisStatNames) -> TCoroutine<>
	{
		co_await Latent::Until([ThisQueryUsersStatsComplete]
		{
			return ThisQueryUsersStatsComplete.IsBound();
		});

		if (!Stats.IsValid())
		{
			co_return;
		}

		Stats.Pin()->QueryStats(ThisLocalUserId, ThisStatUsers, ThisStatNames, ThisQueryUsersStatsComplete);
	}(FLambdaParam(this), QueryUsersStatsComplete, LocalUserId, StatUsers, StatNames);

	co_return !co_await Race(DoQueryStats, QueryStatsCoroutine) ? T() : T(StaticResultError, StaticUsersStatsResult);
}

template <typename T>
TCoroutine<TOptional<T>> UAsyncStats::UpdateStats(const FUniqueNetIdRef LocalUserId, const TArray<FOnlineStatsUserUpdatedStats>& UpdatedStats)
{
	static FOnlineError StaticResultError;

	FOnlineStatsUpdateStatsComplete UpdateUserStatsComplete;

	const TCoroutine<> DoUpdateStats = []
		(const FLatentLambda, FOnlineStatsUpdateStatsComplete& ThisUpdateUserStatsComplete, FOnlineError& ThisResultError) -> TCoroutine<>
	{
		auto [InnerResultError] = co_await ThisUpdateUserStatsComplete;
		
		ThisResultError = InnerResultError;
	}(FLambdaParam(this), UpdateUserStatsComplete, StaticResultError);

	const TCoroutine<> UpdateStatsCoroutine = []
		(const FLatentLambda, FOnlineStatsUpdateStatsComplete& ThisUpdateUserStatsComplete, const FUniqueNetIdRef& ThisLocalUserId,
			const TArray<FOnlineStatsUserUpdatedStats>& ThisUpdatedStats) -> TCoroutine<>
	{
		co_await Latent::Until([ThisUpdateUserStatsComplete]
		{
			return ThisUpdateUserStatsComplete.IsBound();
		});

		if (!Stats.IsValid())
		{
			co_return;
		}

		Stats.Pin()->UpdateStats(ThisLocalUserId, ThisUpdatedStats, ThisUpdateUserStatsComplete);

		co_await Async::PlatformSeconds(UE5CoroOSS::GetTimeout());
	}(FLambdaParam(this), UpdateUserStatsComplete, LocalUserId, UpdatedStats);

	co_return !co_await Race(DoUpdateStats, UpdateStatsCoroutine) ? T() : T(StaticResultError);
}
