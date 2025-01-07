#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "PTG/Generation/Subsystems/ChunkManagerWorldSubsystem.h"
#include "PTGGameMode.generated.h"

class UUserWidget;
class UChunkManagerWorldSubsystem;

//////// CLASS ////////
/// Main game mode class handling terrain generation and player management
UCLASS()
class PTG_API APTGGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	//////// CONSTRUCTORS ////////
	APTGGameMode();

	//////// UNREAL LIFECYCLE ////////
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	//////// METHODS ////////
	/// Init
	UFUNCTION(BlueprintCallable)
	void GenerateInitialChunks();

protected:
	//////// FIELDS ////////
	/// UI Fields
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UUserWidget> MainMenuClass;

	/// Terrain Fields
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Terrain Generation")
	FPerlinParameters TerrainParameters;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Terrain Generation")
	FPerlinParameters BiomesParameters;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Terrain Generation")
	int32 RenderDistance;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Terrain Generation")
	UMaterial* Material;
	
private:
	//////// SUBSYSTEMS ////////
	UPROPERTY()
	UChunkManagerWorldSubsystem* ChunkManager;

	//////// METHODS ////////
	/// Event Handlers
	UFUNCTION()
	void HandleGenerationProgress(int32 Current, int32 Total);

	/// Player setup
	void SpawnPlayer();
	void RepositionPlayerToGround();
	
};
