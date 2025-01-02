#include "ProceduralMeshGeneratorSubsystem.h"

#include "ProceduralMeshComponent.h"

void UProceduralMeshGeneratorSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UProceduralMeshGeneratorSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void UProceduralMeshGeneratorSubsystem::CreateChunkMesh(UProceduralMeshComponent* ProceduralMesh, const FChunk& Chunk, int32 SectionIndex)
{
	TArray<FVector> Vertices;
	TArray<int32> Triangles;
	TArray<FVector> Normals;
	TArray<FVector2D> UVs;

	const int32 NumVertices = Chunk.Size * Chunk.Size;
	const int32 NumTriangles = (Chunk.Size - 1) * (Chunk.Size - 1) * 2 * 3;

	Vertices.Reserve(NumVertices);
	Triangles.Reserve(NumTriangles);
	Normals.Reserve(NumVertices);
	UVs.Reserve(NumVertices);

	for (int32 y = 0; y < Chunk.Size; y++)
	{
		for (int32 x = 0; x < Chunk.Size; x++)
		{
			int32 Index = x + y * Chunk.Size;
			Vertices.Add(Chunk.VertexArray[Index].Coords);
			
			float U = static_cast<float>(x) / (Chunk.Size - 1);
			float V = static_cast<float>(y) / (Chunk.Size - 1);
			UVs.Add(FVector2D(U, V));
			
			Normals.Add(FVector(0.0f, 0.0f, 1.0f));
		}
	}

	for (int32 y = 0; y < Chunk.Size - 1; y++)
	{
		for (int32 x = 0; x < Chunk.Size - 1; x++)
		{
			FSquareIndices Square = GetSquareIndices(x, y, Chunk.Size);
			
			Triangles.Add(Square.bottomLeft);
			Triangles.Add(Square.topLeft);
			Triangles.Add(Square.bottomRight);
            
			Triangles.Add(Square.topLeft);
			Triangles.Add(Square.topRight);
			Triangles.Add(Square.bottomRight);
		}
	}

	Normals.SetNum(Vertices.Num());

	for (FVector& Normal : Normals)
	{
		Normal = FVector::ZeroVector;
	}
	
	for (int32 i = 0; i < Triangles.Num(); i += 3)
	{
		const FVector& V0 = Vertices[Triangles[i]];
		const FVector& V1 = Vertices[Triangles[i + 1]];
		const FVector& V2 = Vertices[Triangles[i + 2]];

		const FVector Edge1 = V1 - V0;
		const FVector Edge2 = V2 - V0;
		const FVector TriangleNormal = FVector::CrossProduct(Edge2, Edge1).GetSafeNormal();
		
		Normals[Triangles[i]] += TriangleNormal;
		Normals[Triangles[i + 1]] += TriangleNormal;
		Normals[Triangles[i + 2]] += TriangleNormal;
	}

	// Normalize all normals
	for (FVector& Normal : Normals)
	{
		if (!Normal.IsNearlyZero())
		{
			Normal.Normalize();
		}
		else
		{
			Normal = FVector(0, 0, 1);
		}
	}

	ProceduralMesh->CreateMeshSection_LinearColor(
		SectionIndex,
		Vertices,
		Triangles,
		Normals,
		UVs,
		TArray<FLinearColor>(),
		TArray<FProcMeshTangent>(),
		true
	);
}
