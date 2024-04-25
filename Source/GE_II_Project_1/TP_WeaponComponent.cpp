// Copyright Epic Games, Inc. All Rights Reserved.

#include "TP_WeaponComponent.h"
#include "GE_II_Project_1Character.h"
#include "GE_II_Project_1Projectile.h"
#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GE_II_Project_1Portal.h"
#include "Engine/EngineTypes.h"

#define COLLISION_VIEWABLE ECC_GameTraceChannel4

// Sets default values for this component's properties
UTP_WeaponComponent::UTP_WeaponComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	// Default offset from the character location for projectiles to spawn
	MuzzleOffset = FVector(100.0f, 0.0f, 10.0f);

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

	static ConstructorHelpers::FClassFinder<AGE_II_Project_1Portal>Portals_Blue_(TEXT("/Game/MyGE_II_Project_1Portal_Blue"));
	if (Portals_Blue_.Class != NULL)
	{
		Portal_Blue = Portals_Blue_.Class;
	}

	static ConstructorHelpers::FClassFinder<AGE_II_Project_1Portal>Portals_Orange_(TEXT("/Game/MyGE_II_Project_1Portal_Orange"));
	if (Portals_Orange_.Class != NULL)
	{
		Portal_Orange = Portals_Orange_.Class;
	}

	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_GameTraceChannel4));
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldStatic));
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldDynamic));

	TraceParams.AddIgnoredActor(Character);
}

// Implementação da função TickComponent
void UTP_WeaponComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (Character == nullptr || Character->GetController() == nullptr)
	{
		return;
	}
	if (!havePlayerController)
	{
		// Get the player controller associated with the character
		APlayerController* PlayerController = Cast<APlayerController>(Character->GetController());
		PlayerControllerThis = PlayerController;
	}	
	if (PlayerControllerThis != nullptr)
	{
		// Get the player controller associated with the character
		SpawnRotation = PlayerControllerThis->PlayerCameraManager->GetCameraRotation();
	}
	SpawnLocation = GetOwner()->GetActorLocation() + SpawnRotation.RotateVector(MuzzleOffset);
	EndLocation = SpawnLocation + SpawnRotation.Vector() * 5000;

	if(GetWorld()->LineTraceSingleByObjectType(HitResult, SpawnLocation, EndLocation, FCollisionObjectQueryParams(ObjectTypes), TraceParams))
	{
		if (HitResult.GetActor()->GetRootComponent()->GetCollisionObjectType() == ECC_GameTraceChannel4)
		{
			can_Shoot = true;
		}
		if (HitResult.GetActor()->GetRootComponent()->GetCollisionObjectType() == ECC_WorldStatic || HitResult.GetActor()->GetRootComponent()->GetCollisionObjectType() == ECC_WorldDynamic)
		{
			can_Shoot = false;
		}
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
	if (can_Shoot)
	{
		// Try and fire a projectile
		if (ProjectileOrange != nullptr && ProjectileBlue != nullptr)
		{
			UWorld* const World = GetWorld();
			if (World != nullptr)
			{
				//Set Spawn Collision Handling Override
				ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
				DrawDebugLine(GetWorld(), SpawnLocation, EndLocation, FColor::Red, true, 5.f, 0, 1.0f);
				if (IsLeftProjectile)
				{
					AGE_II_Project_1Projectile* BULLET = World->SpawnActor<AGE_II_Project_1Projectile>(ProjectileBlue, SpawnLocation, SpawnRotation, ActorSpawnParams);
					BULLET->GetGun(IsLeftProjectile, this);
				}
				if (IsLeftProjectile == false)
				{
					AGE_II_Project_1Projectile* BULLET = World->SpawnActor<AGE_II_Project_1Projectile>(ProjectileOrange, SpawnLocation, SpawnRotation, ActorSpawnParams);
					BULLET->GetGun(IsLeftProjectile, this);
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

void UTP_WeaponComponent::SpawnPortal(bool IsBlueProjectile, FRotator SpawnRotationPortal, FVector SpawnLocationPortal)
{
	if (IsBlueProjectile)
	{
		if (SpawnFirstTimePortalBlue)
		{
			UWorld* const World = GetWorld();
			Portal_Reference_Blue = World->SpawnActor<AGE_II_Project_1Portal>(Portal_Blue, SpawnLocationPortal, SpawnRotationPortal);

			Portal_Reference_Blue->Place_Dummy_Portal(FRotator(SpawnRotationPortal), FVector(SpawnLocationPortal));
			SpawnFirstTimePortalBlue = false;
		}
		if (Portal_Reference_Blue)
		{
			Portal_Reference_Blue->Place_Dummy_Portal(FRotator(SpawnRotationPortal), FVector(SpawnLocationPortal));
		}
	}
	if (IsBlueProjectile == false)
	{
		if (SpawnFirstTimePortalOrange)
		{
			UWorld* const World = GetWorld();
			Portal_Reference_Orange = World->SpawnActor<AGE_II_Project_1Portal>(Portal_Orange, SpawnLocationPortal, SpawnRotationPortal);

			Portal_Reference_Orange->Place_Dummy_Portal(FRotator(SpawnRotationPortal), FVector(SpawnLocationPortal));
			SpawnFirstTimePortalOrange = false;
		}
		if (Portal_Reference_Orange)
		{
			Portal_Reference_Orange->Place_Dummy_Portal(FRotator(SpawnRotationPortal), FVector(SpawnLocationPortal));
		}
	}
	if (!SpawnFirstTimePortalBlue && !SpawnFirstTimePortalOrange && !portalsAreLinked)
	{
		Portal_Reference_Blue->Link(Portal_Reference_Orange);
		Portal_Reference_Orange->Link(Portal_Reference_Blue);
		portalsAreLinked = true;
	}
}