// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "InputContainer.generated.h"

UCLASS()
class DIANA_ACTION_GAME_API UInputContainer : public UObject
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	TArray<class UInputMappingContext*> Contexts;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	TMap<FString, const class UInputAction*> InputActions;

public:
	void Clear() { Contexts.Empty(); InputActions.Empty(); }

	void Init(AController* _Controller);

	const UInputAction* FindIAByName(const FString& _Name);

	void AddInputAction(UInputMappingContext* _IMC);
};
