#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "ChunkData.h"
#include "TerrainGeneratorWorldSubsystem.generated.h"

class UProceduralMeshComponent;
struct FPerlinParameters;

UCLASS()
class PTG_API UTerrainGeneratorWorldSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	FOnChunkGenerationComplete OnChunkGenerationComplete;
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	void GenerateChunk(int32 X, int32 Y, int32 Size, const FPerlinParameters& Parameters);
	void DisplayChunk(int64 ChunkId);
	bool DestroyChunk(int64 ChunkId);
	
	FORCEINLINE void SetProceduralMesh(UProceduralMeshComponent* _proceduralMesh) { ProceduralMesh = _proceduralMesh; }

	bool HasChunk(int64 ChunkId) const { return ChunkMap.Contains(ChunkId); }
	const FChunk* GetChunk(int64 ChunkId) const { return ChunkMap.Find(ChunkId); }

private:
	void DisplayChunkInternal(const FChunk& Chunk);

	UPROPERTY()
	TMap<int64, FChunk> ChunkMap;

	UPROPERTY()
	UProceduralMeshComponent* ProceduralMesh;
};
