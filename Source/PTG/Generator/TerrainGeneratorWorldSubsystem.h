#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "ChunkData.h"
#include "ProceduralMeshComponent.h"
#include "Materials/Material.h"
#include "TerrainGeneratorWorldSubsystem.generated.h"

struct FPerlinParameters;

UCLASS()
class PTG_API UTerrainGeneratorWorldSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	FOnChunkGenerationComplete OnChunkGenerationComplete;
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	void GenerateChunk(int32 X, int32 Y, int32 Size, const FPerlinParameters& Parameters, const FPerlinParameters& BiomesParameters);
	void DisplayChunk(int64 ChunkId);
	bool DestroyChunk(int64 ChunkId);
	void OnChunkCalcOver(int64 _id, FChunk _chunk);

	bool HasChunk(int64 ChunkId) const { return ChunkMap.Contains(ChunkId); }
	const FChunk* GetChunk(int64 ChunkId) const { return ChunkMap.Find(ChunkId); }

	void SetMaterial(UMaterial* _material) { Material = _material; }

	UPROPERTY()
	TMap<int64, FChunk> ChunkMap;
	UPROPERTY()
	TMap<int64, AActor*> MeshMap;

	UMaterial* Material;

private:
	void DisplayChunkInternal(const FChunk& Chunk);

	

	struct FGenerationStats 
	{
		double StartTime;
		double EndTime;
		int32 ChunkSize;
		int32 VertexCount;
		int32 TriangleCount;
	};
    
	TMap<int64, FGenerationStats> GenerationStats;
};
