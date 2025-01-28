// Copyright No Bright Shadows. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

namespace UE5CoroOSS
{
	/**
	 * @brief	Get the current timeout value to use for async OSS operations.
	 *
	 * @return	Timeout value in seconds.
	 */
	double UE5COROOSS_API GetTimeout();

	/**
	 * @brief	Get the current timeout value for autologin, to allow the player to log in to EOS.
	 *
	 * @return	Timeout value in seconds.
	 */
	double UE5COROOSS_API GetLoginTimeout();
} // namespace UE5CoroOSS
