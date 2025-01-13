#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "PTG/Generation/Terrain/ChunkData.h"
#include "ProceduralMeshGeneratorSubsystem.generated.h"

//////// CLASS ////////
/// System managing the generation and update of procedural meshes
UCLASS()
class PTG_API UProceduralMeshGeneratorSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	//////// UNREAL LIFECYCLE ////////
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	//////// FIELDS ////////
	/// Grid node structure
	struct FSquareIndices 
	{
		int32 bottomLeft;
		int32 bottomRight;
		int32 topLeft;
		int32 topRight;
	};
	
	//////// METHODS ////////
	/// Mesh generation
	UFUNCTION()
	void CreateChunkMesh(UProceduralMeshComponent* ProceduralMesh, const FChunk& Chunk, int32 SectionIndex = 0);

	/// Helpers
	/**
	 * @brief Utility function to calculate indices for a grid square
	 * @param x X coordinate in the grid
	 * @param y Y coordinate in the grid
	 * @param gridSize Size of the complete grid
	 * @return FSquareIndices Structure containing vertex indices for the square
	 */
	FORCEINLINE FSquareIndices GetSquareIndices(int32 x, int32 y, int32 gridSize)
	{
		return
		{
			x + y * gridSize,
			(x + 1) + y * gridSize,
			x + (y + 1) * gridSize,
			(x + 1) + (y + 1) * gridSize
		};
	}
};
