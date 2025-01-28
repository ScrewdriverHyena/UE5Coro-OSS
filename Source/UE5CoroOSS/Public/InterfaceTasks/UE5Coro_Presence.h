// Copyright No Bright Shadows. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UE5Coro.h"
#include "Interfaces/OnlinePresenceInterface.h"
#include "UE5Coro_Presence.generated.h"

UCLASS()
class UE5COROOSS_API UAsyncPresence final : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:

	UAsyncPresence();

	//~UGameInstanceSubsystem Interface Begin
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	
	virtual void Deinitialize() override;
	//~UGameInstanceSubsystem Interface End

	static UAsyncPresence* Get();

	/**
	 * @brief	Starts an async operation that will update the cache with presence data from all users in the Users array.
	 *
	 * @note	On platforms that support multiple keys, this function will query all keys.
	 *
	 * @param LocalUserId	The unique id of the user initiating the query for presence information.
	 * @param UserIds		The list of users' unique ids to query for presence information.
	 *
	 * @return	See: FOnPresenceTaskCompleteDelegate
	 */
	template <typename T = TTuple<const FUniqueNetId& /*UserId*/, bool /*bWasSuccessful*/>>
	static TCoroutine<TOptional<T>> QueryPresence(const FUniqueNetId& LocalUserId, const TArray<FUniqueNetIdRef>& UserIds);

private:

	static inline TWeakPtr<IOnlinePresence> Presence;

	static inline IOnlinePresence::FOnPresenceTaskCompleteDelegate QueryPresenceDelegate;
};

template <typename T>
TCoroutine<TOptional<T>> UAsyncPresence::QueryPresence(const FUniqueNetId& LocalUserId, const TArray<FUniqueNetIdRef>& UserIds)
{
	[](const FLatentLambda This, const FUniqueNetId& ThisLocalUserId, const TArray<FUniqueNetIdRef>& ThisUserIds) -> TCoroutine<>
	{
		co_await Latent::Until([This]
			() -> bool
		{
			return QueryPresenceDelegate.IsBound();
		});
		
		Presence.Pin()->QueryPresence(ThisLocalUserId, ThisUserIds, QueryPresenceDelegate);
	}(FLambdaParam(Get()), LocalUserId, UserIds);

	T&& Return = co_await QueryPresenceDelegate;
	co_return Return;
}
