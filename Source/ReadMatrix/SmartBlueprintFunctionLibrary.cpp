// Fill out your copyright notice in the Description page of Project Settings.


#include "SmartBlueprintFunctionLibrary.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"

UTexture2D* USmartBlueprintFunctionLibrary::GetLocalTexture(const FString &_TexPath)
{
	UTexture2D* OutTex = NULL;
	IImageWrapperModule& imageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
	TSharedPtr<IImageWrapper> imageWrapper;
	FString FileExtension = FPaths::GetExtension(_TexPath);
	EPixelFormat PixelFormat = EPixelFormat::PF_R8G8B8A8;
	if (FileExtension.Equals("JPG", ESearchCase::IgnoreCase) || FileExtension.Equals("JPEG", ESearchCase::IgnoreCase))
		imageWrapper = imageWrapperModule.CreateImageWrapper(EImageFormat::JPEG);
	else if (FileExtension.Equals("PNG", ESearchCase::IgnoreCase))
		imageWrapper = imageWrapperModule.CreateImageWrapper(EImageFormat::PNG);
	else
		imageWrapper = imageWrapperModule.CreateImageWrapper(EImageFormat::BMP);
	TArray<uint8> OutArray;
	if (FFileHelper::LoadFileToArray(OutArray, *_TexPath))
	{
		if (imageWrapper.IsValid() &&
			imageWrapper->SetCompressed(OutArray.GetData(), OutArray.Num()))
		{
			TArray<uint8> uncompressedRGBA;
			if (imageWrapper->GetRaw(ERGBFormat::RGBA, 8, uncompressedRGBA))
			{
				const TArray<FColor> uncompressedFColor = uint8ToFColor(uncompressedRGBA);
				OutTex = TextureFromImage(
					imageWrapper->GetWidth(),
					imageWrapper->GetHeight(),
					uncompressedFColor,
					true, PixelFormat);
			}
		}
	}
	return OutTex;
}

UTexture2D* USmartBlueprintFunctionLibrary::GetLocalTexture2(TArray<uint8> &ProvideData, FString ImageType)
{
	UTexture2D* OutTex = NULL;
	IImageWrapperModule& imageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
	TSharedPtr<IImageWrapper> imageWrapper;
	EPixelFormat PixelFormat = EPixelFormat::PF_R8G8B8A8;
	if (ImageType.Equals("JPG", ESearchCase::IgnoreCase) || ImageType.Equals("JPEG", ESearchCase::IgnoreCase))
		imageWrapper = imageWrapperModule.CreateImageWrapper(EImageFormat::JPEG);
	else if (ImageType.Equals("PNG", ESearchCase::IgnoreCase))
	{
		//PixelFormat = EPixelFormat::PF_R8G8B8A8;    
		//This Code will couse Error color of Image while import UTheme.
		//But It must be used while package.
		imageWrapper = imageWrapperModule.CreateImageWrapper(EImageFormat::PNG);
	}
	else
		imageWrapper = imageWrapperModule.CreateImageWrapper(EImageFormat::BMP);
	if (imageWrapper.IsValid() &&
		imageWrapper->SetCompressed(ProvideData.GetData(), ProvideData.Num()))
	{
		TArray<uint8> uncompressedRGBA;
		if (imageWrapper->GetRaw(ERGBFormat::RGBA, 8, uncompressedRGBA))
		{
			const TArray<FColor> uncompressedFColor = uint8ToFColor(uncompressedRGBA);
			OutTex = TextureFromImage(
				imageWrapper->GetWidth(),
				imageWrapper->GetHeight(),
				uncompressedFColor,
				true, PixelFormat);
		}
	}
	return OutTex;
}

TArray<FColor> USmartBlueprintFunctionLibrary::GetLocalTextureToArray(const FString& _TexPath, int &Width, int &Height)
{
	UTexture2D* OutTex = NULL;
	IImageWrapperModule& imageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
	TSharedPtr<IImageWrapper> imageWrapper;
	FString FileExtension = FPaths::GetExtension(_TexPath);
	EPixelFormat PixelFormat = EPixelFormat::PF_R8G8B8A8;
	if (FileExtension.Equals("JPG", ESearchCase::IgnoreCase) || FileExtension.Equals("JPEG", ESearchCase::IgnoreCase))
		imageWrapper = imageWrapperModule.CreateImageWrapper(EImageFormat::JPEG);
	else if (FileExtension.Equals("PNG", ESearchCase::IgnoreCase))
		imageWrapper = imageWrapperModule.CreateImageWrapper(EImageFormat::PNG);
	else
		imageWrapper = imageWrapperModule.CreateImageWrapper(EImageFormat::BMP);
	TArray<uint8> OutArray;
	TArray<FColor> uncompressedFColor;
	if (FFileHelper::LoadFileToArray(OutArray, *_TexPath))
	{
		if (imageWrapper.IsValid() &&
            imageWrapper->SetCompressed(OutArray.GetData(), OutArray.Num()))
		{
			TArray<uint8> uncompressedRGBA;
			if (imageWrapper->GetRaw(ERGBFormat::RGBA, 8, uncompressedRGBA))
			{
				uncompressedFColor = uint8ToFColor(uncompressedRGBA);
			}
		}
	}
	Width = imageWrapper->GetWidth();
    Height= imageWrapper->GetHeight();
	return uncompressedFColor;
}

TArray<FColor> USmartBlueprintFunctionLibrary::uint8ToFColor(const TArray<uint8> origin)
{
	TArray<FColor> uncompressedFColor;
	uint8 auxOrigin;
	FColor auxDst;

	for (int i = 0; i < origin.Num(); i++)
	{
		auxOrigin = origin[i];
		auxDst.R = auxOrigin;
		i++;
		auxOrigin = origin[i];
		auxDst.G = auxOrigin;
		i++;
		auxOrigin = origin[i];
		auxDst.B = auxOrigin;
		i++;
		auxOrigin = origin[i];
		auxDst.A = auxOrigin;
		uncompressedFColor.Add(auxDst);
	}

	return uncompressedFColor;
}

UTexture2D* USmartBlueprintFunctionLibrary::TextureFromImage(const int32 SrcWidth, const int32 SrcHeight, const TArray<FColor>&SrcData, const bool UseAlpha, EPixelFormat PixelFormat)
{
	UTexture2D* MyScreenshot = UTexture2D::CreateTransient(SrcWidth, SrcHeight, PixelFormat);
	uint8* MipData = static_cast<uint8*>(MyScreenshot->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE));
	uint8* DestPtr = NULL;
	const FColor* SrcPtr = NULL;
	for (int32 y = 0; y < SrcHeight; y++)
	{
		DestPtr = &MipData[(SrcHeight - 1 - y)*SrcWidth * sizeof(FColor)];
		SrcPtr = const_cast<FColor*>(&SrcData[(SrcHeight - 1 - y)*SrcWidth]);
		for (int32 x = 0; x < SrcWidth; x++)
		{
			*DestPtr++ = SrcPtr->R;
			*DestPtr++ = SrcPtr->G;
			*DestPtr++ = SrcPtr->B;
			if (UseAlpha)
			{
				*DestPtr++ = SrcPtr->A;
			}
			else
			{
				*DestPtr++ = 0xFF;
			}

			SrcPtr++;
		}
	}

	MyScreenshot->PlatformData->Mips[0].BulkData.Unlock();
	MyScreenshot->UpdateResource();
	return MyScreenshot;
}