// Copyright No Bright Shadows. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Containers/Union.h"
#include "UE5Coro.h"
#include "Core/PlayFabClientAPI.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "AsyncPlayFabClient.generated.h"

typedef TUnion<PlayFab::ClientModels::FLoginResult, PlayFab::FPlayFabCppError> FLoginUnion;
typedef TUnion<PlayFab::ClientModels::FGetUserDataResult, PlayFab::FPlayFabCppError> FGetUserDataUnion;
typedef TUnion<PlayFab::ClientModels::FGetTitleDataResult, PlayFab::FPlayFabCppError> FTitleDataUnion;
typedef TUnion<PlayFab::ClientModels::FGetTitleNewsResult, PlayFab::FPlayFabCppError> FTitleNewsUnion;
typedef TUnion<PlayFab::ClientModels::FUpdateUserDataResult, PlayFab::FPlayFabCppError> FUpdateUserDataUnion;

UCLASS()
class UE5COROOSS_API UAsyncPlayFabClient final : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:

	UAsyncPlayFabClient();
	
	//~USubsystem Interface Begin
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	//~USubsystem Interface End

	static UAsyncPlayFabClient* Get(const UObject* WorldContext = GEngine->GameViewport);

	/**
	 * @brief	Logs in a user with an Open ID Connect JWT created by an existing relationship between a title and
	 *			an Open ID Connect provider.
	 *
	 * @param Request				PlayFab::ClientModels::FLoginWithOpenIdConnectRequest
	 * @param ForceLatentCoroutine	Do not set. Forces latent coroutine.
	 *
	 * @return	When awaited, returns an optional union with either the result object or an error object. If unset,
	 *			the request failed to start.
	 */
	TCoroutine<TOptional<FLoginUnion>> LoginWithOpenIdConnect(PlayFab::ClientModels::FLoginWithOpenIdConnectRequest Request,
		const TLatentContext<> ForceLatentCoroutine = {GWorld, GWorld});

	/**
	 * @brief	Signs the user in using a Steam authentication ticket, returning a session identifier that can
	 *			subsequently be used for API calls which require an authenticated user Steam sign-in is accomplished
	 *			with the Steam Session Ticket.
	 *
	 *	More information on the Ticket can be found in the Steamworks SDK, here: https://partner.steamgames.com/documentation/auth.
	 *
	 *	@note	For Steam authentication to work, the title must be configured with the Steam Application ID and Web
	 *			API Key in the PlayFab Game Manager (under Steam in the Add-ons Marketplace). You can obtain a Web API
	 *			Key from the Permissions page of any Group associated with your App ID in the Steamworks site.
	 *
	 *	If this is the first time a user has signed in with the Steam account and CreateAccount is set to true, a new
	 *	PlayFab account will be created and linked to the provided account's Steam ID. In this case, no email or
	 *	username will be associated with the PlayFab account. Otherwise, if no PlayFab account is linked to the Steam
	 *	account, an error indicating this will be returned, so that the title can guide the user through creation of
	 *	a PlayFab account.
	 *
	 * @param Request				PlayFab::ClientModels::FLoginWithSteamRequest
	 * @param ForceLatentCoroutine	Do not set. Forces latent coroutine.
	 *
	 * @return	When awaited, returns an optional union with either the result object or an error object. If unset,
	 *			the request failed to start.
	 */
	TCoroutine<TOptional<FLoginUnion>> LoginWithSteam(PlayFab::ClientModels::FLoginWithSteamRequest Request,
		const FForceLatentCoroutine ForceLatentCoroutine = {});

	/**
	 * @brief	Signs the user in using a PlayStation :tm: Network authentication code, returning a session identifier
	 *			that can subsequently be used for API calls which require an authenticated user.
	 *			
	 *	If this is the first time a user has signed in with the PlayStation :tm: Network account and CreateAccount is
	 *	set to true, a new PlayFab account will be created and linked to the PlayStation :tm: Network account. In this
	 *	case, no email or username will be associated with the PlayFab account. Otherwise, if no PlayFab account is
	 *	linked to the PlayStation :tm: Network account, an error indicating this will be returned, so that the title
	 *	can guide the user through creation of a PlayFab account.
	 *
	 * @param Request				PlayFab::ClientModels::FLoginWithPSNRequest
	 * @param ForceLatentCoroutine	Do not set. Forces latent coroutine.
	 *
	 * @return	When awaited, returns an optional union with either the result object or an error object. If unset,
	 *			the request failed to start.
	 */
	TCoroutine<TOptional<FLoginUnion>> LoginWithPSN(PlayFab::ClientModels::FLoginWithPSNRequest Request,
		const FForceLatentCoroutine ForceLatentCoroutine = {});

	/**
	 * @brief	Retrieves the title-specific custom data for the user which is readable and writable by the client.
	 *
	 *	Data is stored as JSON key-value pairs. Every time the data is updated via any source, the version counter
	 *	is incremented. If the Version parameter is provided, then this call will only return data if the current
	 *	version on the system is greater than the value provided. If the Keys parameter is provided, the data object
	 *	returned will only contain the data specific to the indicated Keys. Otherwise, the full set of custom user
	 *	data will be returned.
	 *
	 * @param Request				PlayFab::ClientModels::FGetUserDataRequest
	 * @param ForceLatentCoroutine	Do not set. Forces latent coroutine.
	 *
	 * @return	When awaited, returns an optional union with either the result object or an error object. If unset,
	 *			the request failed to start.
	 */
	TCoroutine<TOptional<FGetUserDataUnion>> GetUserData(PlayFab::ClientModels::FGetUserDataRequest Request,
		const FForceLatentCoroutine ForceLatentCoroutine = {});

	/**
	 * @brief	Retrieves the key-value store of custom title settings
	 * 
	 *	This API is designed to return title specific values which can be read, but not written to, by the client.
	 *	For example, a developer could choose to store values which modify the user experience, such as enemy spawn
	 *	rates, weapon strengths, movement speeds, etc. This allows a developer to update the title without the need
	 *	to create, test, and ship a new build. If the player belongs to an experiment variant that uses title data
	 *	overrides, the overrides are applied automatically and returned with the title data. Note that there may up
	 *	to a minute delay in between updating title data and this API call returning the newest value.
	 *
	 * @param Request				PlayFab::ClientModels::FGetTitleDataRequest
	 * @param ForceLatentCoroutine	Do not set. Forces latent coroutine.
	 *
	 * @return	When awaited, returns an optional union with either the result object or an error object. If unset,
	 *			the request failed to start.
	 */
	TCoroutine<TOptional<FTitleDataUnion>> GetTitleData(PlayFab::ClientModels::FGetTitleDataRequest Request,
		const FForceLatentCoroutine ForceLatentCoroutine = {});

	/**
	 * @brief	Retrieves the title news feed, as configured in the PlayFab developer portal.
	 * 
	 * @param Request				PlayFab::ClientModels::FGetTitleNewsRequest
	 * @param ForceLatentCoroutine	Do not set. Forces latent coroutine.
	 * 
	 * @return	When awaited, returns an optional union with either the result object or an error object. If unset,
	 *			the request failed to start.
	 */
	TCoroutine<TOptional<FTitleNewsUnion>> GetTitleNews(PlayFab::ClientModels::FGetTitleNewsRequest Request,
		const FForceLatentCoroutine ForceLatentCoroutine = {});

	/**
	 * @brief	Creates and updates the title-specific custom data for the user which is readable and writable by
	 *			the client.
	 *
	 *	This function performs an additive update of the arbitrary strings containing the custom data for the user.
	 *	In updating the custom data object, keys which already exist in the object will have their values overwritten,
	 *	while keys with null values will be removed. New keys will be added, with the given values. No other key-value
	 *	pairs will be changed apart from those specified in the call.
	 *
	 * @param Request				PlayFab::ClientModels::FUpdateUserDataRequest
	 * @param ForceLatentCoroutine	Do not set. Forces latent coroutine.
	 *
	 * @return	When awaited, returns an optional union with either the result object or an error object. If unset,
	 *			the request failed to start.
	 */
	TCoroutine<TOptional<FUpdateUserDataUnion>> UpdateUserData(PlayFab::ClientModels::FUpdateUserDataRequest Request,
		const FForceLatentCoroutine ForceLatentCoroutine = {});

private:

	TSharedPtr<PlayFab::UPlayFabClientAPI> ClientAPI;
};
