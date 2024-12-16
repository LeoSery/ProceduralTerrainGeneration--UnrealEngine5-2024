
#include "Generator.h"

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
