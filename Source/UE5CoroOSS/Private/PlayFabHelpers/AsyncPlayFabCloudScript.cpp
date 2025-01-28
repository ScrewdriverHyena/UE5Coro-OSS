// Copyright No Bright Shadows. All Rights Reserved.

#include "PlayFabHelpers/AsyncPlayFabCloudScript.h"
#include "PlayFab.h"

UAsyncPlayFabCloudScript::UAsyncPlayFabCloudScript() = default;

void UAsyncPlayFabCloudScript::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	CloudScriptAPI = IPlayFabModuleInterface::Get().GetCloudScriptAPI();
}

UAsyncPlayFabCloudScript* UAsyncPlayFabCloudScript::Get(const UObject* WorldContext)
{
	return WorldContext->GetWorld()->GetGameInstance()->GetSubsystem<UAsyncPlayFabCloudScript>();
}

TCoroutine<TOptional<FExecuteCloudScriptUnion>> UAsyncPlayFabCloudScript::ExecuteCloudScript(
	PlayFab::CloudScriptModels::FExecuteEntityCloudScriptRequest Request, const FForceLatentCoroutine)
{
	PlayFab::CloudScriptModels::FExecuteCloudScriptResult Result;
	PlayFab::FPlayFabCppError Error;

	PlayFab::UPlayFabCloudScriptAPI::FExecuteEntityCloudScriptDelegate Success;
	PlayFab::FPlayFabErrorDelegate ErrorDelegate;

	const TCoroutine<> OnExecuteCloudScript = []
		(const FLatentLambda, PlayFab::UPlayFabCloudScriptAPI::FExecuteEntityCloudScriptDelegate& ThisSuccess,
			PlayFab::CloudScriptModels::FExecuteCloudScriptResult& ThisResult) -> TCoroutine<>
	{
		auto [InnerResult] = co_await ThisSuccess;
		
		ThisResult = InnerResult;
	}(FLambdaParam(this), Success, Result);

	const TCoroutine<> OnError = []
		(const FLatentLambda, PlayFab::FPlayFabErrorDelegate& ThisErrorDelegate, PlayFab::FPlayFabCppError& ThisError) -> TCoroutine<>
	{
		auto [InnerError] = co_await ThisErrorDelegate;
		
		ThisError = InnerError;
	}(FLambdaParam(this), ErrorDelegate, Error);

	if (!CloudScriptAPI->ExecuteEntityCloudScript(Request, Success, ErrorDelegate))
	{
		co_return {};
	}

	co_return { !co_await Race(OnExecuteCloudScript, OnError) ? FExecuteCloudScriptUnion(Result) : FExecuteCloudScriptUnion(Error) };
}

TCoroutine<TOptional<FExecuteFunctionUnion>> UAsyncPlayFabCloudScript::ExecuteFunction(PlayFab::CloudScriptModels::FExecuteFunctionRequest Request,
	const FForceLatentCoroutine)
{
	PlayFab::CloudScriptModels::FExecuteFunctionResult Result;
	PlayFab::FPlayFabCppError Error;

	PlayFab::UPlayFabCloudScriptAPI::FExecuteFunctionDelegate Success;
	PlayFab::FPlayFabErrorDelegate ErrorDelegate;

	const TCoroutine<> OnExecuteFunction = []
		(const FLatentLambda, PlayFab::UPlayFabCloudScriptAPI::FExecuteFunctionDelegate& ThisSuccess,
			PlayFab::CloudScriptModels::FExecuteFunctionResult& ThisResult) -> TCoroutine<>
	{
		auto [InnerResult] = co_await ThisSuccess;
		
		ThisResult = InnerResult;
	}(FLambdaParam(this), Success, Result);

	const TCoroutine<> OnError = []
		(const FLatentLambda, PlayFab::FPlayFabErrorDelegate& ThisErrorDelegate, PlayFab::FPlayFabCppError& ThisError) -> TCoroutine<>
	{
		auto [InnerError] = co_await ThisErrorDelegate;
		
		ThisError = InnerError;
	}(FLambdaParam(this), ErrorDelegate, Error);

	if (!CloudScriptAPI->ExecuteFunction(Request, Success, ErrorDelegate))
	{
		co_return {};
	}

	co_return { !co_await Race(OnExecuteFunction, OnError) ? FExecuteFunctionUnion(Result) : FExecuteFunctionUnion(Error) };
}

