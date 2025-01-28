// Copyright No Bright Shadows. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UE5Coro.h"
#include "UE5CoroOSS_Shared.h"
#include "Interfaces/OnlineFriendsInterface.h"
#include "UE5Coro_Friends.generated.h"

DEFINE_LOG_CATEGORY_STATIC(LogUE5CoroFriends, Log, All);

UCLASS()
class UE5COROOSS_API UAsyncFriends final : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:

	using FLatentAwaiter = Private::FLatentAwaiter;
	
	UAsyncFriends();

	//~UGameInstanceSubsystem Interface Begin
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	
	virtual void Deinitialize() override;
	//~UGameInstanceSubsystem Interface End
	
	static UAsyncFriends* Get();

	static void SetInterfaceName(const FName& Name = FName(TEXT("RedpointEOS")));
	
	/**
	 * @brief	Starts an async task that reads the named friends list for the player 
	 *
	 * @param LocalUserNum 	The user to read the friend list of
	 * @param ListName 		Name of the friend list to read
	 *
	 * @return 	See: FOnReadFriendsListComplete
	 */
	template <typename T = TTuple<int32 /*LocalUserNum*/, bool /*bWasSuccessful*/, const FString& /*ListName*/, const FString& /*ErrorStr*/>>
	static TCoroutine<TOptional<T>> ReadFriendsList(const int32 LocalUserNum, const FString& ListName);
	
private:
	
	static inline TWeakPtr<IOnlineFriends> Friends;

	static inline FOnReadFriendsListComplete ReadFriendsListDelegate;
};

template <typename T>
TCoroutine<TOptional<T>> UAsyncFriends::ReadFriendsList(const int32 LocalUserNum, const FString& ListName)
{
	const TCoroutine<> ReadFriendsListCoroutine = []
		(const FLatentLambda This, const int32 ThisLocalUserNum, const FString& ThisListName) -> TCoroutine<>
	{
		co_await Latent::Until([This]
			() -> bool
		{
			return ReadFriendsListDelegate.IsBound();
		});
		
		if (!Friends.Pin()->ReadFriendsList(ThisLocalUserNum, ThisListName, ReadFriendsListDelegate))
		{
			UE_LOG(LogUE5CoroFriends, Error, TEXT("Failed to read friends list for (%d)"), ThisLocalUserNum);
		
			co_return;
		}

		co_await Async::PlatformSeconds(UE5CoroOSS::GetTimeout());

		co_return;
	}(FLambdaParam(Get()), LocalUserNum, ListName);

	static int32 LocalUserNum2;
	static bool bWasSuccessful;
	static FString ListName2;
	static FString ErrorStr;

	const TCoroutine<> DoReadFriendsList = []
		(int32& OutLocalUserNum, bool& bOutWasSuccessful, FString& OutListName, FString& OutErrorStr) -> TCoroutine<>
	{
		auto [InnerLocalUserNum, bInnerWasSuccessful, bInnerListName, bInnerErrorStr] = co_await ReadFriendsListDelegate;

		OutLocalUserNum = InnerLocalUserNum;
		bOutWasSuccessful = bInnerWasSuccessful;
		OutListName = bInnerListName;
		OutErrorStr = bInnerErrorStr;
	}(LocalUserNum2, bWasSuccessful, ListName2, ErrorStr);

	if (const int32 Success = co_await Race(ReadFriendsListCoroutine, DoReadFriendsList); !Success)
	{
		co_return {};
	}

	T&& Return = ForwardAsTuple(LocalUserNum2, bWasSuccessful, ListName2, ErrorStr);
	
	co_return Return;
}
