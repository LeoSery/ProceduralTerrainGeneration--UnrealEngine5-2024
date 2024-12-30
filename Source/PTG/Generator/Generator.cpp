
#include "Generator.h"
#include <PTG/Generator/PerlinNoise.h>

AGenerator::AGenerator(): Material(nullptr), m_width(0), m_height(0), m_meshGenerator(nullptr)
{
}

void AGenerator::BeginPlay()
{
	Super::BeginPlay();

	if (m_meshGenerator == nullptr)
	{
		m_meshGenerator = GetWorld()->SpawnActor<AProceduralMeshGenerator>();
	}
	
	GenerateTerrain();
}

void AGenerator::GenerateTerrain()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Generating terrain..."));

	if (m_meshGenerator)
	{
		m_meshGenerator->ProceduralMesh->ClearAllMeshSections(); // Clear all mesh sections before generating new ones
		
		// DEBUG TEST
		/*m_meshGenerator->CreateSquareMesh(
			FVector(0, 0, 0),    // Point1 (bas-gauche)
			FVector(100, 0, 0),  // Point2 (bas-droite)
			FVector(0, 100, 0),  // Point3 (haut-gauche)
			FVector(100, 100, 0), // Point4 (haut-droite)
			464664 // random section index for testing with no conflicts with real index in noise generation
		);*/

		// DISPLAY A CHUNK
		DisplayChunk(GenerateChunk(0, 0, 32, 4, 0.5, 0.1, 4));

		// APPLY MATERIAL TO CHUNK MESHES
		if (Material)
		{
			m_meshGenerator->ProceduralMesh->SetMaterial(0, Material);
		}
	}	
}

void AGenerator::DisplayChunk(const FChunk& Chunk) const
{
	if (m_meshGenerator)
	{
		int32 sectionIndex = 0;
		for (int32 x = 0; x < Chunk.size - 1; x++)
		{
			for (int32 y = 0;  y < Chunk.size - 1; y++)
			{
				const AProceduralMeshGenerator::FSquareIndices Indices = m_meshGenerator->GetSquareIndices(x, y, Chunk.size);
            
				m_meshGenerator->CreateSquareMesh(
					Chunk.vertexArray[Indices.bottomLeft].Coords,
					Chunk.vertexArray[Indices.bottomRight].Coords,
					Chunk.vertexArray[Indices.topLeft].Coords,
					Chunk.vertexArray[Indices.topRight].Coords,
					sectionIndex++
				);
			}
		}
	}
}

FChunk AGenerator::GenerateChunk(int _x, int _y, int _size, int _octaves, float _persistence, float _frequency,int _seed)
{
	FChunk Chunk;
	Chunk.size = _size;
	Chunk.Coords = FVector(_x, _y,0);

	for (int y = _y; y < _y + _size; y++) {
		for (int x = _x; x < _x + _size; x++) {
			FVertices vertex;
			vertex.Coords = FVector(x*100, y*100, (UPerlinNoise::GenerateOctavePerlinValue(x,y,_octaves,_persistence,_frequency,_seed))*100);
			/* Put Normal Vector Calc here*/
			Chunk.vertexArray.Add(vertex);
		}
	}
	return Chunk;
}
