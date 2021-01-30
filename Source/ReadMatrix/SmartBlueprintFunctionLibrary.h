// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SmartBlueprintFunctionLibrary.generated.h"

class UTexture2D;
/**
 * 
 */
UCLASS()
class READMATRIX_API USmartBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	static UTexture2D* TextureFromImage(const int32 SrcWidth, const int32 SrcHeight, const TArray<FColor>&SrcData, const bool UseAlpha, EPixelFormat PixelFormat);

	UFUNCTION(BlueprintCallable)
	static TArray<FColor> uint8ToFColor(const TArray<uint8> origin);

	UFUNCTION(BlueprintCallable)
	static UTexture2D* GetLocalTexture(const FString &_TexPath);

	UFUNCTION(BlueprintCallable)
	static UTexture2D* GetLocalTexture2(TArray<uint8> &ProvideData,FString ImageType);

	UFUNCTION(BlueprintCallable)
	static TArray<FColor> GetLocalTextureToArray(const FString &_TexPath, int &Width, int &Height);
	
};
