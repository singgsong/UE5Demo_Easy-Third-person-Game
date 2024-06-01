// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "DemoHUD.generated.h"

class UHUDWidget;
/**
 * 
 */
UCLASS()
class DEMO_1_API ADemoHUD : public AHUD
{
	GENERATED_BODY()
public:
	FORCEINLINE UHUDWidget* GetHUDWidget() { return HUDWidget; }

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<UHUDWidget> HUDClass;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UHUDWidget> HUDWidget;
};
