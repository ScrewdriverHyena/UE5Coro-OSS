// Copyright No Bright Shadows. All Rights Reserved.

#include "PlayFabHelpers/AsyncPlayFabProfiles.h"

#include "PlayFab.h"
#include "PlayFabProfilesInstanceAPI.h"

UAsyncPlayFabProfiles::UAsyncPlayFabProfiles() = default;

void UAsyncPlayFabProfiles::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	ProfilesAPI = IPlayFabModuleInterface::Get().GetProfilesAPI();
}

UAsyncPlayFabProfiles* UAsyncPlayFabProfiles::Get(const UObject* WorldContext)
{
	return WorldContext->GetWorld()->GetGameInstance()->GetSubsystem<UAsyncPlayFabProfiles>();
}

TCoroutine<TOptional<FTitlePlayersUnion>> UAsyncPlayFabProfiles::GetTitlePlayersFromMasterPlayerAccountIds(
	PlayFab::ProfilesModels::FGetTitlePlayersFromMasterPlayerAccountIdsRequest& Request, const FForceLatentCoroutine)
{
	PlayFab::ProfilesModels::FGetTitlePlayersFromMasterPlayerAccountIdsResponse Response;
	PlayFab::FPlayFabCppError Error;

	PlayFab::UPlayFabProfilesInstanceAPI::FGetTitlePlayersFromMasterPlayerAccountIdsDelegate SuccessDelegate;
	PlayFab::FPlayFabErrorDelegate ErrorDelegate;

	const TCoroutine<> OnGetTitlePlayersFromMasterPlayerAccountIds = []
	(const FLatentLambda, PlayFab::UPlayFabProfilesInstanceAPI::FGetTitlePlayersFromMasterPlayerAccountIdsDelegate& ThisSuccessDelegate,
		PlayFab::ProfilesModels::FGetTitlePlayersFromMasterPlayerAccountIdsResponse& ThisResponse) -> TCoroutine<>
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

	if (!ProfilesAPI->GetTitlePlayersFromMasterPlayerAccountIds(Request, SuccessDelegate, ErrorDelegate))
	{
		co_return {};
	}

	co_return { !co_await Race(OnGetTitlePlayersFromMasterPlayerAccountIds, OnError) ? FTitlePlayersUnion(Response) : FTitlePlayersUnion(Error) };
}
