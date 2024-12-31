#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ProceduralMeshGeneratorSubsystem.generated.h"

UCLASS()
class PTG_API UProceduralMeshGeneratorSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UFUNCTION()
	void CreateTriangleMesh(UProceduralMeshComponent* ProceduralMesh, FVector Point1, FVector Point2, FVector Point3, int32 SectionIndex);

	UFUNCTION()
	void CreateSquareMesh(UProceduralMeshComponent* ProceduralMesh, FVector Point1, FVector Point2, FVector Point3, FVector Point4, int32 SectionIndex);

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
			x + y * gridSize,
			(x + 1) + y * gridSize,
			x + (y + 1) * gridSize,
			(x + 1) + (y + 1) * gridSize
		};
	}
};
