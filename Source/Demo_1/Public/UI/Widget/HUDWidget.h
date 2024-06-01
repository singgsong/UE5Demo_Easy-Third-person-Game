// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HUDWidget.generated.h"

class UProgressBar;
class UTextBlock;
/**
 * 
 */
UCLASS()
class DEMO_1_API UHUDWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	void UpdateHealthBar(float Percent);
	void UpdateStamina(float Percent);
	void UpdateCoinsCount(int32 Value);

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> HealthBar;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> StaminaBar;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Coins;
};
