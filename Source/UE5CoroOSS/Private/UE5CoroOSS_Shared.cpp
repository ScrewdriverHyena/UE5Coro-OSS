// Copyright No Bright Shadows. All Rights Reserved.

#include "UE5CoroOSS_Shared.h"

namespace UE5CoroOSS
{
	namespace Private
	{
		constexpr float DEFAULT_ASYNC_TIMEOUT = 10.0f;

		float AsyncTimeout = DEFAULT_ASYNC_TIMEOUT;
		FAutoConsoleVariableRef CVarAsyncTimeout(
			TEXT("oss.asynctimeout"),
			AsyncTimeout,
			TEXT("Time to wait before abandoning an async OSS operation."));

		constexpr float DEFAULT_ASYNC_LOGIN_TIMEOUT = 540.0f;

		float AsyncLoginTimeout = DEFAULT_ASYNC_LOGIN_TIMEOUT;
		FAutoConsoleVariableRef CVarAsyncLoginTimeout(
			TEXT("oss.asynclogintimeout"),
			AsyncLoginTimeout,
			TEXT("Time to wait before abandoning an async AutoLogin operation."));
	} // namespace Private

	double GetTimeout()
	{
		return Private::CVarAsyncTimeout->IsVariableFloat() ? Private::CVarAsyncTimeout->GetFloat() : Private::DEFAULT_ASYNC_TIMEOUT;
	}

	double GetLoginTimeout()
	{
		return Private::CVarAsyncLoginTimeout->IsVariableFloat() ? Private::CVarAsyncLoginTimeout->GetFloat() : Private::DEFAULT_ASYNC_LOGIN_TIMEOUT;
	}
} // namespace UE5CoroOSS