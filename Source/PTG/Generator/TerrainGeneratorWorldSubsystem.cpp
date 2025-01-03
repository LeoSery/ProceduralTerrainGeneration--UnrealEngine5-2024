#include "TerrainGeneratorWorldSubsystem.h"
#include "ChunkData.h"
#include "ChunkThread.h"
#include "ProceduralMeshGeneratorSubsystem.h"

void UTerrainGeneratorWorldSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UTerrainGeneratorWorldSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void UTerrainGeneratorWorldSubsystem::GenerateChunk(int32 X, int32 Y, int32 Size, const FPerlinParameters& TerrainParameters, const FPerlinParameters& BiomesParameters)
{
	FChunk NewChunk;
	NewChunk.Size = Size;
	NewChunk.Coords = FVector(X, Y, 0);
	NewChunk.Id = ChunkData::MakeChunkId(X, Y);

	ChunkMap.Add(NewChunk.Id, NewChunk);
	FChunkThread* Thread = new FChunkThread(NewChunk, TerrainParameters,BiomesParameters);
	Thread->OnCalcOver.AddUObject(this, &UTerrainGeneratorWorldSubsystem::OnChunkCalcOver);
	
}

void UTerrainGeneratorWorldSubsystem::DisplayChunk(int64 ChunkId)
{
	if (const FChunk* ChunkToDisplay = ChunkMap.Find(ChunkId))
	{
		DisplayChunkInternal(*ChunkToDisplay);
	}
}

bool UTerrainGeneratorWorldSubsystem::DestroyChunk(int64 ChunkId)
{
	if (MeshMap.Find(ChunkId) && ChunkMap.Find(ChunkId))
	{
		// TODO: Add logic to clean chunk procedural mesh sections
		AActor* Mesh = MeshMap.FindAndRemoveChecked(ChunkId);
		Mesh->Destroy();
		return ChunkMap.Remove(ChunkId) > 0;
	}
	return false;
}

void UTerrainGeneratorWorldSubsystem::OnChunkCalcOver(int64 _id, FChunk _chunk)
{
	FChunk* chunk = ChunkMap.Find(_id);
	if (chunk) {
		ChunkMap.Add(_id, _chunk);
		UE_LOG(LogTemp, Warning, TEXT("Generating Chunk Soone to Display"));
		OnChunkGenerationComplete.Broadcast(_id);
	}
}

void UTerrainGeneratorWorldSubsystem::DisplayChunkInternal(const FChunk& Chunk)
{
	FGenerationStats& Stats = GenerationStats.Add(Chunk.Id);
	Stats.StartTime = FPlatformTime::Seconds();
	Stats.ChunkSize = Chunk.Size;
	Stats.VertexCount = Chunk.Size * Chunk.Size;
	Stats.TriangleCount = (Chunk.Size - 1) * (Chunk.Size - 1) * 2;
	
	AActor* MeshOwner = nullptr;
	if (AActor* const* ExistingMeshOwner = MeshMap.Find(Chunk.Id))
	{
		MeshOwner = *ExistingMeshOwner;
	}
	else {
		MeshOwner = GetWorld()->SpawnActor<AActor>();

		// Create the procedural mesh component with the actor as its owner
		UProceduralMeshComponent* ProceduralMesh = NewObject<UProceduralMeshComponent>(
			MeshOwner,  // Set the owner to our newly created actor
			UProceduralMeshComponent::StaticClass(),
			NAME_None,
			RF_Transient
		);

		// Attach the component to the actor's root
		ProceduralMesh->SetupAttachment(MeshOwner->GetRootComponent());
		ProceduralMesh->RegisterComponent();

		MeshMap.Emplace(Chunk.Id, MeshOwner);
	}
	
	if (UProceduralMeshGeneratorSubsystem* MeshGenerator = GetWorld()->GetGameInstance()->GetSubsystem<UProceduralMeshGeneratorSubsystem>())
	{
		MeshGenerator->CreateChunkMesh(
			MeshOwner->FindComponentByClass<UProceduralMeshComponent>(),
			Chunk,
			0
		);
	}

	Stats.VertexCount = Chunk.Size * Chunk.Size;
	Stats.TriangleCount = (Chunk.Size - 1) * (Chunk.Size - 1) * 2;
	Stats.EndTime = FPlatformTime::Seconds();
	double GenerationTime = (Stats.EndTime - Stats.StartTime) * 1000;
	
	UE_LOG(LogTemp, Warning, TEXT("Chunk %lld Generation Stats:"), Chunk.Id);
	UE_LOG(LogTemp, Warning, TEXT("  Generation Time: %.2f ms"), GenerationTime);
	UE_LOG(LogTemp, Warning, TEXT("  Size: %d x %d"), Stats.ChunkSize, Stats.ChunkSize);
	UE_LOG(LogTemp, Warning, TEXT("  Vertices: %d"), Stats.VertexCount);
	UE_LOG(LogTemp, Warning, TEXT("  Triangles: %d"), Stats.TriangleCount);
}
