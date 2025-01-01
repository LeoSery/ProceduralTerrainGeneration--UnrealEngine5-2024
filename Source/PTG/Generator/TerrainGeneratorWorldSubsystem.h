#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Chunk_Type.h"
#include "TerrainGeneratorWorldSubsystem.generated.h"

class UProceduralMeshComponent;
struct FPerlinParameters;

UCLASS()
class PTG_API UTerrainGeneratorWorldSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	void DisplayChunk(const FChunk& Chunk, UProceduralMeshComponent* ProceduralMesh) const;
	void GenerateChunk(int _x, int _y, int _size, FPerlinParameters _parameters);

	void OnChunkCalcOver(FString _id, FChunk _chunk);

	void SetProceduralMesh(UProceduralMeshComponent* _proceduralMesh) { ProceduralMesh = _proceduralMesh; };

	TMap<FString, FChunk> ChunkMap;
	UProceduralMeshComponent* ProceduralMesh;
};
