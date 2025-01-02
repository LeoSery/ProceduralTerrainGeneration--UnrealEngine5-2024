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

void UTerrainGeneratorWorldSubsystem::GenerateChunk(int32 X, int32 Y, int32 Size, const FPerlinParameters& Parameters)
{
	FChunk NewChunk;
	NewChunk.Size = Size;
	NewChunk.Coords = FVector(X, Y, 0);
	NewChunk.Id = ChunkData::MakeChunkId(X, Y);

	ChunkMap.Add(NewChunk.Id, NewChunk);
	FChunkThread* Thread = new FChunkThread(NewChunk,Parameters);
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
	AActor* MeshOwner = MeshMap.Find(Chunk.Id)[1];
	if (MeshMap.Find(Chunk.Id)) {

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
		for (int32 x = 0; x < Chunk.Size - 1; x++)
		{
			for (int32 y = 0; y < Chunk.Size - 1; y++)
			{
				const auto Indices = MeshGenerator->GetSquareIndices(x, y, Chunk.Size);
                
				MeshGenerator->CreateSquareMesh(
					ProceduralMesh,
					Chunk.VertexArray[Indices.bottomLeft].Coords,
					Chunk.VertexArray[Indices.bottomRight].Coords,
					Chunk.VertexArray[Indices.topLeft].Coords,
					Chunk.VertexArray[Indices.topRight].Coords,
					x + y * (Chunk.Size - 1)
				);
			}
		}
	}
}
