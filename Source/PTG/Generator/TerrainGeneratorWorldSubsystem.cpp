#include "TerrainGeneratorWorldSubsystem.h"
#include "ChunkData.h"
#include "ProceduralMeshGeneratorSubsystem.h"

void UTerrainGeneratorWorldSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UTerrainGeneratorWorldSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void UTerrainGeneratorWorldSubsystem::GenerateChunk(int32 X, int32 Y, int32 Size, const FPerlinParameters& Parameters)
{
	FChunk NewChunk;
	NewChunk.Size = Size;
	NewChunk.Coords = FVector(X, Y, 0);
	NewChunk.Id = ChunkData::MakeChunkId(X, Y);

	ChunkMap.Add(NewChunk.Id, NewChunk);
	OnChunkGenerationComplete.Broadcast(NewChunk.Id);
}

void UTerrainGeneratorWorldSubsystem::DisplayChunk(int64 ChunkId)
{
	if (const FChunk* ChunkToDisplay = ChunkMap.Find(ChunkId))
	{
		DisplayChunkInternal(*ChunkToDisplay);
	}
}

bool UTerrainGeneratorWorldSubsystem::DestroyChunk(int64 ChunkId)
{
	if (ProceduralMesh && ChunkMap.Find(ChunkId))
	{
		// TODO: Add logic to clean chunk procedural mesh sections
		return ChunkMap.Remove(ChunkId) > 0;
	}
	return false;
}

void UTerrainGeneratorWorldSubsystem::DisplayChunkInternal(const FChunk& Chunk)
{
	if (!ProceduralMesh)
	{
		return;
	}

	if (UProceduralMeshGeneratorSubsystem* MeshGenerator = GetWorld()->GetGameInstance()->GetSubsystem<UProceduralMeshGeneratorSubsystem>())
	{
		for (int32 x = 0; x < Chunk.Size - 1; x++)
		{
			for (int32 y = 0; y < Chunk.Size - 1; y++)
			{
				const auto Indices = MeshGenerator->GetSquareIndices(x, y, Chunk.Size);
                
				MeshGenerator->CreateSquareMesh(
					ProceduralMesh,
					Chunk.VertexArray[Indices.bottomLeft].Coords,
					Chunk.VertexArray[Indices.bottomRight].Coords,
					Chunk.VertexArray[Indices.topLeft].Coords,
					Chunk.VertexArray[Indices.topRight].Coords,
					x + y * (Chunk.Size - 1)
				);
			}
		}
	}
}
