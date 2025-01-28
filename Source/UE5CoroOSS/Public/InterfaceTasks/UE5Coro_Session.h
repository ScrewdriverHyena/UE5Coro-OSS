// Copyright No Bright Shadows. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UE5Coro.h"
#include "UE5CoroOSS_Shared.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Types/OSBSessionTypes.h"
#include "UE5Coro_Session.generated.h"

UCLASS()
class UE5COROOSS_API UAsyncSession final : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:

	UAsyncSession();

	//~UGameInstanceSubsystem Interface Begin
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	
	virtual void Deinitialize() override;
	//~UGameInstanceSubsystem Interface End

	static UAsyncSession* Get(const UObject* WorldContext = GEngine->GameViewport.Get());

	/**
	 * @brief	Create a new session.
	 *
	 * @param LocalUserNum			User initiating the request.		
	 * @param SessionName			Name of session to create.
	 * @param Settings				Session settings.
	 * @param ForceLatentCoroutine	Forces a latent coroutine. Do not set.
	 *
	 * @return	See: FOnCreateSessionCompleteDelegate.
	 */
	template <typename T = TTuple<FName /*SessionName*/, bool /*bWasSuccessful*/>>
	TCoroutine<TOptional<T>> CreateSession(const FPlatformUserId LocalUserNum, const FName SessionName, const FOnlineSessionSettings& Settings, 
		const FForceLatentCoroutine ForceLatentCoroutine = {});

	/**
	 * @brief	Find a single advertised session by session ID.
	 *
	 * @param SearchingUserId		User initiating the request.
	 * @param SessionId				Session ID to search for.
	 * @param FriendId				Optional ID of user to verify in session.
	 * @param ForceLatentCoroutine	Forces a latent coroutine. Do not set.
	 *
	 * @return	See: FOnSingleSessionResultComplete.
	 */
	template <typename T = TTuple<int /*LocalUserNum*/, bool /*bWasSuccessful*/, FOnlineSessionSearchResultBP /*SearchResult*/>>
	TCoroutine<TOptional<T>> FindSessionById(const FUniqueNetId& SearchingUserId, const FUniqueNetId& SessionId,
		const FUniqueNetId& FriendId, const FForceLatentCoroutine ForceLatentCoroutine = {});

	/**
     * @brief	Joins the session specified.
     *
     * @param LocalUserId			The ID of the player searching for a match.
     * @param SessionName			The name of the session to join.
     * @param DesiredSession		The desired session to join.
     * @param ForceLatentCoroutine	Forces a latent coroutine. Do not set.
     *
	* @return	See: FOnJoinSessionCompleteDelegate.
     */
	template <typename T = TTuple<FName /*SessionName*/, EOnJoinSessionCompleteResult::Type /*Result*/>>
	TCoroutine<TOptional<T>> JoinSession(const FUniqueNetId& LocalUserId, const FName SessionName,
		const FOnlineSessionSearchResult& DesiredSession, const FForceLatentCoroutine ForceLatentCoroutine = {});

	/**
	 * @brief	Marks an online session as having been ended.
	 *
	 * @param SessionName			The name of the session to end.
	 * @param ForceLatentCoroutine	Forces a latent coroutine. Do not set.
	 *
	 * @return	See: FOnEndSessionCompleteDelegate.
	 */
	template <typename T = TTuple<FName /*SessionName*/, bool /*bWasSuccessful*/>>
	TCoroutine<TOptional<T>> EndSession(const FName SessionName, const FForceLatentCoroutine ForceLatentCoroutine = {});

	/**
	 * @brief	Destroys the specified online session.
	 * 
	 * @note	Online session unregistration is an async process and does not complete
	 *			until the OnDestroySessionComplete delegate is called.
	 *
	 * @param SessionName			The name of the session to delete.
	 * @param ForceLatentCoroutine	Forces a latent coroutine. Do not set.
	 *
	 * @return	See: FOnDestroySessionCompleteDelegate.
	 */
	template <typename T = TTuple<FName /*SessionName*/, bool /*bWasSuccessful*/>>
	TCoroutine<TOptional<T>> DestroySession(const FName SessionName, const FForceLatentCoroutine ForceLatentCoroutine = {});

private:

	static inline TWeakPtr<IOnlineSession> Session;

	static inline FOnSingleSessionResultComplete::FDelegate FindSessionByIdDelegate;
};

template <typename T>
TCoroutine<TOptional<T>> UAsyncSession::CreateSession(const FPlatformUserId LocalUserNum, const FName SessionName,
	const FOnlineSessionSettings& Settings, const FForceLatentCoroutine)
{
	if (!ensureMsgf(Session.Pin()->GetSessionState(SessionName) != EOnlineSessionState::InProgress,
		TEXT("Attempted to create session while session already in progress!")))
	{
		co_return {};
	}
	
	static FName SessionName2;
	static bool bWasSuccessful;

	FOnCreateSessionCompleteDelegate CreateSessionCompleteDelegate;

	const TCoroutine<> DoCreateSession = []
		(const FLatentLambda, FOnCreateSessionCompleteDelegate& ThisCreateSessionCompleteDelegate,
			FName& OutSessionName, bool& OutbWasSuccessful) -> TCoroutine<>
	{
		auto [InnerSessionName, bInnerWasSuccessful] = co_await ThisCreateSessionCompleteDelegate;

		Session.Pin()->OnCreateSessionCompleteDelegates.Clear();

		OutSessionName = InnerSessionName;
		OutbWasSuccessful = bInnerWasSuccessful;
	}(FLambdaParam(this), CreateSessionCompleteDelegate, SessionName2, bWasSuccessful);
	
	const TCoroutine<> CreateSessionCoroutine = []
		(const FLatentLambda, FOnCreateSessionCompleteDelegate& ThisCreateSessionCompleteDelegate, const FPlatformUserId ThisLocalUserNum,
			const FName ThisSessionName, const FOnlineSessionSettings ThisSettings) -> TCoroutine<>
	{
		co_await Latent::Until(std::function([ThisCreateSessionCompleteDelegate]
			() -> bool
		{
			return ThisCreateSessionCompleteDelegate.IsBound();
		}));

		if (!Session.IsValid())
		{
			co_return;
		}
	
		Session.Pin()->AddOnCreateSessionCompleteDelegate_Handle(ThisCreateSessionCompleteDelegate);
		
		if (!Session.Pin()->CreateSession(ThisLocalUserNum, ThisSessionName, ThisSettings))
		{
			co_return;
		}

		co_await Async::PlatformSeconds(UE5CoroOSS::GetTimeout());
	}(FLambdaParam(this), CreateSessionCompleteDelegate, LocalUserNum, SessionName, Settings);

	co_return !co_await Race(CreateSessionCoroutine, DoCreateSession) ? Session.Pin()->OnCreateSessionCompleteDelegates.Clear(), TOptional<T>()
		: T(SessionName2, bWasSuccessful);
}

template <typename T>
TCoroutine<TOptional<T>> UAsyncSession::FindSessionById(const FUniqueNetId& SearchingUserId, const FUniqueNetId& SessionId,
	const FUniqueNetId& FriendId, const FForceLatentCoroutine)
{
	static int32 LocalUserNum;
	static bool bWasSuccessful;
	static FOnlineSessionSearchResult SearchResult;

	const TCoroutine<> DoFindSessionById = []
		(const FLatentLambda, int32& OutLocalUserNum, bool& OutbWasSuccessful, FOnlineSessionSearchResult& OutSearchResult) -> TCoroutine<>
	{
		auto [InnerLocalUserNum, InnerbWasSuccessful, InnerSearchResult] = co_await FindSessionByIdDelegate;

		OutLocalUserNum = InnerLocalUserNum;
		OutbWasSuccessful = InnerbWasSuccessful;
		OutSearchResult = InnerSearchResult;
	}(FLambdaParam(this), LocalUserNum, bWasSuccessful, SearchResult);
	
	const TCoroutine<> FindSessionCoroutine = []
		(const FLatentLambda, const FUniqueNetId& ThisSearchingUserId, const FUniqueNetId& ThisSessionId, const FUniqueNetId& ThisFriendId)
			-> TCoroutine<>
	{
		co_await Latent::Until(std::function([]
			() -> bool
		{
			return FindSessionByIdDelegate.IsBound();
		}));

		if (!Session.IsValid())
		{
			co_return;
		}
	
		if (!Session.Pin()->FindSessionById(ThisSearchingUserId, ThisSessionId, ThisFriendId, FindSessionByIdDelegate))
		{
			co_return;
		}

		co_await Async::PlatformSeconds(UE5CoroOSS::GetTimeout());
	}(FLambdaParam(this), SearchingUserId, SessionId, FriendId);
	
	co_return !co_await Race(FindSessionCoroutine, DoFindSessionById) ? TOptional<T>()
		: T(LocalUserNum, bWasSuccessful, FOnlineSessionSearchResultBP::FromNative(SearchResult));
}

template <typename T>
TCoroutine<TOptional<T>> UAsyncSession::JoinSession(const FUniqueNetId& LocalUserId, const FName SessionName,
	const FOnlineSessionSearchResult& DesiredSession, const FForceLatentCoroutine)
{
	static FName SessionName2;
	static EOnJoinSessionCompleteResult::Type JoinSessionCompleteResult;

	FOnJoinSessionCompleteDelegate JoinSessionCompleteDelegate;

	const TCoroutine<> DoJoinSession = []
		(const FLatentLambda, FOnJoinSessionCompleteDelegate& ThisJoinSessionCompleteDelegate, FName& OutSessionName,
			EOnJoinSessionCompleteResult::Type& OutResult) -> TCoroutine<>
	{
		auto [InnerSessionName, InnerResult] = co_await ThisJoinSessionCompleteDelegate;

		OutSessionName = InnerSessionName;
		OutResult = InnerResult;
	}(FLambdaParam(this), JoinSessionCompleteDelegate, SessionName2, JoinSessionCompleteResult);
	
	const TCoroutine<> JoinSessionCoroutine = []
		(const FLatentLambda, FOnJoinSessionCompleteDelegate& ThisJoinSessionCompleteDelegate, const FUniqueNetId& ThisLocalUserId,
			const FName ThisSessionName, const FOnlineSessionSearchResult& ThisDesiredSession) -> TCoroutine<>
	{
		co_await Latent::Until(std::function([ThisJoinSessionCompleteDelegate]
			() -> bool
		{
			return ThisJoinSessionCompleteDelegate.IsBound();
		}));

		if (!Session.IsValid())
		{
			co_return;
		}
	
		Session.Pin()->AddOnJoinSessionCompleteDelegate_Handle(ThisJoinSessionCompleteDelegate);
		
		if (!Session.Pin()->JoinSession(ThisLocalUserId, ThisSessionName, ThisDesiredSession))
		{
			co_return;
		}

		co_await Async::PlatformSeconds(UE5CoroOSS::GetTimeout());
	}(FLambdaParam(this), JoinSessionCompleteDelegate, LocalUserId, SessionName, DesiredSession);

	co_return !co_await Race(JoinSessionCoroutine, DoJoinSession) ? TOptional<T>() : T(SessionName2, JoinSessionCompleteResult);
}

template <typename T>
TCoroutine<TOptional<T>> UAsyncSession::EndSession(const FName SessionName, const FForceLatentCoroutine)
{
	static FName SessionName2;
	static bool bWasSuccessful;

	FOnEndSessionCompleteDelegate EndSessionCompleteDelegate;

	const TCoroutine<> DoEndSession = []
		(const FLatentLambda, FOnEndSessionCompleteDelegate& ThisEndSessionCompleteDelegate, FName& OutSessionName, bool& OutbWasSuccessful)
			-> TCoroutine<>
	{
		auto [InnerSessionName, bInnerWasSuccessful] = co_await ThisEndSessionCompleteDelegate;

		OutSessionName = InnerSessionName;
		OutbWasSuccessful = bInnerWasSuccessful;
	}(FLambdaParam(this), EndSessionCompleteDelegate, SessionName2, bWasSuccessful);
	
	const TCoroutine<> EndSessionCoroutine = []
		(const FLatentLambda, FOnEndSessionCompleteDelegate& ThisEndSessionCompleteDelegate, const FName ThisSessionName) -> TCoroutine<>
	{
		co_await Latent::Until(std::function([ThisEndSessionCompleteDelegate]
			() -> bool
		{
			return ThisEndSessionCompleteDelegate.IsBound();
		}));

		if (!Session.IsValid())
		{
			co_return;
		}
	
		Session.Pin()->AddOnEndSessionCompleteDelegate_Handle(ThisEndSessionCompleteDelegate);
		
		if (!Session.Pin()->EndSession(ThisSessionName))
		{
			co_return;
		}

		co_await Async::PlatformSeconds(UE5CoroOSS::GetTimeout());
	}(FLambdaParam(this), EndSessionCompleteDelegate, SessionName);

	co_return !co_await Race(EndSessionCoroutine, DoEndSession) ? TOptional<T>() : T(SessionName2, bWasSuccessful);
}

template <typename T>
TCoroutine<TOptional<T>> UAsyncSession::DestroySession(const FName SessionName, const FForceLatentCoroutine)
{
	static FName SessionName2;
	static bool bWasSuccessful;

	const FOnDestroySessionCompleteDelegate DestroySessionCompleteDelegate;

	const TCoroutine<> DoDestroySession = []
		(const FLatentLambda, FOnDestroySessionCompleteDelegate ThisDestroySessionCompleteDelegate, FName& OutSessionName, bool& OutbWasSuccessful)
			-> TCoroutine<>
	{
		auto [InnerSessionName, bInnerWasSuccessful] = co_await ThisDestroySessionCompleteDelegate;

		OutSessionName = InnerSessionName;
		OutbWasSuccessful = bInnerWasSuccessful;
	}(FLambdaParam(this), DestroySessionCompleteDelegate, SessionName2, bWasSuccessful);
	
	const TCoroutine<> DestroySessionCoroutine = []
		(const FLatentLambda, FOnDestroySessionCompleteDelegate ThisDestroySessionCompleteDelegate, const FName ThisSessionName) -> TCoroutine<>
	{
		co_await Latent::Until(std::function([ThisDestroySessionCompleteDelegate]
			() -> bool
		{
			return ThisDestroySessionCompleteDelegate.IsBound();
		}));

		if (!Session.IsValid())
		{
			co_return;
		}
	
		Session.Pin()->AddOnDestroySessionCompleteDelegate_Handle(ThisDestroySessionCompleteDelegate);
		
		if (!Session.Pin()->DestroySession(ThisSessionName))
		{
			co_return;
		}

		co_await Async::PlatformSeconds(UE5CoroOSS::GetTimeout());
	}(FLambdaParam(Get()), DestroySessionCompleteDelegate, SessionName);

	co_return !co_await Race(DestroySessionCoroutine, DoDestroySession) ? TOptional<T>() : T(SessionName2, bWasSuccessful);
}