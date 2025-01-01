#include "ChunkManager.h"
#include "ProceduralMeshComponent.h"
#include "TerrainGeneratorWorldSubsystem.h"

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

	if (TerrainGenerator)
	{
		TerrainGenerator->SetProceduralMesh(ProceduralMesh);
		TerrainGenerator->OnChunkGenerationComplete.AddUObject(this, &AChunkManager::OnChunkGenerated);
	}

	ProceduralMesh->ClearAllMeshSections();
	RequestChunkGeneration(0, 0, 32);
}

void AChunkManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (TerrainGenerator)
	{
		TerrainGenerator->OnChunkGenerationComplete.RemoveAll(this);
	}
	
	Super::EndPlay(EndPlayReason);
}

void AChunkManager::OnChunkGenerated(int64 ChunkId)
{
	/* At present, the chunk is displayed as soon as it is generated, but later
	 * it may be subject to a more advanced logic based on distance from the player
	 * (a distance from which you decide to generate/destroy the chunk, and another
	 * distance from which you decide to display/hide the chunk).
	 */
	if (TerrainGenerator)
	{
		TerrainGenerator->DisplayChunk(ChunkId);
	}
}

void AChunkManager::RequestChunkGeneration(int32 X, int32 Y, int32 Size)
{
	if (TerrainGenerator)
	{
		// Hard-coded parameters for system testing
		FPerlinParameters Parameters;
		Parameters.Frequency = 0.1f;
		Parameters.Octaves = 4;
		Parameters.Persistence = 0.5f;
		Parameters.Seed = 4;

		TerrainGenerator->GenerateChunk(X, Y, Size, Parameters);
	}
}

void AChunkManager::RequestChunkDestruction(int64 ChunkId)
{
	if (TerrainGenerator)
	{
		TerrainGenerator->DestroyChunk(ChunkId);
	}
}
