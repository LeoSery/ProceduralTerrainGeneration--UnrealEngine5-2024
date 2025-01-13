#include "PTGGameMode.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerStart.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PTG/Generation/Subsystems/ChunkManagerWorldSubsystem.h"
#include "PTG/Generation/Subsystems/TerrainGeneratorWorldSubsystem.h"

/**
 * @file PTGGameMode.cpp
 * @brief Main game mode handling terrain generation and player management
 */

 /**
  * @brief Constructor initializing default terrain and biome parameters
  */
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

	RenderDistance = 1;
}

/**
 * @brief Initializes subsystems and creates main menu
 */
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

/**
 * @brief Starts initial terrain chunk generation
 * @details Temporarily disables player movement, positions player and triggers chunk generation
 */
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
	ChunkManager->SetRenderDistance(RenderDistance);

	if (UTerrainGeneratorWorldSubsystem* TerrainGenerator = GetWorld()->GetSubsystem<UTerrainGeneratorWorldSubsystem>())
	{
		TerrainGenerator->SetMaterial(Material);	
	}
	
	ChunkManager->OnLoadingProgressUpdate.AddDynamic(this, &APTGGameMode::HandleGenerationProgress);
	ChunkManager->InitialChunkGeneration(ChunkManager->GetRenderDistance());
}

/**
 * @brief Handles chunk generation progress updates
 * @param Current Number of chunks currently generated
 * @param Total Total number of chunks to generate
 */
void APTGGameMode::HandleGenerationProgress(int32 Current, int32 Total)
{
	UE_LOG(LogTemp, Warning, TEXT("Generation Progress: %d/%d"), Current, Total);
	
	if (Current >= Total)
	{
		UE_LOG(LogTemp, Warning, TEXT("Generation Complete, Repositioning Player"));
		RepositionPlayerToGround();
	}
}

/**
 * @brief Spawns player at initial chunk center
 */
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

/**
 * @brief Repositions player to proper height after terrain generation
 */
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
			FVector NewLocation(CurrentLocation.X, CurrentLocation.Y, TerrainHeight + 400.0f);

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
