// Copyright No Bright Shadows. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Containers/Union.h"
#include "UE5Coro.h"
#include "Core/PlayFabCloudScriptAPI.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "AsyncPlayFabCloudScript.generated.h"

typedef TUnion<PlayFab::CloudScriptModels::FExecuteCloudScriptResult, PlayFab::FPlayFabCppError> FExecuteCloudScriptUnion;
typedef TUnion<PlayFab::CloudScriptModels::FExecuteFunctionResult, PlayFab::FPlayFabCppError> FExecuteFunctionUnion;

UCLASS()
class UE5COROOSS_API UAsyncPlayFabCloudScript final : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:

	UAsyncPlayFabCloudScript();
	
	//~USubsystem Interface Begin
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	//~USubsystem Interface End

	static UAsyncPlayFabCloudScript* Get(const UObject* WorldContext = GEngine->GameViewport);

	/**
	 * @brief	Executes CloudScript with the entity profile that is defined in the request.
	 * 
	 *	Cloud Script is one of PlayFab's most versatile features. It allows client code to request execution of any
	 *	kind of custom server-side functionality you can implement, and it can be used in conjunction with virtually
	 *	anything.
	 *
	 * @param Request				PlayFab::CloudScriptModels::FExecuteEntityCloudScriptRequest
	 * @param ForceLatentCoroutine	Do not set. Forces latent coroutine.
	 *
	 * @return	When awaited, returns an optional union with either the result object or an error object. If unset,
	 *			the request failed to start.
	 */
	TCoroutine<TOptional<FExecuteCloudScriptUnion>> ExecuteCloudScript(PlayFab::CloudScriptModels::FExecuteEntityCloudScriptRequest Request,
		const FForceLatentCoroutine ForceLatentCoroutine = {});

	/**
	 * @brief	Executes an Azure Function with the profile of the entity that is defined in the request.
	 * 
	 *	Cloud Script is one of PlayFab's most versatile features. It allows client code to request execution of any
	 *	kind of custom server-side functionality you can implement, and it can be used in conjunction with virtually
	 *	anything.
	 *
	 * @param Request				PlayFab::CloudScriptModels::FExecuteFunctionRequest
	 * @param ForceLatentCoroutine	Do not set. Forces latent coroutine.
	 *
	 * @return	When awaited, returns an optional union with either the result object or an error object. If unset,
	 *			the request failed to start.
	 */
	TCoroutine<TOptional<FExecuteFunctionUnion>> ExecuteFunction(PlayFab::CloudScriptModels::FExecuteFunctionRequest Request,
		const FForceLatentCoroutine ForceLatentCoroutine = {});

private:

	TSharedPtr<PlayFab::UPlayFabCloudScriptAPI> CloudScriptAPI;
};
