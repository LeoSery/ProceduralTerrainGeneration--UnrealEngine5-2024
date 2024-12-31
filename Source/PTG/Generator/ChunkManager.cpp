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
		FChunk NewChunk = TerrainGenerator->GenerateChunk(0, 0, 32, 4, 0.5, 0.1, 4);
		TerrainGenerator->DisplayChunk(NewChunk, ProceduralMesh);
	}
}
