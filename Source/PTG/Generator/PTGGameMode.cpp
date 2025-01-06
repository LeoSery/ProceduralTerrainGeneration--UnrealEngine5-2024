#include "PTGGameMode.h"
#include "ChunkManagerWorldSubsystem.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TerrainGeneratorWorldSubsystem.h"
#include "GameFramework/PlayerStart.h"

APTGGameMode::APTGGameMode()
{
	PrimaryActorTick.bCanEverTick = true;

	// Default terrain parameters
	TerrainParameters.Frequency = 0.1f;
	TerrainParameters.Octaves = 4;
	TerrainParameters.Persistence = 0.5f;
	TerrainParameters.Seed = 420;
	TerrainParameters.HeightFactor = 100;

	// Default biomes parameters
	BiomesParameters.Frequency = 0.01f;
	BiomesParameters.Octaves = 1;
	BiomesParameters.Persistence = 0.5f;
	BiomesParameters.Seed = 69;
	BiomesParameters.HeightFactor = 0;
}

void APTGGameMode::BeginPlay()
{
	Super::BeginPlay();

	ChunkManager = GetWorld()->GetSubsystem<UChunkManagerWorldSubsystem>();
	if (!ChunkManager)
	{
		return;
	}

	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		if (MainMenuClass)
		{
			if (UUserWidget* MainMenu = CreateWidget<UUserWidget>(PC, MainMenuClass))
			{
				MainMenu->AddToViewport();
			}
		}
	}
}

void APTGGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APTGGameMode::GenerateInitialChunks()
{
	if (!ChunkManager)
	{
		return;
	}

	// Disable player movement during generation
	if (ACharacter* PlayerCharacter = Cast<ACharacter>(GetWorld()->GetFirstPlayerController()->GetPawn()))
	{
		PlayerCharacter->GetCharacterMovement()->SetMovementMode(MOVE_Flying);
	}

	SpawnPlayer();
	
	ChunkManager->SetTerrainParameters(TerrainParameters);
	ChunkManager->SetBiomesParameters(BiomesParameters);

	if (UTerrainGeneratorWorldSubsystem* TerrainGenerator = GetWorld()->GetSubsystem<UTerrainGeneratorWorldSubsystem>())
	{
		TerrainGenerator->SetMaterial(Material);	
	}
	
	ChunkManager->OnLoadingProgressUpdate.AddDynamic(this, &APTGGameMode::HandleGenerationProgress);
	ChunkManager->InitialChunkGeneration(ChunkManager->GetRenderDistance());
}

void APTGGameMode::HandleGenerationProgress(int32 Current, int32 Total)
{
	UE_LOG(LogTemp, Warning, TEXT("Generation Progress: %d/%d"), Current, Total);
	
	if (Current >= Total)
	{
		UE_LOG(LogTemp, Warning, TEXT("Generation Complete, Repositioning Player"));
		RepositionPlayerToGround();
	}
}

void APTGGameMode::SpawnPlayer()
{
	FVector InitialChunkCenter(
		(ChunkManager->GetChunkSize() - 1) * 100 / 2.0f,  
		(ChunkManager->GetChunkSize() - 1) * 100 / 2.0f,  
		200.0f                       
	);

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	GetWorld()->SpawnActor<APlayerStart>(
	   APlayerStart::StaticClass(),
	   InitialChunkCenter,
	   FRotator::ZeroRotator,
	   SpawnParams
   );
}

void APTGGameMode::RepositionPlayerToGround()
{
	if (!ChunkManager)
	{
		UE_LOG(LogTemp, Error, TEXT("ChunkManager is null in RepositionPlayerToGround"));
		return;
	}

	if (UTerrainGeneratorWorldSubsystem* TerrainGenerator = GetWorld()->GetSubsystem<UTerrainGeneratorWorldSubsystem>())
	{
		int64 CentralChunkId = ChunkData::GetChunkIdFromCoordinates(0, 0);
		const FChunk* CentralChunk = TerrainGenerator->GetChunk(CentralChunkId);
        
		if (!CentralChunk || CentralChunk->VertexArray.Num() == 0)
		{
			UE_LOG(LogTemp, Error, TEXT("Central chunk not found or empty"));
			return;
		}
        
		int32 CenterX = (ChunkManager->GetChunkSize() - 1) / 2;
		int32 CenterY = (ChunkManager->GetChunkSize() - 1) / 2;
		int32 CenterIndex = CenterX + CenterY * ChunkManager->GetChunkSize();
        
		float TerrainHeight = CentralChunk->VertexArray[CenterIndex].Coords.Z;
        
		if (ACharacter* PlayerCharacter = Cast<ACharacter>(GetWorld()->GetFirstPlayerController()->GetPawn()))
		{
			FVector CurrentLocation = PlayerCharacter->GetActorLocation();
			FVector NewLocation(CurrentLocation.X, CurrentLocation.Y, TerrainHeight + 100.0f);

			UE_LOG(LogTemp, Warning, TEXT("Repositioning player to X:%f Y:%f Z:%f"), NewLocation.X, NewLocation.Y, NewLocation.Z);
			PlayerCharacter->SetActorLocation(NewLocation);
			PlayerCharacter->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Player character not found"));
		}
	}
	else 
	{
		UE_LOG(LogTemp, Error, TEXT("TerrainGenerator not found in RepositionPlayerToGround"));
	}
}
