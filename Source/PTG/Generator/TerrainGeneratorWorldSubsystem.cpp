#include "TerrainGeneratorWorldSubsystem.h"
#include "Chunk_Type.h"
#include "ProceduralMeshGeneratorSubsystem.h"
#include "PerlinNoise.h"
#include <PTG/Generator/ChunkThread.h>

void UTerrainGeneratorWorldSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UTerrainGeneratorWorldSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void UTerrainGeneratorWorldSubsystem::DisplayChunk(const FChunk& Chunk, UProceduralMeshComponent* _ProceduralMesh) const
{
	if (UProceduralMeshGeneratorSubsystem* MeshGenerator = GetWorld()->GetGameInstance()->GetSubsystem<UProceduralMeshGeneratorSubsystem>())
	{
		for (int32 x = 0; x < Chunk.size - 1; x++)
		{
			for (int32 y = 0; y < Chunk.size - 1; y++)
			{
				const auto Indices = MeshGenerator->GetSquareIndices(x, y, Chunk.size);
                
				MeshGenerator->CreateSquareMesh(
					ProceduralMesh,
					Chunk.vertexArray[Indices.bottomLeft].Coords,
					Chunk.vertexArray[Indices.bottomRight].Coords,
					Chunk.vertexArray[Indices.topLeft].Coords,
					Chunk.vertexArray[Indices.topRight].Coords,
					x + y * (Chunk.size - 1)
				);
			}
		}
	}
}

void UTerrainGeneratorWorldSubsystem::GenerateChunk(int _x, int _y, int _size, FPerlinParameters _parameters)
{
	FChunk Chunk;
	Chunk.size = _size;
	Chunk.Coords = FVector(_x, _y, 0);
	Chunk.Id = FString::FromInt(_x) + FString::FromInt(_y);
	ChunkMap.Add({ FString::FromInt(_x) + FString::FromInt(_y),Chunk });
	FChunkThread* Thread = new FChunkThread(Chunk);
	
}

void UTerrainGeneratorWorldSubsystem::OnChunkCalcOver(FString _id, FChunk _chunk)
{
	FChunk* chunk = ChunkMap.Find(_id);
	if (chunk) {
		chunk = &_chunk;

		DisplayChunk(*chunk, ProceduralMesh);
	}
}
