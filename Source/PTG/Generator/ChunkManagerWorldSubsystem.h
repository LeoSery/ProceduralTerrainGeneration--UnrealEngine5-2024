#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "ChunkData.h"
#include "ProceduralMeshGeneratorSubsystem.h"
#include "Tickable.h"
#include "ChunkManagerWorldSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnLoadingProgressUpdate, int32, Current, int32, Total);

class UTerrainGeneratorWorldSubsystem;

UCLASS()
class PTG_API UChunkManagerWorldSubsystem : public UWorldSubsystem, public FTickableGameObject
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	virtual void Tick(float DeltaTime) override;
	FORCEINLINE virtual bool IsTickable() const override { return !IsTemplate(); }
	virtual TStatId GetStatId() const override { RETURN_QUICK_DECLARE_CYCLE_STAT(UChunkManagerWorldSubsystem, STATGROUP_Tickables); }

	UPROPERTY(BlueprintAssignable)
	FOnLoadingProgressUpdate OnLoadingProgressUpdate;

	UFUNCTION(BlueprintCallable)
	void StressTest(int32 NumChunks);

	UFUNCTION(BlueprintCallable)
	void InitialChunkGeneration(int32 InRenderDistance);

	UFUNCTION(BlueprintCallable)
	int32 GetRenderDistance() const { return RenderDistance; }

	UFUNCTION(BlueprintCallable)
	int32 GetChunkSize() const { return ChunkSize; }

	UFUNCTION(BlueprintCallable, Category = "Terrain Generation")
	void SetTerrainParameters(const FPerlinParameters& Parameters) { TerrainParameters = Parameters; }

	UFUNCTION(BlueprintCallable, Category = "Terrain Generation") 
	void SetBiomesParameters(const FPerlinParameters& Parameters) { BiomesParameters = Parameters; }

	UFUNCTION(BlueprintCallable,Category = "Terrain Generation")
	void SetRenderDistance(int32 _RenderDistance) { RenderDistance = _RenderDistance; }
	
private:
	void RequestChunkGeneration(int32 X, int32 Y, int32 Size);
	void RequestChunkDestruction(int64 ChunkId);
	void OnChunkGenerated(int64 ChunkId);

	UPROPERTY()
	UTerrainGeneratorWorldSubsystem* TerrainGenerator;

	UPROPERTY()
	UProceduralMeshGeneratorSubsystem* MeshGenerator;

	UPROPERTY(EditAnywhere)
	FPerlinParameters TerrainParameters;

	UPROPERTY(EditAnywhere)
	FPerlinParameters BiomesParameters;

	UPROPERTY(EditAnywhere)
	int32 ChunkSize = 64;

	UPROPERTY(EditAnywhere)
	int32 RenderDistance = 10;

	FVector PlayerPos;
	TQueue<FVector2D> ChunkGenerationQueue;
	TQueue<int64> ChunkDestructionQueue;
	float TimeSinceLastChunkOperation = 0.0f;
	float ChunkOperationInterval = 0.05f;
	bool bStressTestInProgress = false;
	double StressTestStartTime = 0.0;
	int32 PendingChunks = 0;
	int32 InitialChunksRemaining;
	bool bInitialChunksGenerated;
};
