#include "PTG/Core/Player/Wanderer.h"
#include "InputActionValue.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/Controller.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

/**
 * @file Wanderer.cpp
 * @brief Implementation of the first-person character controller for terrain exploration
 */

 /**
  * @brief Constructor configuring character movement and camera setup
  * @details Initializes collision capsule, camera positioning, and movement parameters
  */
AWanderer::AWanderer()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 60.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void AWanderer::BeginPlay()
{
	Super::BeginPlay();
}

void AWanderer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

/**
 * @brief Sets up player input bindings using Enhanced Input system
 * @param PlayerInputComponent Input component to bind actions to
 */
void AWanderer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AWanderer::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AWanderer::Look);
	}
}

/**
 * @brief Handles character movement input
 * @param Value Input vector containing movement directions
 * @details Translates 2D input into world-space movement considering camera orientation
 */
void AWanderer::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// get forward vector
		const FVector ForwardDirection = FRotator(0, GetWorld()->GetFirstPlayerController()->PlayerCameraManager.Get()->GetCameraRotation().Yaw, 0).Vector();

		// get right vector 
		const FVector RightDirection = ForwardDirection.RotateAngleAxis(90, { 0,0,1 });

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

/**
 * @brief Handles camera look input
 * @param Value Input vector containing look directions
 * @details Updates camera pitch and yaw based on mouse/stick input
 */
void AWanderer::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

/**
 * @brief Toggles character sprint state
 * @param Value Boolean indicating sprint state
 * @details Modifies character max walk speed based on sprint state
 */
void AWanderer::Sprint(const FInputActionValue& Value)
{
	bool Sprint = Value.Get<bool>();

	if (Sprint)
	{
		GetCharacterMovement()->MaxWalkSpeed = 1000;
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = 500;
	}
}
