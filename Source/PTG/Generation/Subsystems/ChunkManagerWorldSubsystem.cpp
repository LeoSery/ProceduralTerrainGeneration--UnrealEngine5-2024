#include "PTG/Generation/Subsystems/ChunkManagerWorldSubsystem.h"
#include "PTG/Generation/Subsystems/TerrainGeneratorWorldSubsystem.h"

/**
 * @file ChunkManagerWorldSubsystem.cpp
 * @brief Implementation of the chunk management system handling terrain chunk lifecycle
 * @details Manages chunk loading and unloading based on player position
 */

void UChunkManagerWorldSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
    
	if (UWorld* World = GetWorld())
	{
		UTerrainGeneratorWorldSubsystem* Generator = World->GetSubsystem<UTerrainGeneratorWorldSubsystem>();
		if (!Generator)
		{
			Collection.InitializeDependency<UTerrainGeneratorWorldSubsystem>();
			Generator = World->GetSubsystem<UTerrainGeneratorWorldSubsystem>();
		}
		TerrainGenerator = Generator;
		
		if (TerrainGenerator)
		{
			TerrainGenerator->OnChunkGenerationComplete.AddUObject(this, &UChunkManagerWorldSubsystem::OnChunkGenerated);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to initialize TerrainGenerator"));
			return;
		}

		if (UGameInstance* GameInstance = World->GetGameInstance())
		{
			MeshGenerator = GameInstance->GetSubsystem<UProceduralMeshGeneratorSubsystem>();
		}
	}
}

/**
 * @brief Cleans up subsystem resources
 */
void UChunkManagerWorldSubsystem::Deinitialize()
{
	if (TerrainGenerator)
	{
		TerrainGenerator->OnChunkGenerationComplete.RemoveAll(this);
	}
    
	Super::Deinitialize();
}

/**
 * @brief Updates chunk loading state based on player position
 * @param DeltaTime Time elapsed since last tick
 * @details Manages chunk generation and destruction queues based on render distance
 */
void UChunkManagerWorldSubsystem::Tick(float DeltaTime)
{
	if (!bInitialChunksGenerated)
    {
        return;
    }
    
    if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
    {
        if (APawn* Pawn = PC->GetPawn())
        {
            FVector pos = FVector(
                FMath::Floor(Pawn->GetActorLocation().X / ((ChunkSize - 1.0f) * 100)),
                FMath::Floor(Pawn->GetActorLocation().Y / ((ChunkSize - 1.0f) * 100)),
                0.0f);

            if (!PlayerPos.Equals(pos))
            {
                PlayerPos = pos;
            	
                // Queue new chunks generation
                for (int y = PlayerPos.Y - RenderDistance; y <= PlayerPos.Y + RenderDistance; y++)
                {
                    for (int x = PlayerPos.X - RenderDistance; x <= PlayerPos.X + RenderDistance; x++)
                    {
                        if (!TerrainGenerator->HasChunk(ChunkData::GetChunkIdFromCoordinates(x * (ChunkSize - 1), y * (ChunkSize - 1))))
                        {
            				FScopeLock Lock(&ChunkQueueLock);
                            ChunkGenerationQueue.AddUnique(FVector2D(x, y));
                        }
                    }
                }

                // Queue far chunks for destruction
                int32 playerQuadX = FMath::RoundToInt(PlayerPos.X * (ChunkSize - 1));
                int32 playerQuadY = FMath::RoundToInt(PlayerPos.Y * (ChunkSize - 1));

                if (TerrainGenerator)
                {
                    for (auto& [id, chunk] : TerrainGenerator->ChunkMap)
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
            }
        }
    }

    // Process queued operations
    TimeSinceLastChunkOperation += DeltaTime;
    
    if (TimeSinceLastChunkOperation >= ChunkOperationInterval)
    {
    	TimeSinceLastChunkOperation = 0.0f;

    	int32 QueueSize;
	    {
    		FScopeLock Lock(&ChunkQueueLock);
    		QueueSize = ChunkGenerationQueue.Num();
	    }

    	int32 ChunksToProcess = FMath::Clamp(QueueSize / 5 + 1, 1, 4);
    	
    	ChunksToProcess = FMath::Min(ChunksToProcess, QueueSize);

    	FVector PlayerForward = FVector::ZeroVector;
    	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
    	{
    		PlayerForward = PC->GetControlRotation().Vector();
    		PlayerForward.Z = 0;
    		PlayerForward.Normalize();
    	}

    	for (int32 i = 0; i < ChunksToProcess; ++i)
    	{
    		FVector2D ChunkPos;
    		bool bFoundChunk = false;
	        
		    {
    			FScopeLock Lock(&ChunkQueueLock);
    			
    			if (ChunkGenerationQueue.Num() > 0)
    			{
					if (i == 0)
					{
    					ChunkGenerationQueue.Sort([this, PlayerForward](const FVector2D& A, const FVector2D& B)
    					{
							FVector2D PlayerPos2D(PlayerPos.X, PlayerPos.Y);
    						
							FVector DirToA(A.X - PlayerPos.X, A.Y - PlayerPos.Y, 0);
							FVector DirToB(B.X - PlayerPos.X, B.Y - PlayerPos.Y, 0);
    						
							if (!DirToA.IsNearlyZero()) DirToA.Normalize();
							if (!DirToB.IsNearlyZero()) DirToB.Normalize();
    						
							float DotA = FVector::DotProduct(PlayerForward, DirToA);
							float DotB = FVector::DotProduct(PlayerForward, DirToB);
    						
							float ViewScoreA = (DotA > 0.0f) ? DotA : -0.5f;
							float ViewScoreB = (DotB > 0.0f) ? DotB : -0.5f;
    						
							float DistA = FVector2D::DistSquared(A, PlayerPos2D);
							float DistB = FVector2D::DistSquared(B, PlayerPos2D);
    						
							const float ViewWeightFactor = 0.9f;
    						
							float ScoreA = DistA * (1.0f - ViewScoreA * ViewWeightFactor);
							float ScoreB = DistB * (1.0f - ViewScoreB * ViewWeightFactor);
						    
							return ScoreA < ScoreB;
						});
					}
    				
	                ChunkPos = ChunkGenerationQueue[0];
	                ChunkGenerationQueue.RemoveAt(0);
	                bFoundChunk = true;
    			}
		    }
    		
    		if (bFoundChunk)
    		{
    			RequestChunkGeneration(
					ChunkPos.X * (ChunkSize - 1),
					ChunkPos.Y * (ChunkSize - 1),
					ChunkSize
				);
    		}
    		else
    		{
    			break;
    		}
    	}

    	int32 DestructionsToProcess = FMath::Min(2, ChunksToProcess);

    	for (int32 i = 0; i < DestructionsToProcess; ++i)
    	{
    		int64 ChunkId;
    		if (ChunkDestructionQueue.Dequeue(ChunkId))
    		{
    			RequestChunkDestruction(ChunkId);
    		}
    		else
    		{
    			break;
    		}
    	}
    }
}

/**
 * @brief Performs stress test of chunk generation
 * @param NumChunks Number of chunks to generate for testing
 * @details Measures performance metrics during bulk chunk generation
 */
void UChunkManagerWorldSubsystem::StressTest(int32 NumChunks)
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

/**
 * @brief Initiates generation of initial chunk grid
 * @param InRenderDistance Radius of chunks to generate around player
 * @details Creates initial terrain grid centered on player position
 */
void UChunkManagerWorldSubsystem::InitialChunkGeneration(int32 InRenderDistance)
{
	UE_LOG(LogTemp, Warning, TEXT("Starting InitialChunkGeneration with RenderDistance: %d"), InRenderDistance);
	InitialChunksRemaining = ChunkData::GetInitialChunkCount(InRenderDistance);
    
	RequestChunkGeneration(0, 0, ChunkSize);

	for (int y = -RenderDistance; y <= RenderDistance; y++)
	{
		for (int x = -RenderDistance; x <= RenderDistance; x++)
		{
			if (x == 0 && y == 0)
			{
				continue;
			}
			
			RequestChunkGeneration(x * (ChunkSize - 1), y * (ChunkSize - 1), ChunkSize);
		}
	}
}

/**
 * @brief Requests generation of a new chunk
 * @param X X-coordinate of chunk origin
 * @param Y Y-coordinate of chunk origin
 * @param Size Size of chunk in vertices
 */
void UChunkManagerWorldSubsystem::RequestChunkGeneration(int32 X, int32 Y, int32 Size)
{
	UE_LOG(LogTemp, Warning, TEXT("Requested Chunk Generation"));

	if (TerrainGenerator)
	{
		TerrainGenerator->GenerateChunk(X, Y, Size, TerrainParameters, BiomesParameters);
	}
}

/**
 * @brief Requests destruction of an existing chunk
 * @param ChunkId Unique identifier of chunk to destroy
 */
void UChunkManagerWorldSubsystem::RequestChunkDestruction(int64 ChunkId)
{
	if (TerrainGenerator)
	{
		TerrainGenerator->DestroyChunk(ChunkId);
	}
}

/**
 * @brief Callback handler for chunk generation completion
 * @param ChunkId Identifier of completed chunk
 * @details Updates generation progress and manages chunk display and stress test completion
 */
void UChunkManagerWorldSubsystem::OnChunkGenerated(int64 ChunkId)
{
	if (!bInitialChunksGenerated)
	{
		InitialChunksRemaining--;

		OnLoadingProgressUpdate.Broadcast(
			ChunkData::GetInitialChunkCount(RenderDistance) - InitialChunksRemaining,
			ChunkData::GetInitialChunkCount(RenderDistance)
		);
        
		if (InitialChunksRemaining <= 0)
		{
			bInitialChunksGenerated = true;
			UE_LOG(LogTemp, Warning, TEXT("Initial chunks generation complete!"));
		}
	}
    
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
