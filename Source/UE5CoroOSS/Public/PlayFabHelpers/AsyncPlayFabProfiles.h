// Copyright No Bright Shadows. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PlayFabError.h"
#include "UE5Coro.h"
#include "PlayFabProfilesDataModels.h"
#include "Containers/Union.h"
#include "Core/PlayFabProfilesAPI.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "AsyncPlayFabProfiles.generated.h"

typedef TUnion<PlayFab::ProfilesModels::FGetTitlePlayersFromMasterPlayerAccountIdsResponse, PlayFab::FPlayFabCppError> FTitlePlayersUnion;

UCLASS()
class UE5COROOSS_API UAsyncPlayFabProfiles : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:

	UAsyncPlayFabProfiles();

	//~USubsystem Interface Begin
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	//~USubsystem Interface End

	static UAsyncPlayFabProfiles* Get(const UObject* WorldContext = GEngine->GameViewport);

	/**
	 * @brief	Retrieves the title player accounts associated with the given master player account.
	 * 
	 *	Given a master player account id (PlayFab ID), returns all title player accounts associated with it.
	 *
	 * @param Request				PlayFab::ProfilesModels::FGetTitlePlayersFromMasterPlayerAccountIdsRequest
	 * @param ForceLatentCoroutine	Do not set. Forces latent coroutine.
	 *
	 * @return	When awaited, returns an optional union with either the result object or an error object. If unset,
	 *			the request failed to start.
	 */
	TCoroutine<TOptional<FTitlePlayersUnion>> GetTitlePlayersFromMasterPlayerAccountIds(
		PlayFab::ProfilesModels::FGetTitlePlayersFromMasterPlayerAccountIdsRequest& Request, const FForceLatentCoroutine ForceLatentCoroutine = {});

private:

	TSharedPtr<PlayFab::UPlayFabProfilesAPI> ProfilesAPI;
};
