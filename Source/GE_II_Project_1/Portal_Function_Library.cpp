// Fill out your copyright notice in the Description page of Project Settings.


#include "Portal_Function_Library.h"
#include "Engine/TextureRenderTarget2D.h"

void UPortal_Function_Library::ResizeRenderTarget(UTextureRenderTarget2D* RenderTarget, float size_x, float size_y)
{
	if (RenderTarget==nullptr)
	{
		RenderTarget->ResizeTarget(size_x, size_y);
	}
}