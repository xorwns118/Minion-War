// Fill out your copyright notice in the Description page of Project Settings.


#include "InputContainer.h"

#include "EnhancedInputComponent.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "EnhancedActionKeyMapping.h"
#include "EnhancedInputSubsystems.h"

void UInputContainer::Init(AController* _Controller)
{
	InputActions.Empty();

	for (const auto& IMC : Contexts)
	{
		AddInputAction(IMC);
	}

	APlayerController* PC = Cast<APlayerController>(_Controller);

	if (PC)
	{
		ULocalPlayer* LocalPlayer = PC->GetLocalPlayer();

		UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();

		Subsystem->ClearAllMappings();

		for (const auto& IMC : Contexts)
		{
			Subsystem->AddMappingContext(IMC, 0);
		}
	}
}

void UInputContainer::AddInputAction(UInputMappingContext* _IMC)
{
	if (_IMC == nullptr)
		return;

	for (const FEnhancedActionKeyMapping& Mapping : _IMC->GetMappings())
	{
		if (Mapping.Action == nullptr)
			continue;

		FString ActionName = Mapping.Action->GetName();

		if (!InputActions.Contains(ActionName))
			InputActions.Add(ActionName, Mapping.Action);
	}
}

const UInputAction* UInputContainer::FindIAByName(const FString& _Name)
{
	const UInputAction** Action = InputActions.Find(_Name);

	if (Action != nullptr)
		return *Action;

	return nullptr;
}