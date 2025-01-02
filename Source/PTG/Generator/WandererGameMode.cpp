// Fill out your copyright notice in the Description page of Project Settings.


#include "PTG/Generator/WandererGameMode.h"

AWandererGameMode::AWandererGameMode()
{// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/BP_Wanderer"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

}

void AWandererGameMode::BeginPlay()
{
	Super::BeginPlay();
}

void AWandererGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
