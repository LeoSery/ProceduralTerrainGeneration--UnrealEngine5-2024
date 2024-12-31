#include "TerrainGeneratorWorldSubsystem.h"
#include "Chunk_Type.h"
#include "ProceduralMeshGeneratorSubsystem.h"
#include "PerlinNoise.h"

void UTerrainGeneratorWorldSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UTerrainGeneratorWorldSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void UTerrainGeneratorWorldSubsystem::DisplayChunk(const FChunk& Chunk, UProceduralMeshComponent* ProceduralMesh) const
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

FChunk UTerrainGeneratorWorldSubsystem::GenerateChunk(int _x, int _y, int _size, int _octaves, float _persistence, float _frequency, int _seed)
{
	FChunk Chunk;
	Chunk.size = _size;
	Chunk.Coords = FVector(_x, _y, 0);

	for (int y = _y; y < _y + _size; y++) {
		for (int x = _x; x < _x + _size; x++) {
			FVertices vertex;
			vertex.Coords = FVector(x * 100, y * 100, (UPerlinNoise::GenerateOctavePerlinValue(x, y, _octaves, _persistence, _frequency, _seed)) * 100);
			/* Put Normal Vector Calc here*/
			Chunk.vertexArray.Add(vertex);
		}
	}
	return Chunk;
}
