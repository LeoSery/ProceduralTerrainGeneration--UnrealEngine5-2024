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
