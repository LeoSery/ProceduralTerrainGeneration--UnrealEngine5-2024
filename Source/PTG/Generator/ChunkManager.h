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

	UPROPERTY(VisibleAnywhere)
	UProceduralMeshComponent* ProceduralMesh;

protected:
	virtual void BeginPlay() override;
	
private:
	UPROPERTY()
	UTerrainGeneratorWorldSubsystem* TerrainGenerator;

	UPROPERTY()
	UProceduralMeshGeneratorSubsystem* MeshGenerator;

	void GenerateChunk();
};
