// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CharacterDefinition.generated.h"

UCLASS()
class DIANA_ACTION_GAME_API UCharacterDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Info")
	FName								CharacterName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Info")
	class UTexture2D*					PortaitCircleImage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Info")
	class UTexture2D*					SkillImage_1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Info")
	class UTexture2D*					SkillImage_2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Info")
	class UTexture2D*					SkillImage_3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Info")
	class UTexture2D*					SkillImage_4;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Info")
	class UMaterialInterface*			BaseSkin;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Info")
	TArray<class UMaterialInterface*>	SkinArray;
};