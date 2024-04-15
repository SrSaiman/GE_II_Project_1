// Copyright Epic Games, Inc. All Rights Reserved.


#include "TP_WeaponComponent.h"
#include "GE_II_Project_1Character.h"
#include "GE_II_Project_1Projectile.h"
#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

// Sets default values for this component's properties
UTP_WeaponComponent::UTP_WeaponComponent()
{
	static ConstructorHelpers::FClassFinder<AGE_II_Project_1Projectile>ProjectileO(TEXT("/Game/FirstPerson/Blueprints/Projectile_Orange"));
	if (ProjectileO.Class != NULL)
	{
		ProjectileOrange = ProjectileO.Class;
	}
	static ConstructorHelpers::FClassFinder<AGE_II_Project_1Projectile>ProjectileB(TEXT("/Game/FirstPerson/Blueprints/Projectile_Blue"));
	if (ProjectileB.Class != NULL)
	{
		ProjectileBlue = ProjectileB.Class;
	}
}

void UTP_WeaponComponent::FireLeft()
{
	IsLeftProjectile = true;
	Fire();
}

void UTP_WeaponComponent::FireRight()
{
	IsLeftProjectile = false;
	Fire();
}

void UTP_WeaponComponent::Fire()
{
	if (Character == nullptr || Character->GetController() == nullptr)
	{
		return;
	}

	// Try and fire a projectile
	if (ProjectileBlue != nullptr && ProjectileOrange != nullptr)
	{
		UWorld* const World = GetWorld();
		if (World != nullptr)
		{
			APlayerController* PlayerController = Cast<APlayerController>(Character->GetController());
			const FRotator SpawnRotation = PlayerController->PlayerCameraManager->GetCameraRotation();
			// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
			const FVector SpawnLocation = GetOwner()->GetActorLocation() + SpawnRotation.RotateVector(MuzzleOffset);
	
			//Set Spawn Collision Handling Override
			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
	
			if (IsLeftProjectile)
			{
				World->SpawnActor<AGE_II_Project_1Projectile>(ProjectileBlue, SpawnLocation, SpawnRotation, ActorSpawnParams);

			}
			if (IsLeftProjectile == false)
			{
				World->SpawnActor<AGE_II_Project_1Projectile>(ProjectileOrange, SpawnLocation, SpawnRotation, ActorSpawnParams);
			}
		}
	}
	
	// Try and play the sound if specified
	if (FireSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, Character->GetActorLocation());
	}
	
	// Try and play a firing animation if specified
	if (FireAnimation != nullptr)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = Character->GetMesh1P()->GetAnimInstance();
		if (AnimInstance != nullptr)
		{
			AnimInstance->Montage_Play(FireAnimation, 1.f);
		}
	}
}

void UTP_WeaponComponent::AttachWeapon(AGE_II_Project_1Character* TargetCharacter)
{
	Character = TargetCharacter;

	// Check that the character is valid, and has no rifle yet
	if (Character == nullptr || Character->GetHasRifle())
	{
		return;
	}

	// Attach the weapon to the First Person Character
	FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
	AttachToComponent(Character->GetMesh1P(), AttachmentRules, FName(TEXT("GripPoint")));
	
	// switch bHasRifle so the animation blueprint can switch to another animation set
	Character->SetHasRifle(true);

	// Set up action bindings
	if (APlayerController* PlayerController = Cast<APlayerController>(Character->GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			// Set the priority of the mapping to 1, so that it overrides the Jump action with the Fire action when using touch input
			Subsystem->AddMappingContext(FireMappingContext, 1);
		}

		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerController->InputComponent))
		{
			// Fire
				EnhancedInputComponent->BindAction(FireActionLeft, ETriggerEvent::Triggered, this, &UTP_WeaponComponent::FireLeft);
				EnhancedInputComponent->BindAction(FireActionRight, ETriggerEvent::Triggered, this, &UTP_WeaponComponent::FireRight);
		}
	}
}

void UTP_WeaponComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (Character == nullptr)
	{
		return;
	}

	if (APlayerController* PlayerController = Cast<APlayerController>(Character->GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->RemoveMappingContext(FireMappingContext);
		}
	}
}