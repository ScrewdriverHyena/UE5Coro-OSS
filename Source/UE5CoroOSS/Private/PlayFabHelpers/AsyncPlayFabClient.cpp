// Copyright No Bright Shadows. All Rights Reserved.

#include "PlayFabHelpers/AsyncPlayFabClient.h"
#include "PlayFab.h"
#include "PlayFabClientInstanceAPI.h"

UAsyncPlayFabClient::UAsyncPlayFabClient() = default;

void UAsyncPlayFabClient::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	ClientAPI = IPlayFabModuleInterface::Get().GetClientAPI();
}

UAsyncPlayFabClient* UAsyncPlayFabClient::Get(const UObject* WorldContext)
{
	return GEngine->GameViewport->GetGameInstance()->GetSubsystem<UAsyncPlayFabClient>();
}

TCoroutine<TOptional<FLoginUnion>> UAsyncPlayFabClient::LoginWithOpenIdConnect(PlayFab::ClientModels::FLoginWithOpenIdConnectRequest Request,
	const TLatentContext<>)
{
	PlayFab::ClientModels::FLoginResult Response;
	PlayFab::FPlayFabCppError Error;

	PlayFab::UPlayFabClientInstanceAPI::FLoginWithOpenIdConnectDelegate SuccessDelegate;
	PlayFab::FPlayFabErrorDelegate ErrorDelegate;

	const TCoroutine<> OnLoginWithOpenIdConnect = []
		(const FLatentLambda, PlayFab::UPlayFabClientInstanceAPI::FLoginWithOpenIdConnectDelegate& ThisSuccessDelegate,
			PlayFab::ClientModels::FLoginResult& ThisResponse) -> TCoroutine<>
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

	if (!ClientAPI->LoginWithOpenIdConnect(Request, SuccessDelegate, ErrorDelegate))
	{
		co_return {};
	}

	co_return { !co_await Race(OnLoginWithOpenIdConnect, OnError) ? FLoginUnion(Response) : FLoginUnion(Error) };
}

TCoroutine<TOptional<FLoginUnion>> UAsyncPlayFabClient::LoginWithSteam(PlayFab::ClientModels::FLoginWithSteamRequest Request,
	const FForceLatentCoroutine)
{
	PlayFab::ClientModels::FLoginResult Response;
	PlayFab::FPlayFabCppError Error;

	PlayFab::UPlayFabClientInstanceAPI::FLoginWithSteamDelegate SuccessDelegate;
	PlayFab::FPlayFabErrorDelegate ErrorDelegate;

	const TCoroutine<> OnLoginWithSteam = []
		(const FLatentLambda, PlayFab::UPlayFabClientInstanceAPI::FLoginWithSteamDelegate& ThisSuccessDelegate,
			PlayFab::ClientModels::FLoginResult& ThisResponse) -> TCoroutine<>
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

	if (!ClientAPI->LoginWithSteam(Request, SuccessDelegate, ErrorDelegate))
	{
		co_return {};
	}

	co_return { !co_await Race(OnLoginWithSteam, OnError) ? FLoginUnion(Response) : FLoginUnion(Error) };
}

TCoroutine<TOptional<FLoginUnion>> UAsyncPlayFabClient::LoginWithPSN(PlayFab::ClientModels::FLoginWithPSNRequest Request,
	const FForceLatentCoroutine)
{
	PlayFab::ClientModels::FLoginResult Response;
	PlayFab::FPlayFabCppError Error;

	PlayFab::UPlayFabClientInstanceAPI::FLoginWithPSNDelegate SuccessDelegate;
	PlayFab::FPlayFabErrorDelegate ErrorDelegate;

	const TCoroutine<> OnLoginWithPSN = []
		(const FLatentLambda, PlayFab::UPlayFabClientInstanceAPI::FLoginWithPSNDelegate& ThisSuccessDelegate,
			PlayFab::ClientModels::FLoginResult& ThisResponse) -> TCoroutine<>
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

	if (!ClientAPI->LoginWithPSN(Request, SuccessDelegate, ErrorDelegate))
	{
		co_return {};
	}

	co_return { !co_await Race(OnLoginWithPSN, OnError) ? FLoginUnion(Response) : FLoginUnion(Error) };
}

TCoroutine<TOptional<FGetUserDataUnion>> UAsyncPlayFabClient::GetUserData(PlayFab::ClientModels::FGetUserDataRequest Request,
	const FForceLatentCoroutine)
{
	PlayFab::ClientModels::FGetUserDataResult Response;
	PlayFab::FPlayFabCppError Error;

	PlayFab::UPlayFabClientInstanceAPI::FGetUserDataDelegate SuccessDelegate;
	PlayFab::FPlayFabErrorDelegate ErrorDelegate;

	const TCoroutine<> OnGetUserData = []
		(const FLatentLambda, PlayFab::UPlayFabClientInstanceAPI::FGetUserDataDelegate& ThisSuccessDelegate,
			PlayFab::ClientModels::FGetUserDataResult& ThisResponse) -> TCoroutine<>
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

	if (!ClientAPI->GetUserData(Request, SuccessDelegate, ErrorDelegate))
	{
		co_return {};
	}

	co_return { !co_await Race(OnGetUserData, OnError) ? FGetUserDataUnion(Response) : FGetUserDataUnion(Error) };
}

TCoroutine<TOptional<FTitleDataUnion>> UAsyncPlayFabClient::GetTitleData(PlayFab::ClientModels::FGetTitleDataRequest Request,
	const FForceLatentCoroutine)
{
	PlayFab::ClientModels::FGetTitleDataResult Response;
	PlayFab::FPlayFabCppError Error;

	PlayFab::UPlayFabClientInstanceAPI::FGetTitleDataDelegate SuccessDelegate;
	PlayFab::FPlayFabErrorDelegate ErrorDelegate;

	const TCoroutine<> OnGetTitleData = []
		(const FLatentLambda, PlayFab::UPlayFabClientInstanceAPI::FGetTitleDataDelegate& ThisSuccessDelegate,
			PlayFab::ClientModels::FGetTitleDataResult& ThisResponse) -> TCoroutine<>
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

	if (!ClientAPI->GetTitleData(Request, SuccessDelegate, ErrorDelegate))
	{
		co_return {};
	}

	co_return { !co_await Race(OnGetTitleData, OnError) ? FTitleDataUnion(Response) : FTitleDataUnion(Error) };
}

TCoroutine<TOptional<FTitleNewsUnion>> UAsyncPlayFabClient::GetTitleNews(PlayFab::ClientModels::FGetTitleNewsRequest Request,
	const FForceLatentCoroutine)
{
	PlayFab::ClientModels::FGetTitleNewsResult Response;
	PlayFab::FPlayFabCppError Error;

	PlayFab::UPlayFabClientInstanceAPI::FGetTitleNewsDelegate SuccessDelegate;
	PlayFab::FPlayFabErrorDelegate ErrorDelegate;

	const TCoroutine<> OnGetTitleNews = []
		(const FLatentLambda, PlayFab::UPlayFabClientInstanceAPI::FGetTitleNewsDelegate& ThisSuccessDelegate,
			PlayFab::ClientModels::FGetTitleNewsResult& ThisResponse) -> TCoroutine<>
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

	if (!ClientAPI->GetTitleNews(Request, SuccessDelegate, ErrorDelegate))
	{
		co_return {};
	}

	co_return { !co_await Race(OnGetTitleNews, OnError) ? FTitleNewsUnion(Response) : FTitleNewsUnion(Error) };
}

TCoroutine<TOptional<FUpdateUserDataUnion>> UAsyncPlayFabClient::UpdateUserData(PlayFab::ClientModels::FUpdateUserDataRequest Request,
	const FForceLatentCoroutine)
{
	PlayFab::ClientModels::FUpdateUserDataResult Response;
	PlayFab::FPlayFabCppError Error;

	PlayFab::UPlayFabClientInstanceAPI::FUpdateUserDataDelegate SuccessDelegate;
	PlayFab::FPlayFabErrorDelegate ErrorDelegate;

	const TCoroutine<> OnUpdateUserData = []
		(const FLatentLambda, PlayFab::UPlayFabClientInstanceAPI::FUpdateUserDataDelegate& ThisSuccessDelegate,
			PlayFab::ClientModels::FUpdateUserDataResult& ThisResponse) -> TCoroutine<>
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

	if (!ClientAPI->UpdateUserData(Request, SuccessDelegate, ErrorDelegate))
	{
		co_return {};
	}

	co_return { !co_await Race(OnUpdateUserData, OnError) ? FUpdateUserDataUnion(Response) : FUpdateUserDataUnion(Error) };
}
