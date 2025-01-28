// Copyright No Bright Shadows. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UE5Coro.h"
#include "UE5CoroOSS_Shared.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "UE5Coro_Identity.generated.h"

UCLASS()
class UE5COROOSS_API UAsyncIdentity final : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:

	using FLatentAwaiter = Private::FLatentAwaiter;

	UAsyncIdentity();

	//~ UGameInstanceSubsystem Interface Begin
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	
	virtual void Deinitialize() override;
	//~ UGameInstanceSubsystem Interface End

	static UAsyncIdentity* Get();

	static void SetInterfaceName(const FName& Name = FName(TEXT("RedpointEOS")));

	/**
	 * @brief	Logs the player into the online service using parameters passed on the
	 *			command line. Expects -AUTH_LOGIN=<UserName> -AUTH_PASSWORD=<password>. If either
	 *			is missing, the function returns false and doesn't start the login process.
	 *
	 * @param PlatformUser			The Platform User to login.
	 * @param ForceLatentCoroutine	Forces a latent coroutine. Do not set.
	 *
	 * @return See: FOnLoginCompleteDelegate
	 */
	template <typename T = TTuple<const FPlatformUserId& /*PlatformUser*/, bool /*bWasSuccessful*/, const FUniqueNetId& /*LocalUserId*/,
		const FString& /*Error*/>>
	TCoroutine<TOptional<T>> AutoLogin(const FPlatformUserId& PlatformUser, const FForceLatentCoroutine ForceLatentCoroutine = {});

	/**
	 * @brief	Signs the player out of the online service.
	 *
	 * @param PlatformUser			The Platform User to logout.
	 * @param ForceLatentCoroutine	Forces a latent coroutine. Do not set.
	 *
	 * @return See: FOnLogoutCompleteDelegate
	 */
	template <typename T = TTuple<int32 /*LocalUserNum*/, bool /*bWasSuccessful*/>>
	TCoroutine<TOptional<T>> Logout(const FPlatformUserId& PlatformUser, const FForceLatentCoroutine ForceLatentCoroutine = {});

	/**
	 * @brief	Gets the status of a user's privilege.
	 *
	 * @param LocalUserId	The unique id of the user to query
	 * @param Privilege		The privilege you want to know about
	 * @param ShowResolveUI Whether to show UI to resolve if the user doesn't have the given privilege
	 *
	 * @return	See: FOnGetUserPrivilegeCompleteDelegate
	 */
	template <typename T = TTuple<const FUniqueNetId& /*LocalUserId*/, EUserPrivileges::Type /*Privilege*/,
		uint32 /*PrivilegeResult*/>>
	static TCoroutine<TOptional<T>> GetUserPrivilege(const FUniqueNetId& LocalUserId, const EUserPrivileges::Type Privilege,
		const EShowPrivilegeResolveUI ShowResolveUI = EShowPrivilegeResolveUI::Default);

private:

	static inline TWeakPtr<IOnlineIdentity> Identity;

	static inline IOnlineIdentity::FOnGetUserPrivilegeCompleteDelegate GetUserPrivilegeDelegate;

	static inline FOnLogoutCompleteDelegate LogoutDelegate;
};

template <typename T>
TCoroutine<TOptional<T>> UAsyncIdentity::AutoLogin(const FPlatformUserId& PlatformUser, const FForceLatentCoroutine)
{
	FOnLoginCompleteDelegate AutoLoginDelegate;

	static int LocalUserNum = 0;
	static bool bWasSuccessful = false;
	static FUniqueNetIdRepl UserId;
	static FString Error;

	const TCoroutine<> DoAutoLogin = []
		(const FLatentLambda, int& OutLocalUserNum, bool& bOutWasSuccessful, FUniqueNetIdRepl& OutUserId, FString& OutError,
			FOnLoginCompleteDelegate& ThisAutoLoginDelegate) -> TCoroutine<>
	{
		auto [InnerLocalUserNum, bInnerWasSuccesful, InnerUserId, InnerError] = co_await ThisAutoLoginDelegate;

		Identity.Pin()->OnLoginCompleteDelegates->Clear();

		OutLocalUserNum = InnerLocalUserNum;
		bOutWasSuccessful = bInnerWasSuccesful;
		OutUserId = InnerUserId;
		OutError = InnerError;
	}(FLambdaParam(this), LocalUserNum, bWasSuccessful, UserId, Error, AutoLoginDelegate);
	
	const TCoroutine<> AutoLoginCoroutine = []
		(const FLatentLambda This, const FPlatformUserId& ThisPlatformUser, 
			FOnLoginCompleteDelegate& ThisAutoLoginDelegate) -> TCoroutine<>
	{
		co_await Latent::Until([This, ThisAutoLoginDelegate]
			() -> bool
		{
			return ThisAutoLoginDelegate.IsBound();
		});

		if (!Identity.IsValid())
		{
			co_return;
		}

		Identity.Pin()->AddOnLoginCompleteDelegate_Handle(ThisPlatformUser.GetInternalId(), ThisAutoLoginDelegate);
	
		if (!Identity.Pin()->AutoLogin(ThisPlatformUser.GetInternalId()))
		{
			co_return;
		}

		co_await Latent::RealSeconds(UE5CoroOSS::GetLoginTimeout());

		co_return;
	}(FLambdaParam(this), PlatformUser, AutoLoginDelegate);

	if (const int32 Success = co_await Race(AutoLoginCoroutine, DoAutoLogin); !Success)
	{
		co_return {};
	}

	T&& Return = ForwardAsTuple(FPlatformUserId::CreateFromInternalId(LocalUserNum), bWasSuccessful, *UserId, Error);

	co_return Return;
}

template <typename T>
TCoroutine<TOptional<T>> UAsyncIdentity::Logout(const FPlatformUserId& PlatformUser, const FForceLatentCoroutine)
{
	const TCoroutine<> LogoutCoroutine = []
		(const FLatentLambda This, const FPlatformUserId& ThisPlatformUser) -> TCoroutine<>
	{
		co_await Latent::Until([This]
			() -> bool
		{
			return LogoutDelegate.IsBound();
		});

		if (!Identity.IsValid())
		{
			co_return;
		}

		Identity.Pin()->AddOnLogoutCompleteDelegate_Handle(ThisPlatformUser.GetInternalId(), LogoutDelegate);

		if (!Identity.Pin()->Logout(ThisPlatformUser.GetInternalId()))
		{
			co_return;
		}

		co_await Async::PlatformSeconds(UE5CoroOSS::GetTimeout());
	}(FLambdaParam(this), PlatformUser);

	int32 LocalUserNum;
	bool bWasSuccessful = false;

	const TCoroutine<> DoLogout = [](const FLatentLambda, int32& OutLocalUserNum, bool& bOutWasSuccessful) -> TCoroutine<>
	{
		auto [InnerLocalUserNum, InnerWasSuccessful] = co_await LogoutDelegate;

		OutLocalUserNum = InnerLocalUserNum;
		bOutWasSuccessful = InnerWasSuccessful;
	}(FLambdaParam(this), LocalUserNum, bWasSuccessful);

	co_return !co_await Race(LogoutCoroutine, DoLogout) ? TOptional<T>{} : TOptional<T>(ForwardAsTuple(LocalUserNum, bWasSuccessful));
}

template <typename T>
TCoroutine<TOptional<T>> UAsyncIdentity::GetUserPrivilege(const FUniqueNetId& LocalUserId, const EUserPrivileges::Type Privilege,
	const EShowPrivilegeResolveUI ShowResolveUI)
{
	[](const FLatentLambda This, const FUniqueNetId& ThisLocalUserId, const EUserPrivileges::Type ThisPrivilege,
		const EShowPrivilegeResolveUI ThisShowResolveUI) -> TCoroutine<>
	{
		co_await Latent::Until([This]
			() -> bool
		{
			return GetUserPrivilegeDelegate.IsBound();
		});
		
		Identity.Pin()->GetUserPrivilege(ThisLocalUserId, ThisPrivilege, GetUserPrivilegeDelegate, ThisShowResolveUI);
	}(FLambdaParam(Get()), LocalUserId, Privilege, ShowResolveUI);

	T Return = co_await GetUserPrivilegeDelegate;
	co_return Return;
}
