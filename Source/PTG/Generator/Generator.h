#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PTG/Generator/ProceduralMeshGenerator.h"
#include <PTG/Generator/Chunk_Type.h>

#include "Generator.generated.h"

UCLASS(Blueprintable)
class PTG_API AGenerator : public AActor
{
	GENERATED_BODY()
	
public:
	AGenerator();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProceduralMesh")
	UMaterialInterface* Material;
	
	virtual void BeginPlay() override;
	
	void GenerateTerrain();
	void DisplayChunk(const FChunk& Chunk) const;
	FChunk GenerateChunk(int _x, int _y, int _size, int _octaves, float _persistence, float _frequency);

private:
	int m_width;
	int m_height;

	AProceduralMeshGenerator * m_meshGenerator;
};
