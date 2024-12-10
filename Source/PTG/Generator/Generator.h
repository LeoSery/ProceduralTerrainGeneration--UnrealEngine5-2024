#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PTG/Generator/ProceduralMeshGenerator.h"
#include "Generator.generated.h"

UCLASS(Blueprintable)
class PTG_API AGenerator : public AActor
{
	GENERATED_BODY()
	
public:
	AGenerator();
	
	virtual void BeginPlay() override;
	
	void GenerateTerrain();

private:
	int m_width;
	int m_height;

	AProceduralMeshGenerator * m_meshGenerator;
};
