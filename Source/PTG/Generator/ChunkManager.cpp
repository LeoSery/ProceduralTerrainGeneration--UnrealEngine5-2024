#include "ChunkManager.h"
#include "TerrainGeneratorWorldSubsystem.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"

AChunkManager::AChunkManager(): TerrainGenerator(nullptr), MeshGenerator(nullptr)
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

void AChunkManager::StressTest(int32 NumChunks)
{
	PendingChunks = NumChunks;
	bStressTestInProgress = true;
	StressTestStartTime = FPlatformTime::Seconds();
    
	UE_LOG(LogTemp, Warning, TEXT("Starting Stress Test - Generating %d chunks"), NumChunks);
    
	for(int32 i = 0; i < NumChunks; i++)
	{
		RequestChunkGeneration(i * 31, 0, 32);
	}
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

	InitialChunkGeneration();
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

	if (!PlayerPos.Equals(pos))
	{
		PlayerPos = pos;

		// Queue new chunks for generation
		for (int y = PlayerPos.Y - RenderDistance; y < PlayerPos.Y + RenderDistance; y++)
		{
			for (int x = PlayerPos.X - RenderDistance; x < PlayerPos.X + RenderDistance; x++)
			{
				if (!TerrainGenerator->HasChunk(ChunkData::MakeChunkId(x * (ChunkSize - 1), y * (ChunkSize - 1))))
				{
					ChunkGenerationQueue.Enqueue(FVector2D(x, y));
				}
			}
		}

		// Queue far chunks for destruction
		int32 playerQuadX = FMath::RoundToInt(PlayerPos.X * (ChunkSize - 1));
		int32 playerQuadY = FMath::RoundToInt(PlayerPos.Y * (ChunkSize - 1));

		for (auto [id, chunk] : TerrainGenerator->ChunkMap)
		{
			int32 chunkX = FMath::RoundToInt(chunk.Coords.X);
			int32 chunkY = FMath::RoundToInt(chunk.Coords.Y);

			if (FMath::Abs(chunkX - playerQuadX) > RenderDistance * (ChunkSize - 1) ||
				FMath::Abs(chunkY - playerQuadY) > RenderDistance * (ChunkSize - 1))
			{
				ChunkDestructionQueue.Enqueue(id);
			}
		}
	}

	// Process queued operations
	TimeSinceLastChunkOperation += DeltaTime;
	
	if (TimeSinceLastChunkOperation >= ChunkOperationInterval)
	{
		TimeSinceLastChunkOperation = 0.0f;

		// Process one chunk generation
		FVector2D ChunkPos;
		if (ChunkGenerationQueue.Dequeue(ChunkPos))
		{
			RequestChunkGeneration(
				ChunkPos.X * (ChunkSize - 1),
				ChunkPos.Y * (ChunkSize - 1),
				ChunkSize
			);
		}

		// Process one chunk destruction
		int64 ChunkId;
		if (ChunkDestructionQueue.Dequeue(ChunkId))
		{
			RequestChunkDestruction(ChunkId);
		}
	}

	Super::Tick(DeltaTime);
}

void AChunkManager::InitialChunkGeneration()
{
	// Generate center chunk first (at 0,0)
	RequestChunkGeneration(0, 0, ChunkSize);

	// Generate surrounding chunks
	for (int y = -RenderDistance; y <= RenderDistance; y++)
	{
		for (int x = -RenderDistance; x <= RenderDistance; x++)
		{
			if (x == 0 && y == 0)
			{
				continue;
			}

			RequestChunkGeneration(x * (ChunkSize-1), y * (ChunkSize-1), ChunkSize);
		}
	}

	// Spawn player in center of initial chunk
	SpawnPlayer();
}

void AChunkManager::SpawnPlayer()
{
	FVector InitialChunkCenter(
		(ChunkSize-1) * 100 / 2.0f,  // x center
		(ChunkSize-1) * 100 / 2.0f,  // y center
		200.0f                       // z height above terrain
	);
	
	if (APlayerStart* PlayerStart = Cast<APlayerStart>(UGameplayStatics::GetActorOfClass(GetWorld(), APlayerStart::StaticClass())))
	{
		PlayerStart->SetActorLocation(InitialChunkCenter);
		
		PlayerPos = FVector(0, 0, 0);
	}
}

void AChunkManager::OnChunkGenerated(int64 ChunkId)
{
	if (bStressTestInProgress)
	{
		PendingChunks--;
        
		if (PendingChunks <= 0)
		{
			double EndTime = FPlatformTime::Seconds();
			double TotalTime = (EndTime - StressTestStartTime) * 1000.0;
            
			UE_LOG(LogTemp, Warning, TEXT("Stress Test Complete:"));
			UE_LOG(LogTemp, Warning, TEXT("  Total Time: %.2f ms"), TotalTime);
			UE_LOG(LogTemp, Warning, TEXT("  Average Time per Chunk: %.2f ms"), TotalTime / PendingChunks);
            
			bStressTestInProgress = false;
		}
	}
	
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
