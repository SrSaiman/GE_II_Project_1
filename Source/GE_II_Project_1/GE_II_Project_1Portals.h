// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GE_II_Project_1Portals.generated.h"

UCLASS()
class GE_II_PROJECT_1_API AGE_II_Project_1Portals : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGE_II_Project_1Portals();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
