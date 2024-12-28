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
	void CreateTriangleMesh(FVector Point1, FVector Point2, FVector Point3);

	UFUNCTION(BlueprintCallable, Category = "ProceduralMesh")
	void CreateSquareMesh(FVector Point1, FVector Point2, FVector Point3, FVector Point4);

protected:
	virtual void BeginPlay() override;
};
