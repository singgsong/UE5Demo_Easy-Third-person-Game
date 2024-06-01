// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HealthBarComponent.h"
#include "UI/Widget/HealthBarWidget.h"
#include "Components/ProgressBar.h"

void UHealthBarComponent::SetHealthPercent(float Percent)
{
    if (HealthBarWidget == nullptr)
    {
        HealthBarWidget = Cast<UHealthBarWidget>(GetUserWidgetObject());
    }
    if (HealthBarWidget && HealthBarWidget->HealthBar)
    {
        HealthBarWidget->HealthBar->SetPercent(Percent);
    }
}
