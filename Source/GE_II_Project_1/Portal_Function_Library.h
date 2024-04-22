// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "Portal_Function_Library.generated.h"

/**
 * 
 */
UCLASS()
class GE_II_PROJECT_1_API UPortal_Function_Library : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	static void ResizeRenderTarget(UTextureRenderTarget2D* RenderTarget, float size_x,float size_y);
};
