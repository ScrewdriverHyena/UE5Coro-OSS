// Copyright No Bright Shadows. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FUE5CoroOSSModule final : public IModuleInterface
{
public:

	//~IModuleInterface Interface Begin
	virtual void StartupModule() override;
	
	virtual void ShutdownModule() override;
	//~IModuleInterface Interface End
};

