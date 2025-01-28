// Copyright No Bright Shadows. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UE5Coro.h"
#include "PlayFabAuthenticationDataModels.h"
#include "PlayFabError.h"
#include "Containers/Union.h"
#include "Core/PlayFabAuthenticationAPI.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "AsyncPlayFabAuthentication.generated.h"

typedef TUnion<PlayFab::AuthenticationModels::FGetEntityTokenResponse, PlayFab::FPlayFabCppError> FGetEntityTokenUnion;

UCLASS()
class UE5COROOSS_API UAsyncPlayFabAuthentication final : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:

	UAsyncPlayFabAuthentication();

	//~USubsystem Interface Begin
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	//~USubsystem Interface End

	static UAsyncPlayFabAuthentication* Get(const UObject* WorldContext = GEngine->GameViewport);

	/**
	 * @brief	Method to exchange a legacy AuthenticationTicket or title SecretKey for an Entity Token or to refresh a still valid
	 *			Entity Token.
	 * @note	API must be called with X-SecretKey, X-Authentication or X-EntityToken headers. An optional EntityKey may be
	 *			included to attempt to set the resulting EntityToken to a specific entity, however the entity must be a relation of
	 *			the caller, such as the master_player_account of a character. If sending X-EntityToken the account will be marked as
	 *			freshly logged in and will issue a new token. If using X-Authentication or X-EntityToken the header must still be
	 *			valid and cannot be expired or revoked.
	 *
	 * @param	Request					PlayFab::AuthenticationModels::FGetEntityTokenRequest
	 * @param	ForceLatentCoroutine	Do not set. Forces latent coroutine.
	 *
	 * @return	When awaited, return a union structure containing either the FGetEntityTokenResponse or FPlayFabCppError.
	 */
	TCoroutine<TOptional<FGetEntityTokenUnion>> GetEntityToken(PlayFab::AuthenticationModels::FGetEntityTokenRequest Request,
		const FForceLatentCoroutine ForceLatentCoroutine = {});

private:
	
	TSharedPtr<PlayFab::UPlayFabAuthenticationAPI> AuthenticationAPI;
};
