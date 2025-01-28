// Copyright No Bright Shadows. All Rights Reserved.

#include "PlayFabHelpers/AsyncPlayFabEconomy.h"
#include "Core/PlayFabEconomyInstanceAPI.h"
#include "PlayFab.h"

UAsyncPlayFabEconomy::UAsyncPlayFabEconomy() = default;

void UAsyncPlayFabEconomy::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	EconomyAPI = IPlayFabModuleInterface::Get().GetEconomyAPI();
}

UAsyncPlayFabEconomy* UAsyncPlayFabEconomy::Get(const UObject* WorldContext)
{
	return WorldContext->GetWorld()->GetGameInstance()->GetSubsystem<UAsyncPlayFabEconomy>();
}

TCoroutine<TOptional<FItemsUnion>> UAsyncPlayFabEconomy::GetItems(PlayFab::EconomyModels::FGetItemsRequest Request, const FForceLatentCoroutine)
{
	PlayFab::EconomyModels::FGetItemsResponse Response;
	PlayFab::FPlayFabCppError Error;

	PlayFab::UPlayFabEconomyInstanceAPI::FGetItemsDelegate SuccessDelegate;
	PlayFab::FPlayFabErrorDelegate ErrorDelegate;

	const TCoroutine<> OnGetItems = []
		(const FLatentLambda, PlayFab::UPlayFabEconomyInstanceAPI::FGetItemsDelegate& ThisSuccessDelegate,
			PlayFab::EconomyModels::FGetItemsResponse& ThisResponse) -> TCoroutine<>
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

	if (!EconomyAPI->GetItems(Request, SuccessDelegate, ErrorDelegate))
	{
		co_return {};
	}

	co_return { !co_await Race(OnGetItems, OnError) ? FItemsUnion(Response) : FItemsUnion(Error) };
}

TCoroutine<TOptional<FInventoryItemsUnion>> UAsyncPlayFabEconomy::GetInventoryItems(PlayFab::EconomyModels::FGetInventoryItemsRequest Request,
	const FForceLatentCoroutine)
{
	PlayFab::EconomyModels::FGetInventoryItemsResponse Response;
	PlayFab::FPlayFabCppError Error;

	PlayFab::UPlayFabEconomyInstanceAPI::FGetInventoryItemsDelegate SuccessDelegate;
	PlayFab::FPlayFabErrorDelegate ErrorDelegate;

	const TCoroutine<> OnGetInventoryItems = []
		(const FLatentLambda, PlayFab::UPlayFabEconomyInstanceAPI::FGetInventoryItemsDelegate& ThisSuccessDelegate,
			PlayFab::EconomyModels::FGetInventoryItemsResponse& ThisResponse) -> TCoroutine<>
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

	if (!EconomyAPI->GetInventoryItems(Request, SuccessDelegate, ErrorDelegate))
	{
		co_return {};
	}

	co_return { !co_await Race(OnGetInventoryItems, OnError) ? FInventoryItemsUnion(Response) : FInventoryItemsUnion(Error) };
}

TCoroutine<TOptional<FPurchaseInventoryItemsUnion>> UAsyncPlayFabEconomy::PurchaseInventoryItems(
	PlayFab::EconomyModels::FPurchaseInventoryItemsRequest Request, const FForceLatentCoroutine)
{
	PlayFab::EconomyModels::FPurchaseInventoryItemsResponse Response;
	PlayFab::FPlayFabCppError Error;

	PlayFab::UPlayFabEconomyInstanceAPI::FPurchaseInventoryItemsDelegate SuccessDelegate;
	PlayFab::FPlayFabErrorDelegate ErrorDelegate;

	const TCoroutine<> OnPurchaseInventoryItems = []
		(const FLatentLambda, PlayFab::UPlayFabEconomyInstanceAPI::FPurchaseInventoryItemsDelegate& ThisSuccessDelegate,
			PlayFab::EconomyModels::FPurchaseInventoryItemsResponse& ThisResponse) -> TCoroutine<>
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
	
	if (!EconomyAPI->PurchaseInventoryItems(Request, SuccessDelegate, ErrorDelegate))
	{
		co_return {};
	}

	co_return { !co_await Race(OnPurchaseInventoryItems, OnError) ? FPurchaseInventoryItemsUnion(Response) : FPurchaseInventoryItemsUnion(Error) };
}
