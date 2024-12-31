#include "ChunkManager.h"
#include "ProceduralMeshComponent.h"
#include "TerrainGeneratorWorldSubsystem.h"
#include "Chunk_Type.h" 

AChunkManager::AChunkManager()
{
	PrimaryActorTick.bCanEverTick = false;
	
	ProceduralMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("ProceduralMesh"));
	RootComponent = ProceduralMesh;
}

void AChunkManager::BeginPlay()
{
	Super::BeginPlay();

	TerrainGenerator = GetWorld()->GetSubsystem<UTerrainGeneratorWorldSubsystem>();
	MeshGenerator = GetGameInstance()->GetSubsystem<UProceduralMeshGeneratorSubsystem>();

	// Debug test, generates a single chunk on startup
	ProceduralMesh->ClearAllMeshSections();
	GenerateChunk();
}

void AChunkManager::GenerateChunk()
{
	if (TerrainGenerator && ProceduralMesh)
	{
		FPerlinParameters parameters;
		parameters.frequency = 0.1;
		parameters.octaves = 4;
		parameters.persistence = 0.5;
		parameters.seed = 4;
		TerrainGenerator->SetProceduralMesh(ProceduralMesh);
		TerrainGenerator->GenerateChunk(0, 0, 32,parameters);
	}
}
