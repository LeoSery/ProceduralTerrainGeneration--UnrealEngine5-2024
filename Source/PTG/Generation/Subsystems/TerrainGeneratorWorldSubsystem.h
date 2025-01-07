#pragma once

#include "CoreMinimal.h"
#include "Materials/Material.h"
#include "Subsystems/WorldSubsystem.h"
#include "PTG/Generation/Terrain/ChunkData.h"
#include "TerrainGeneratorWorldSubsystem.generated.h"

//////// FORWARD DECLARATION ////////
/// Struct
struct FPerlinParameters;

//////// CLASS ////////
/// Core system handling terrain generation and chunk management
UCLASS()
class PTG_API UTerrainGeneratorWorldSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	//////// UNREAL LIFECYCLE ////////
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	//////// METHODS ////////
	/// Chunk management
	void GenerateChunk(int32 X, int32 Y, int32 Size, const FPerlinParameters& Parameters, const FPerlinParameters& BiomesParameters);
	void DisplayChunk(int64 ChunkId);
	bool DestroyChunk(int64 ChunkId);
	void OnChunkCalcOver(int64 _id, FChunk _chunk);

	/// Getters
	bool HasChunk(int64 ChunkId) const { return ChunkMap.Contains(ChunkId); }
	const FChunk* GetChunk(int64 ChunkId) const { return ChunkMap.Find(ChunkId); }

	/// Setters
	void SetMaterial(UMaterial* _material) { Material = _material; }
	
	//////// DELEGATES IMPLEMENTATION ////////
	FOnChunkGenerationComplete OnChunkGenerationComplete;

	//////// FIELDS ////////
	/// Core data
	UPROPERTY()
	TMap<int64, FChunk> ChunkMap;
	UPROPERTY()
	TMap<int64, AActor*> MeshMap;

	/// Material
	UMaterial* Material;

private:
	//////// FIELDS ////////
	/// Stats
	struct FGenerationStats 
	{
		double StartTime;
		double EndTime;
		int32 ChunkSize;
		int32 VertexCount;
		int32 TriangleCount;
	};
	TMap<int64, FGenerationStats> GenerationStats;

	//////// METHODS ////////
	void DisplayChunkInternal(const FChunk& Chunk);
};
