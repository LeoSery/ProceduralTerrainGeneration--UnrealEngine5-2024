#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TerrainGeneratorWorldSubsystem.h"
#include "ProceduralMeshGeneratorSubsystem.h"
#include "ChunkManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnLoadingProgressUpdate, int32, Current, int32, Total);

UCLASS()
class PTG_API AChunkManager : public AActor
{
	GENERATED_BODY()

public:
	AChunkManager();

	UPROPERTY(BlueprintAssignable)
	FOnLoadingProgressUpdate OnLoadingProgressUpdate;
	
	UFUNCTION(BlueprintCallable)
	void StressTest(int32 NumChunks);

	UPROPERTY(BlueprintReadOnly, Category = "Startup")
	bool bInitialChunksGenerated;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> MainMenuClass;

	UFUNCTION(BlueprintCallable)
	void InitialChunkGeneration();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override; // like a destructor for unsubscribing to delegates
	virtual void Tick(float DeltaTime) override;
	
private:

	UPROPERTY()
	UTerrainGeneratorWorldSubsystem* TerrainGenerator;

	UPROPERTY()
	UProceduralMeshGeneratorSubsystem* MeshGenerator;
	
	UFUNCTION()
	void SpawnPlayer();
	
	UFUNCTION()
	void OnChunkGenerated(int64 ChunkId);

	UFUNCTION(BlueprintCallable)
	void RequestChunkGeneration(int32 X, int32 Y, int32 Size);
	UFUNCTION(BlueprintCallable)
	void RequestChunkDestruction(int64 ChunkId);

	UPROPERTY(EditAnywhere)
	FPerlinParameters TerrainParameters;

	UPROPERTY(EditAnywhere)
	FPerlinParameters BiomesParameters;

	UPROPERTY(EditAnywhere)
	int32 ChunkSize;

	UPROPERTY(EditAnywhere)
	int32 RenderDistance;

	UPROPERTY(EditAnywhere)
	UMaterial* Material;

	FVector PlayerPos;

	TQueue<FVector2D> ChunkGenerationQueue;
	TQueue<int64> ChunkDestructionQueue;
	float TimeSinceLastChunkOperation = 0.0f;

	UPROPERTY(EditAnywhere)
	float ChunkOperationInterval = 0.05f;

	// stress test
	bool bStressTestInProgress = false;
	double StressTestStartTime = 0.0;
	int32 PendingChunks = 0;
	
	int32 InitialChunksRemaining;

	UFUNCTION()
	void RepositionPlayerToGround();
};
