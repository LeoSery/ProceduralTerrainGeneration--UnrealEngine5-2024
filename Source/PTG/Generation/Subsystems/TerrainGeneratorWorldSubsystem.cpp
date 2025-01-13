#include "PTG/Generation/Subsystems/TerrainGeneratorWorldSubsystem.h"
#include "ProceduralMeshComponent.h"
#include "ProceduralMeshGeneratorSubsystem.h"
#include "PTG/Generation/Terrain/ChunkData.h"
#include "PTG/Generation/Terrain/ChunkThread.h"

/**
 * @file TerrainGeneratorWorldSubsystem.cpp
 * @brief Implementation of the core terrain generation and management system
 * @details Handles terrain chunk generation, mesh creation, and material application
 */

void UTerrainGeneratorWorldSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

/**
 * @brief Cleans up terrain resources and meshes
 * @details Removes all generated meshes and clears chunk data
 */
void UTerrainGeneratorWorldSubsystem::Deinitialize()
{
	for (auto& MeshPair : MeshMap)
	{
		if (AActor* Mesh = MeshPair.Value)
		{
			if (UProceduralMeshComponent* ProceduralMesh = Mesh->FindComponentByClass<UProceduralMeshComponent>())
			{
				ProceduralMesh->ClearMeshSection(0);
			}
			Mesh->Destroy();
		}
	}
	MeshMap.Empty();
	ChunkMap.Empty();
	
	Super::Deinitialize();
}

/**
 * @brief Initiates generation of a new terrain chunk
 * @param X X-coordinate of chunk origin
 * @param Y Y-coordinate of chunk origin
 * @param Size Size of chunk in vertices
 * @param TerrainParameters Perlin noise parameters for height generation
 * @param BiomesParameters Perlin noise parameters for biome variation
 */
void UTerrainGeneratorWorldSubsystem::GenerateChunk(int32 X, int32 Y, int32 Size, const FPerlinParameters& TerrainParameters, const FPerlinParameters& BiomesParameters)
{
	UE_LOG(LogTemp, Warning, TEXT("Starting chunk generation at X: %d, Y: %d"), X, Y);
	
	FChunk NewChunk;
	NewChunk.Size = Size;
	NewChunk.Coords = FVector(X, Y, 0);
	NewChunk.Id = ChunkData::GetChunkIdFromCoordinates(X, Y);

	ChunkMap.Add(NewChunk.Id, NewChunk);
	FChunkThread* Thread = new FChunkThread(NewChunk, TerrainParameters,BiomesParameters);
	Thread->OnCalcOver.AddUObject(this, &UTerrainGeneratorWorldSubsystem::OnChunkCalcOver);
}

/**
 * @brief Creates visual mesh for a generated chunk
 * @param ChunkId Unique identifier of chunk to display
 */
void UTerrainGeneratorWorldSubsystem::DisplayChunk(int64 ChunkId)
{	
	if (const FChunk* ChunkToDisplay = ChunkMap.Find(ChunkId))
	{
		DisplayChunkInternal(*ChunkToDisplay);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Chunk %lld not found in map"), ChunkId);
	}
}

/**
 * @brief Removes a chunk from the world
 * @param ChunkId Unique identifier of chunk to destroy
 * @return True if chunk was successfully destroyed
 */
bool UTerrainGeneratorWorldSubsystem::DestroyChunk(int64 ChunkId)
{
	if (MeshMap.Find(ChunkId) && ChunkMap.Find(ChunkId))
	{
		AActor* Mesh = MeshMap.FindAndRemoveChecked(ChunkId);
		UProceduralMeshComponent* ProceduralMesh = Mesh->FindComponentByClass<UProceduralMeshComponent>();
		ProceduralMesh->ClearMeshSection(0);
		Mesh->Destroy();
		return ChunkMap.Remove(ChunkId) > 0;
	}
	return false;
}

/**
 * @brief Callback handler for chunk calculation completion
 * @param _id Identifier of completed chunk
 * @param _chunk Data of completed chunk
 */
void UTerrainGeneratorWorldSubsystem::OnChunkCalcOver(int64 _id, FChunk _chunk)
{
	FChunk* chunk = ChunkMap.Find(_id);
	
	if (chunk)
	{
		ChunkMap.Add(_id, _chunk);
		UE_LOG(LogTemp, Warning, TEXT("Generating Chunk Soone to Display"));
		OnChunkGenerationComplete.Broadcast(_id);
	}
}

/**
 * @brief Internal method to handle chunk mesh creation and display
 * @param Chunk Data of chunk to display
 * @details Creates procedural mesh component and applies materials
 */
void UTerrainGeneratorWorldSubsystem::DisplayChunkInternal(const FChunk& Chunk)
{
	FGenerationStats& Stats = GenerationStats.Add(Chunk.Id);
	Stats.StartTime = FPlatformTime::Seconds();
	Stats.ChunkSize = Chunk.Size;
	Stats.VertexCount = Chunk.Size * Chunk.Size;
	Stats.TriangleCount = (Chunk.Size - 1) * (Chunk.Size - 1) * 2;
	
	AActor* MeshOwner = nullptr;
	if (AActor* const* ExistingMeshOwner = MeshMap.Find(Chunk.Id))
	{
		MeshOwner = *ExistingMeshOwner;
	}
	else
	{
		MeshOwner = GetWorld()->SpawnActor<AActor>();

		// Create the procedural mesh component with the actor as its owner
		UProceduralMeshComponent* ProceduralMesh = NewObject<UProceduralMeshComponent>(
			MeshOwner,  // Set the owner to our newly created actor
			UProceduralMeshComponent::StaticClass(),
			NAME_None,
			RF_Transient
		);

		// Attach the component to the actor's root
		ProceduralMesh->SetupAttachment(MeshOwner->GetRootComponent());
		ProceduralMesh->RegisterComponent();
		
		if (Material) 
		{
			ProceduralMesh->SetMaterial(0, Material);
		}
		
		MeshMap.Emplace(Chunk.Id, MeshOwner);
	}
	
	if (UProceduralMeshGeneratorSubsystem* MeshGenerator = GetWorld()->GetGameInstance()->GetSubsystem<UProceduralMeshGeneratorSubsystem>())
	{
		MeshGenerator->CreateChunkMesh(
			MeshOwner->FindComponentByClass<UProceduralMeshComponent>(),
			Chunk,
			0
		);
	}

	Stats.VertexCount = Chunk.Size * Chunk.Size;
	Stats.TriangleCount = (Chunk.Size - 1) * (Chunk.Size - 1) * 2;
	Stats.EndTime = FPlatformTime::Seconds();
	double GenerationTime = (Stats.EndTime - Stats.StartTime) * 1000;
	
	UE_LOG(LogTemp, Warning, TEXT("Chunk %lld Generation Stats:"), Chunk.Id);
	UE_LOG(LogTemp, Warning, TEXT("  Generation Time: %.2f ms"), GenerationTime);
	UE_LOG(LogTemp, Warning, TEXT("  Size: %d x %d"), Stats.ChunkSize, Stats.ChunkSize);
	UE_LOG(LogTemp, Warning, TEXT("  Vertices: %d"), Stats.VertexCount);
	UE_LOG(LogTemp, Warning, TEXT("  Triangles: %d"), Stats.TriangleCount);
}
