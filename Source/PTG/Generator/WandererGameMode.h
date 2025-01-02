// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "WandererGameMode.generated.h"

/**
 * 
 */
UCLASS()
class PTG_API AWandererGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	AWandererGameMode();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;


};
