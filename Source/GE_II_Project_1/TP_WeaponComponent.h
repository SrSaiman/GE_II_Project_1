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

	UPROPERTY(EditDefaultsOnly, Category=Projectile)
	TSubclassOf<class AGE_II_Project_1Projectile> ProjectileBlue;

	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	TSubclassOf<class AGE_II_Project_1Projectile> ProjectileOrange;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class AGE_II_Project_1Portals* Portals_Reference;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class AGE_II_Project_1Portals> Portals;

	//UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	//class UPhysicalMaterial* Portal_Supporting;

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

	UFUNCTION(BlueprintCallable)
	void SpawnPortal(bool IsOrangeProjetile, FRotator SpawnRotationPortal, FVector SpawnLocationPortal);

	void MoveTo(FVector TargetLocation, FVector InitialLocation , float Duration);

private:
	// Variables to store movement parameters
	float MoveTime;
	float StartTime;


protected:
	/** Ends gameplay for this component. */
	UFUNCTION()
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Projectile)
	bool IsLeftProjectile;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Projectile)
	bool SpawnFirstTimePortals = true;

private:
	/** The Character holding this weapon*/
	AGE_II_Project_1Character* Character;
};
