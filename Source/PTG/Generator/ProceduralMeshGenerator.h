#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "ProceduralMeshGenerator.generated.h"

UCLASS()
class PTG_API AProceduralMeshGenerator : public AActor
{
	GENERATED_BODY()

public:
	AProceduralMeshGenerator();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProceduralMesh")
	UProceduralMeshComponent * ProceduralMesh;

	UFUNCTION(BlueprintCallable, Category = "ProceduralMesh")
	void CreateTriangleMesh(FVector Point1, FVector Point2, FVector Point3, int32 sectionIndex);

	UFUNCTION(BlueprintCallable, Category = "ProceduralMesh")
	void CreateSquareMesh(FVector Point1, FVector Point2, FVector Point3, FVector Point4, int32 sectionIndex);

	struct FSquareIndices 
	{
		int32 bottomLeft;
		int32 bottomRight;
		int32 topLeft;
		int32 topRight;
	};

	FORCEINLINE FSquareIndices GetSquareIndices(int32 x, int32 y, int32 gridSize)
	{
		return {
			x + y * gridSize,              // bottomLeft
			(x + 1) + y * gridSize,        // bottomRight
			x + (y + 1) * gridSize,        // topLeft
			(x + 1) + (y + 1) * gridSize   // topRight
		};
	}

protected:
	virtual void BeginPlay() override;
};
