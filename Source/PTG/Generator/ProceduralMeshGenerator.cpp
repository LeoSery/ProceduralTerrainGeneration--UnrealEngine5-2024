#include "ProceduralMeshGenerator.h"


AProceduralMeshGenerator::AProceduralMeshGenerator()
{
	PrimaryActorTick.bCanEverTick = false;
	ProceduralMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("ProceduralMesh"));
	RootComponent = ProceduralMesh;
}

void AProceduralMeshGenerator::CreateTriangleMesh(FVector Point1, FVector Point2, FVector Point3)
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
    
	if (ProceduralMesh)
	{
		ProceduralMesh->ClearAllMeshSections();
		ProceduralMesh->CreateMeshSection_LinearColor(
			0,
			Vertices,
			Triangles,
			Normals,
			UVs,
			TArray<FLinearColor>(),
			TArray<FProcMeshTangent>(),
			true
			);
	}
}

void AProceduralMeshGenerator::CreateSquareMesh(FVector Point1, FVector Point2, FVector Point3, FVector Point4)
{
	TArray<FVector> Vertices;
	Vertices.Add(Point1);  // Coin bas-gauche
	Vertices.Add(Point2);  // Coin bas-droite
	Vertices.Add(Point3);  // Coin haut-gauche
	Vertices.Add(Point4);  // Coin haut-droite
	
	TArray<int32> Triangles;
	// Premier triangle
	Triangles.Add(0);
	Triangles.Add(2);
	Triangles.Add(1);
	// Second triangle
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
	
	if (ProceduralMesh)
	{
		ProceduralMesh->ClearAllMeshSections();
		ProceduralMesh->CreateMeshSection_LinearColor(
			0,
			Vertices,
			Triangles,
			Normals,
			UVs,
			TArray<FLinearColor>(),
			TArray<FProcMeshTangent>(),
			true
		);
	}
}

void AProceduralMeshGenerator::BeginPlay()
{
	Super::BeginPlay();
}
