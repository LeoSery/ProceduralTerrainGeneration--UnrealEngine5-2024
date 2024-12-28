
#include "Generator.h"
#include <PTG/Generator/PerlinNoise.h>

AGenerator::AGenerator()
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

	m_meshGenerator->CreateTriangleMesh(FVector(0, 0, 0), FVector(0, 100, 0), FVector(100, 0, 0));	
}

FChunk AGenerator::GenerateChunk(int _x, int _y, int _size, int _octaves, float _persistence, float _frequency)
{
	FChunk chunk;
	chunk.size = _size;
	chunk.Coords = FVector(_x, _y,0);
	APerlinNoise perlinPinPin;
	perlinPinPin.SetSeed(42);
	for (int y = _y; y < _y + _size; y++) {
		for (int x = _x; x < _x + _size; x++) {
			FVertices vertex;
			vertex.Coords = FVector(x, y, perlinPinPin.GenerateOctavePerlinValue(_x,_y,_octaves,_persistence,_frequency));
			/* Put Normal Vector Calc here*/
			chunk.vertexArray.Add(vertex);
		}
	}
	return FChunk();
}
