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

void UProceduralMeshGeneratorSubsystem::CreateTriangleMesh(UProceduralMeshComponent* ProceduralMesh, FVector Point1, FVector Point2, FVector Point3, int32 SectionIndex)
{
	TArray<FVector> Vertices;
	Vertices.Add(Point1);
	Vertices.Add(Point2);
	Vertices.Add(Point3);

	TArray<int32> Triangles;
	Triangles.Add(0);
	Triangles.Add(1);
	Triangles.Add(2);

	TArray<FVector> Normals;
	FVector Edge1 = Point3 - Point1;
	FVector Edge2 = Point2 - Point1;
	FVector Normal = FVector::CrossProduct(Edge1, Edge2).GetSafeNormal();

	for (int32 i = 0; i < Vertices.Num(); ++i)
	{
		Normals.Add(Normal);
	}

	TArray<FVector2D> UVs;
	UVs.Add(FVector2D(0, 0));
	UVs.Add(FVector2D(1, 0));
	UVs.Add(FVector2D(0, 1));

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

void UProceduralMeshGeneratorSubsystem::CreateSquareMesh(UProceduralMeshComponent* ProceduralMesh, FVector Point1, FVector Point2, FVector Point3, FVector Point4, int32 SectionIndex)
{
	TArray<FVector> Vertices;
	Vertices.Add(Point1); // bottom-left corner
	Vertices.Add(Point2); // bottom-right corner
	Vertices.Add(Point3); // top-left corner
	Vertices.Add(Point4); // top-right corner

	TArray<int32> Triangles;
	// first triangle
	Triangles.Add(0);
	Triangles.Add(2);
	Triangles.Add(1);
	// second triangle
	Triangles.Add(2);
	Triangles.Add(3);
	Triangles.Add(1);

	TArray<FVector> Normals;
	FVector Edge1 = Point2 - Point1;
	FVector Edge2 = Point3 - Point1;
	FVector Normal = FVector::CrossProduct(Edge1, Edge2).GetSafeNormal();

	for (int32 i = 0; i < Vertices.Num(); ++i)
	{
		Normals.Add(Normal);
	}

	TArray<FVector2D> UVs;
	UVs.Add(FVector2D(0, 0));
	UVs.Add(FVector2D(1, 0));
	UVs.Add(FVector2D(0, 1));
	UVs.Add(FVector2D(1, 1));

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
			int32 BottomLeft = x + y * Chunk.Size;
			int32 BottomRight = (x + 1) + y * Chunk.Size;
			int32 TopLeft = x + (y + 1) * Chunk.Size;
			int32 TopRight = (x + 1) + (y + 1) * Chunk.Size;
			
			Triangles.Add(BottomLeft);
			Triangles.Add(TopLeft);
			Triangles.Add(BottomRight);
			
			Triangles.Add(TopLeft);
			Triangles.Add(TopRight);
			Triangles.Add(BottomRight);
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
