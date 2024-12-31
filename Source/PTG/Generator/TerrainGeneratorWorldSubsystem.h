#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "TerrainGeneratorWorldSubsystem.generated.h"

class UProceduralMeshComponent;
struct FChunk;

UCLASS()
class PTG_API UTerrainGeneratorWorldSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	void DisplayChunk(const FChunk& Chunk, UProceduralMeshComponent* ProceduralMesh) const;
	FChunk GenerateChunk(int _x, int _y, int _size, int _octaves, float _persistence, float _frequency, int _seed);
};
