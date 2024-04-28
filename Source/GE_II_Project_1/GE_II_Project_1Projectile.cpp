// Copyright Epic Games, Inc. All Rights Reserved.

#include "GE_II_Project_1Projectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "TP_WeaponComponent.h"

AGE_II_Project_1Projectile::AGE_II_Project_1Projectile() 
{
	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(20.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &AGE_II_Project_1Projectile::OnHit);		// set up a notification for when this component hits something blocking


	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	// Set as root component
	RootComponent = CollisionComp;

	//projectile mesh
	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));
	ProjectileMesh->SetRelativeScale3D(FVector(0.125f, 0.125f, 0.125f));
	ProjectileMesh->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->ProjectileGravityScale = 0.f;

	// Die after 3 seconds by default
	InitialLifeSpan = 10.0f;

}


void AGE_II_Project_1Projectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (Hit.GetActor()->GetRootComponent()->GetCollisionObjectType() == ECC_GameTraceChannel4)
	{
		FVector ImpactNormalPortal = Hit.ImpactNormal;
		FRotator SpawnRotationPortal_ = ImpactNormalPortal.Rotation();
		FVector SpawnLocationPortal_ = Hit.ImpactPoint + (Hit.ImpactNormal * FVector(0.01f, 0.01f, 0.01f));
		Weapon->SpawnPortal(BlueProjectile, SpawnRotationPortal_, SpawnLocationPortal_);
		Destroy();
	}
	else
	{
		Destroy();
	}
}

void AGE_II_Project_1Projectile::GetGun(bool IsBlueProjectile, UTP_WeaponComponent* gunReference)
{
	Weapon = gunReference;
	BlueProjectile = IsBlueProjectile;
}
