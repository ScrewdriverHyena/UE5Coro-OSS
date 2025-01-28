// Copyright No Bright Shadows. All Rights Reserved.

#include "PlayFabHelpers/AsyncPlayFabAuthentication.h"
#include "PlayFab.h"
#include "PlayFabAuthenticationInstanceAPI.h"

UAsyncPlayFabAuthentication::UAsyncPlayFabAuthentication() = default;

void UAsyncPlayFabAuthentication::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	AuthenticationAPI = IPlayFabModuleInterface::Get().GetAuthenticationAPI();
}

UAsyncPlayFabAuthentication* UAsyncPlayFabAuthentication::Get(const UObject* WorldContext)
{
	return WorldContext->GetWorld()->GetGameInstance()->GetSubsystem<UAsyncPlayFabAuthentication>();
}

TCoroutine<TOptional<FGetEntityTokenUnion>> UAsyncPlayFabAuthentication::GetEntityToken(
	PlayFab::AuthenticationModels::FGetEntityTokenRequest Request, const FForceLatentCoroutine)
{
	PlayFab::AuthenticationModels::FGetEntityTokenResponse Response;
	PlayFab::FPlayFabCppError Error;

	PlayFab::UPlayFabAuthenticationInstanceAPI::FGetEntityTokenDelegate SuccessDelegate;
	PlayFab::FPlayFabErrorDelegate ErrorDelegate;

	const TCoroutine<> OnGetEntityToken = []
		(const FLatentLambda, PlayFab::UPlayFabAuthenticationInstanceAPI::FGetEntityTokenDelegate& ThisSuccessDelegate,
			PlayFab::AuthenticationModels::FGetEntityTokenResponse& ThisResponse) -> TCoroutine<>
	{
		auto [InnerResponse] = co_await ThisSuccessDelegate;

		ThisResponse = InnerResponse;
	}(FLambdaParam(this), SuccessDelegate, Response);

	const TCoroutine<> OnError = []
		(const FLatentLambda, PlayFab::FPlayFabErrorDelegate& ThisErrorDelegate,
			PlayFab::FPlayFabCppError& ThisError) -> TCoroutine<>
	{
		auto [InnerError] = co_await ThisErrorDelegate;

		ThisError = InnerError;
	}(FLambdaParam(this), ErrorDelegate, Error);

	if (!AuthenticationAPI->GetEntityToken(Request, SuccessDelegate, ErrorDelegate))
	{
		co_return {};
	}

	co_return { !co_await Race(OnGetEntityToken, OnError) ? FGetEntityTokenUnion(Response) : FGetEntityTokenUnion(Error) };
}
