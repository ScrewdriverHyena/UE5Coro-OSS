// Copyright No Bright Shadows. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Containers/Union.h"
#include "UE5Coro.h"
#include "Core/PlayFabEconomyAPI.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "AsyncPlayFabEconomy.generated.h"

typedef TUnion<PlayFab::EconomyModels::FGetItemsResponse, PlayFab::FPlayFabCppError> FItemsUnion;
typedef TUnion<PlayFab::EconomyModels::FGetInventoryItemsResponse, PlayFab::FPlayFabCppError> FInventoryItemsUnion;
typedef TUnion<PlayFab::EconomyModels::FPurchaseInventoryItemsResponse, PlayFab::FPlayFabCppError> FPurchaseInventoryItemsUnion;

UCLASS()
class UE5COROOSS_API UAsyncPlayFabEconomy final : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:

	UAsyncPlayFabEconomy();

	//~USubsystem Interface Begin
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	//~USubsystem Interface End

	static UAsyncPlayFabEconomy* Get(const UObject* WorldContext = GEngine->GameViewport);
	
	/**
	 * @brief	Retrieves items from the public catalog.
	 *
	 *	Up to 50 items can be returned at once. GetItems does not work off a cache of the Catalog and should be
	 *	used when trying to get recent item updates. However, please note that item references data is cached and
	 *	may take a few moments for changes to propagate.
	 *
	 * @param Request				PlayFab::EconomyModels::FGetItemsRequest
	 * @param ForceLatentCoroutine	Do not set. Forces latent coroutine.
	 *
	 * @return	When awaited, returns an optional union with either the result object or an error object. If unset,
	 *			the request failed to start.
	 */
	TCoroutine<TOptional<FItemsUnion>> GetItems(PlayFab::EconomyModels::FGetItemsRequest Request,
		const FForceLatentCoroutine ForceLatentCoroutine = {});

	/**
	 * @brief	Get current inventory items.
	 * 
	 *	Given an entity type, entity identifier and container details, will get the entity's inventory items.
	 *	
	 * @param Request				PlayFab::EconomyModels::FGetInventoryItemsRequest
	 * @param ForceLatentCoroutine	Do not set. Forces latent coroutine.
	 *
	 * @return	When awaited, returns an optional union with either the result object or an error object. If unset,
	 *			the request failed to start.
	 */
	TCoroutine<TOptional<FInventoryItemsUnion>> GetInventoryItems(PlayFab::EconomyModels::FGetInventoryItemsRequest Request,
		const FForceLatentCoroutine ForceLatentCoroutine = {});

	/**
	 * @brief	Purchase a single item or bundle, paying the associated price.
	 *
	 *	Up to 10,000 stacks of items can be added to a single inventory collection. Stack size is uncapped.
	 *
	 * @param Request				PlayFab::EconomyModels::FPurchaseInventoryItemsRequest
	 * @param ForceLatentCoroutine	Do not set. Forces latent coroutine.
	 *
	 *	@return	When awaited, returns an optional union with either the result object or an error object. If unset,
	 *			the request failed to start.
	 */
	TCoroutine<TOptional<FPurchaseInventoryItemsUnion>> PurchaseInventoryItems(
		PlayFab::EconomyModels::FPurchaseInventoryItemsRequest Request, const FForceLatentCoroutine ForceLatentCoroutine = {});
	
private:

	TSharedPtr<PlayFab::UPlayFabEconomyAPI> EconomyAPI;
};
