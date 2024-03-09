// Copyright 2023 Daniel Martinez Amigo. All Rights Reserved.


#include "DensAssetManager.h"

#include "DensGameplayTags.h"

UDensAssetManager::UDensAssetManager()
{

}



UDensAssetManager& UDensAssetManager::Get()
{
	check(GEngine);

	if (UDensAssetManager* Singleton = Cast<UDensAssetManager>(GEngine->AssetManager))
	{
		return *Singleton;
	}

	UE_LOG(LogTemp, Fatal, TEXT("Invalid AssetManagerClassName in DefaultEngine.ini.  It must be set to UDensAssetManager!"));

	// Fatal error above prevents this from being called.
	return *NewObject<UDensAssetManager>();
}

void UDensAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();

	//TODO:
		//Initialize Ability systems 
		//Initialize globals here
		//Initialize gameplay tags etc etc etc
	//FDensGameplayTags::InitializeNativeTags(); //Todo Move to function?
}