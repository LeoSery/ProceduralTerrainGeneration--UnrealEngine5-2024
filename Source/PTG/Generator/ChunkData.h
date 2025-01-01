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

	UPROPERTY()
	int32 Octaves;

	UPROPERTY()
	float Frequency;

	UPROPERTY()
	float Persistence;

	UPROPERTY()
	int32 Seed;
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
	FORCEINLINE int64 MakeChunkId(int32 X, int32 Y)
	{
		return ((int64)X << 32) | ((int64)Y & 0xFFFFFFFF);
	}

	FORCEINLINE void GetChunkCoords(int64 Id, int32& OutX, int32& OutY)
	{
		OutX = (int32)(Id >> 32);
		OutY = (int32)(Id & 0xFFFFFFFF);
	}
}