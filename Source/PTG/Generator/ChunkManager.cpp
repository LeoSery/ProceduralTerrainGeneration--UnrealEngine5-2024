#include "ChunkManager.h"
#include "ProceduralMeshComponent.h"
#include "TerrainGeneratorWorldSubsystem.h"

AChunkManager::AChunkManager()
{
	PrimaryActorTick.bCanEverTick = false;
	
	Parameters.Frequency = 0.1f;
	Parameters.Octaves = 4;
	Parameters.Persistence = 0.5f;
	Parameters.Seed = 4;
	Parameters.HeightFactor = 100;

	ChunkSize = 32;
	RenderDistance = 2;
	PrimaryActorTick.bCanEverTick = true;

}

void AChunkManager::BeginPlay()
{
	Super::BeginPlay();

	TerrainGenerator = GetWorld()->GetSubsystem<UTerrainGeneratorWorldSubsystem>();
	MeshGenerator = GetGameInstance()->GetSubsystem<UProceduralMeshGeneratorSubsystem>();

	if (TerrainGenerator)
	{
		TerrainGenerator->OnChunkGenerationComplete.AddUObject(this, &AChunkManager::OnChunkGenerated);
	}

	AActor* Player = GetWorld()->GetFirstPlayerController()->GetPawn();
	PlayerPos = FVector(
		FMath::Floor(Player->GetActorLocation().X / ((ChunkSize-1.0f )* 100)),
		FMath::Floor(Player->GetActorLocation().Y / ((ChunkSize-1.0f )* 100)),
		0.0f);


	for (int y = PlayerPos.Y- RenderDistance; y < PlayerPos.Y+ RenderDistance; y++) {
		for (int x = PlayerPos.X- RenderDistance; x < PlayerPos.X+ RenderDistance; x++) {

			RequestChunkGeneration(x * (ChunkSize-1), y * (ChunkSize-1), ChunkSize);
		}
	}

}

void AChunkManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (TerrainGenerator)
	{
		TerrainGenerator->OnChunkGenerationComplete.RemoveAll(this);
	}
	
	Super::EndPlay(EndPlayReason);
}

void AChunkManager::Tick(float DeltaTime)
{
	AActor* Player = GetWorld()->GetFirstPlayerController()->GetPawn();

	FVector pos = FVector(
		FMath::Floor(Player->GetActorLocation().X / ((ChunkSize - 1.0f) * 100)),
		FMath::Floor(Player->GetActorLocation().Y / ((ChunkSize - 1.0f) * 100)),
		0.0f);

	if (!PlayerPos.Equals(pos)) {
		PlayerPos = pos;

		// Queue new chunks for generation
		for (int y = PlayerPos.Y - RenderDistance; y < PlayerPos.Y + RenderDistance; y++) {
			for (int x = PlayerPos.X - RenderDistance; x < PlayerPos.X + RenderDistance; x++) {
				if (!TerrainGenerator->HasChunk(ChunkData::MakeChunkId(x * (ChunkSize - 1), y * (ChunkSize - 1)))) {
					ChunkGenerationQueue.Enqueue(FVector2D(x, y));
				}
			}
		}

		// Queue far chunks for destruction
		int32 playerQuadX = FMath::RoundToInt(PlayerPos.X * (ChunkSize - 1));
		int32 playerQuadY = FMath::RoundToInt(PlayerPos.Y * (ChunkSize - 1));

		for (auto [id, chunk] : TerrainGenerator->ChunkMap) {
			int32 chunkX = FMath::RoundToInt(chunk.Coords.X);
			int32 chunkY = FMath::RoundToInt(chunk.Coords.Y);

			if (FMath::Abs(chunkX - playerQuadX) > RenderDistance * (ChunkSize - 1) ||
				FMath::Abs(chunkY - playerQuadY) > RenderDistance * (ChunkSize - 1)) {
				ChunkDestructionQueue.Enqueue(id);
			}
		}
	}

	// Process queued operations
	TimeSinceLastChunkOperation += DeltaTime;
	if (TimeSinceLastChunkOperation >= ChunkOperationInterval) {
		TimeSinceLastChunkOperation = 0.0f;

		// Process one chunk generation
		FVector2D ChunkPos;
		if (ChunkGenerationQueue.Dequeue(ChunkPos)) {
			RequestChunkGeneration(
				ChunkPos.X * (ChunkSize - 1),
				ChunkPos.Y * (ChunkSize - 1),
				ChunkSize
			);
		}

		// Process one chunk destruction
		int64 ChunkId;
		if (ChunkDestructionQueue.Dequeue(ChunkId)) {
			RequestChunkDestruction(ChunkId);
		}
	}

	Super::Tick(DeltaTime);
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
	UE_LOG(LogTemp, Warning, TEXT("Requested Chunk Generation"));
	if (TerrainGenerator)
	{
		// Hard-coded parameters for system testing
		

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
