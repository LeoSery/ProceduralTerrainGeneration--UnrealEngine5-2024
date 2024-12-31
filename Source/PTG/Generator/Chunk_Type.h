#pragma once

#include "CoreMinimal.h"
#include "Chunk_Type.generated.h"

USTRUCT(BlueprintType)
struct FVertices
{
    GENERATED_BODY()

    UPROPERTY()
    FVector Coords;

    UPROPERTY()
    FVector Normal;
};



USTRUCT(BlueprintType)
struct FChunk
{
    GENERATED_BODY()

    UPROPERTY()
    TArray<FVertices> vertexArray;

    UPROPERTY()
    int32 size;

    UPROPERTY()
    FVector Coords;

    UPROPERTY()
    FString Id;
};

USTRUCT(BlueprintType)
struct FPerlinParameters {

    GENERATED_BODY()

public :

    UPROPERTY()
    int octaves;

    UPROPERTY()
    float frequency;

    UPROPERTY()
    float persistence;

    UPROPERTY()
    int seed;
};