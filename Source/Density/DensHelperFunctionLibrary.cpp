// Copyright 2023 Daniel Martinez Amigo. All Rights Reserved.


#include "DensHelperFunctionLibrary.h"
#include "GameFeaturesSubsystem.h"

void UDensHelperFunctionLibrary::SetGameFeatureActive(const FString& Name, bool bNewActive)
{
	FString PluginURL;
	UGameFeaturesSubsystem::Get().GetPluginURLByName(Name, PluginURL);
	

	if (bNewActive)
	{
		UGameFeaturesSubsystem::Get().LoadAndActivateGameFeaturePlugin(PluginURL, FGameFeaturePluginLoadComplete());
	}
	else 
	{
		UGameFeaturesSubsystem::Get().UnloadGameFeaturePlugin(PluginURL, true);
	}

}