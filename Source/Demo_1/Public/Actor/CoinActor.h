// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actor/BaseActor.h"
#include "CoinActor.generated.h"

/**
 * 
 */
UCLASS()
class DEMO_1_API ACoinActor : public ABaseActor
{
	GENERATED_BODY()
public:


protected:
	virtual void BeginPlay() override;

	virtual void SphereCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;


private:
	UPROPERTY(EditAnywhere)
	int32 Coins = 10;

};
