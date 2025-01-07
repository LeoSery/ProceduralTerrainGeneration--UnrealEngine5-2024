#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Wanderer.generated.h"

//////// FORWARD DECLARATION ////////
/// Class 
class UInputMappingContext;
class UCameraComponent;
class UInputAction;

/// Struct
struct FInputActionValue;

//////// CLASS ////////
/// First person character controller for terrain exploration
UCLASS()
class PTG_API AWanderer : public ACharacter
{
	GENERATED_BODY()

public:
	//////// CONSTRUCTORS ////////
	AWanderer();

	//////// UNREAL LIFECYCLE ////////
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//////// METHODS ////////
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Sprint(const FInputActionValue& Value);

protected:
	//////// UNREAL LIFECYCLE ////////
	virtual void BeginPlay() override;

private:
	//////// FIELD ////////
	/// Mapping context
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/// Input actions
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SprintAction;

	/// Camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;
	
};
