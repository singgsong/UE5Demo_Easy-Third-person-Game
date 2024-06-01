// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/DemoHUD.h"
#include "UI/Widget/HUDWidget.h"

void ADemoHUD::BeginPlay()
{
	Super::BeginPlay();
	if (HUDClass)
	{
		HUDWidget = CreateWidget<UHUDWidget>(GetWorld()->GetFirstPlayerController(), HUDClass);
		HUDWidget->AddToViewport();
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::MakeRandomColor(), FString::Printf(TEXT("HUDClass未设置 !")));
	}
}
