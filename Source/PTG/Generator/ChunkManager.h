#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "TerrainGeneratorWorldSubsystem.h"
#include "ProceduralMeshGeneratorSubsystem.h"
#include "ChunkManager.generated.h"

UCLASS()
class PTG_API AChunkManager : public AActor
{
	GENERATED_BODY()

public:
	AChunkManager();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override; // like a destructor for unsubscribing to delegates
	
private:

	UPROPERTY()
	UTerrainGeneratorWorldSubsystem* TerrainGenerator;

	UPROPERTY()
	UProceduralMeshGeneratorSubsystem* MeshGenerator;

	UFUNCTION()
	void OnChunkGenerated(int64 ChunkId);

	UFUNCTION(BlueprintCallable)
	void RequestChunkGeneration(int32 X, int32 Y, int32 Size);
	UFUNCTION(BlueprintCallable)
	void RequestChunkDestruction(int64 ChunkId);

	UPROPERTY(EditAnywhere);
	FPerlinParameters Parameters;
};
