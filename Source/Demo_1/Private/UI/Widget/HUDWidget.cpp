// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/HUDWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UHUDWidget::UpdateHealthBar(float Percent)
{
	if (HealthBar)
		HealthBar->SetPercent(Percent);
}

void UHUDWidget::UpdateStamina(float Percent)
{
	if (StaminaBar)
		StaminaBar->SetPercent(Percent);
}

void UHUDWidget::UpdateCoinsCount(int32 Value)
{
	if (Coins)
		Coins->SetText(FText::FromString(FString::Printf(TEXT("%d"), Value)));
}
