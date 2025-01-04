#pragma once

#include "CoreMinimal.h"
#include "ChunkData.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnChunkGenerationComplete, int64);

USTRUCT()
struct FVertices
{
	GENERATED_BODY()

	UPROPERTY()
	FVector Coords;

	UPROPERTY()
	FVector Normal;
};

USTRUCT()
struct FPerlinParameters
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	int32 Octaves;

	UPROPERTY(EditAnywhere)
	float Frequency;

	UPROPERTY(EditAnywhere)
	float Persistence;

	UPROPERTY(EditAnywhere)
	int32 Seed;

	UPROPERTY(EditAnywhere)
	int32 HeightFactor;
};

USTRUCT()
struct FChunk
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FVertices> VertexArray;

	UPROPERTY()
	int32 Size;

	UPROPERTY()
	FVector Coords;

	UPROPERTY()
	int64 Id;
};

namespace ChunkData
{
	FORCEINLINE int64 GetChunkIdFromCoordinates(int32 X, int32 Y)
	{
		return ((int64)X << 32) | ((int64)Y & 0xFFFFFFFF);
	}

	FORCEINLINE int32 GetInitialChunkCount(int32 RenderDistance)
	{
		return (2 * RenderDistance + 1) * (2 * RenderDistance + 1);
	}
}