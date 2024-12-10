#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "ProceduralMesh.generated.h"

UCLASS()
class PTG_API AProceduralMesh : public AActor
{
	GENERATED_BODY()

public:
	AProceduralMesh();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProceduralMesh")
	UProceduralMeshComponent * ProceduralMesh;

	UFUNCTION(BlueprintCallable, Category = "ProceduralMesh")
	static void CreateTriangleMesh(FVector Point1, FVector Point2, FVector Point3);

protected:
	virtual void BeginPlay() override;
};
