#pragma once

#include "CoreMinimal.h"
#include "Tickable.h"
#include "Subsystems/WorldSubsystem.h"
#include "PTG/Generation/Terrain/ChunkData.h"
#include "PTG/Generation/Subsystems/ProceduralMeshGeneratorSubsystem.h"
#include "ChunkManagerWorldSubsystem.generated.h"

//////// FORWARD DECLARATION ////////
/// Class
class UTerrainGeneratorWorldSubsystem;

//////// DELEGATES ////////
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnLoadingProgressUpdate, int32, Current, int32, Total);

//////// CLASS ////////
/// Manages chunk generation and lifecycle
UCLASS()
class PTG_API UChunkManagerWorldSubsystem : public UWorldSubsystem, public FTickableGameObject
{
	GENERATED_BODY()

public:
	//////// UNREAL LIFECYCLE ////////
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	virtual void Tick(float DeltaTime) override;

	/// FTickableGameObject interface implementation
	FORCEINLINE virtual bool IsTickable() const override { return !IsTemplate(); }
	virtual TStatId GetStatId() const override { RETURN_QUICK_DECLARE_CYCLE_STAT(UChunkManagerWorldSubsystem, STATGROUP_Tickables); }

	//////// METHODS ////////
	/// Chunk generation
	UFUNCTION(BlueprintCallable)
	void InitialChunkGeneration(int32 InRenderDistance);
	UFUNCTION(BlueprintCallable)
	void StressTest(int32 NumChunks);

	/// Getters
	UFUNCTION(BlueprintCallable)
	int32 GetRenderDistance() const { return RenderDistance; }
	UFUNCTION(BlueprintCallable)
	int32 GetChunkSize() const { return ChunkSize; }

	/// Setters
	UFUNCTION(BlueprintCallable, Category = "Terrain Generation")
	void SetTerrainParameters(const FPerlinParameters& Parameters) { TerrainParameters = Parameters; }
	UFUNCTION(BlueprintCallable, Category = "Terrain Generation") 
	void SetBiomesParameters(const FPerlinParameters& Parameters) { BiomesParameters = Parameters; }
	UFUNCTION(BlueprintCallable,Category = "Terrain Generation")
	void SetRenderDistance(int32 _RenderDistance) { RenderDistance = _RenderDistance; }

	//////// DELEGATES IMPLEMENTATION ////////
	UPROPERTY(BlueprintAssignable)
	FOnLoadingProgressUpdate OnLoadingProgressUpdate;

private:
	//////// FIELDS ////////
	/// Subsystems
	UPROPERTY()
	UTerrainGeneratorWorldSubsystem* TerrainGenerator;
	UPROPERTY()
	UProceduralMeshGeneratorSubsystem* MeshGenerator;

	/// Parameters
	UPROPERTY(EditAnywhere)
	FPerlinParameters TerrainParameters;
	UPROPERTY(EditAnywhere)
	FPerlinParameters BiomesParameters;
	UPROPERTY(EditAnywhere)
	int32 ChunkSize = 64;
	UPROPERTY(EditAnywhere)
	int32 RenderDistance = 10;

	/// Runtime Data
	FVector PlayerPos;
	TQueue<FVector2D> ChunkGenerationQueue;
	TQueue<int64> ChunkDestructionQueue;
	float TimeSinceLastChunkOperation = 0.0f;
	float ChunkOperationInterval = 0.01f;
	bool bStressTestInProgress = false;
	bool bInitialChunksGenerated;
	double StressTestStartTime = 0.0;
	int32 PendingChunks = 0;
	int32 InitialChunksRemaining;

	//////// METHODS ////////
	/// Chunk management
	void RequestChunkGeneration(int32 X, int32 Y, int32 Size);
	void RequestChunkDestruction(int64 ChunkId);
	void OnChunkGenerated(int64 ChunkId);
};
