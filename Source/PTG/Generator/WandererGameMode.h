#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "WandererGameMode.generated.h"

UCLASS()
class PTG_API AWandererGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	AWandererGameMode();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
};
