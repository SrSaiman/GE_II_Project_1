// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine.h"
#include "Components/SkeletalMeshComponent.h"
#include "TP_WeaponComponent.generated.h"

class AGE_II_Project_1Character;

UCLASS(Blueprintable, BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GE_II_PROJECT_1_API UTP_WeaponComponent : public USkeletalMeshComponent
{
	GENERATED_BODY()

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction);

public:

	UPROPERTY(EditDefaultsOnly, Category=Projectile)
	TSubclassOf<class AGE_II_Project_1Projectile> ProjectileBlue;

	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	TSubclassOf<class AGE_II_Project_1Projectile> ProjectileOrange;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class AGE_II_Project_1Portal* Portal_Reference_Blue;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class AGE_II_Project_1Portal* Portal_Reference_Orange = nullptr;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class AGE_II_Project_1Portal> Portal_Blue;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class AGE_II_Project_1Portal> Portal_Orange;

	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	USoundBase* FireSound;
	
	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	UAnimMontage* FireAnimation;

	/** Gun muzzle's offset from the characters location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	FVector MuzzleOffset;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputMappingContext* FireMappingContext;

	/** Fire Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* FireActionLeft;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* FireActionRight;

	/** Sets default values for this component's properties */
	UTP_WeaponComponent();

	/** Attaches the actor to a FirstPersonCharacter */
	UFUNCTION(BlueprintCallable, Category="Weapon")
	void AttachWeapon(AGE_II_Project_1Character* TargetCharacter);

	/** Make the weapon Fire a Projectile */
	UFUNCTION(BlueprintCallable, Category="Weapon")
	void Fire();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void FireLeft();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void FireRight();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void CheckIfCanShoot();

	UFUNCTION()
	void SpawnPortal(bool IsOrangeProjetile, FRotator SpawnRotationPortal, FVector SpawnLocationPortal);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;

protected:
	/** Ends gameplay for this component. */
	UFUNCTION()
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Projectile)
	bool IsLeftProjectile;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Projectile)
	bool SpawnFirstTimePortalBlue = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Projectile)
	bool SpawnFirstTimePortalOrange = true;

	bool portalsAreLinked = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool can_Shoot = false;

private:
	/** The Character holding this weapon*/
	AGE_II_Project_1Character* Character;
	APlayerController* PlayerControllerThis;
	FActorSpawnParameters ActorSpawnParams;
	FRotator SpawnRotation;
	FVector SpawnLocation;
	FVector EndLocation;
	FHitResult HitResult;
	FCollisionQueryParams TraceParams;
	bool havePlayerController = false;
};
