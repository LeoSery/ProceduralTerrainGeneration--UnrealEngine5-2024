#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ChunkManagerWorldSubsystem.h"
#include "PTGGameMode.generated.h"

class UUserWidget;
class UChunkManagerWorldSubsystem;

UCLASS()
class PTG_API APTGGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	APTGGameMode();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void GenerateInitialChunks();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UUserWidget> MainMenuClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Terrain Generation")
	FPerlinParameters TerrainParameters;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Terrain Generation")
	FPerlinParameters BiomesParameters;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Terrain Generation")
	int32 RenderDistance;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Terrain Generation")
	UMaterial* Material;
	
private:
	UPROPERTY()
	UChunkManagerWorldSubsystem* ChunkManager;

	UFUNCTION()
	void HandleGenerationProgress(int32 Current, int32 Total);
    
	void SpawnPlayer();
	void RepositionPlayerToGround();
	
};
