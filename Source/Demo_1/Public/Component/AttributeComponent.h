// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttributeComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DEMO_1_API UAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UAttributeComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void LoseHealth(float DamageAmount);
	void AddHealth(float ValueAmount);

	void UseStamina(float Value);
	void RecoverStamina(float Value);

	FORCEINLINE float GetHealthPercent() { return Health / MaxHealth; }
	FORCEINLINE float GetStaminaPercent() { return Stamina / MaxStamina; }

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, Category = "Attribute")
	float MaxHealth = 100.f;

	UPROPERTY(EditAnywhere, Category = "Attribute")
	float Health = 100.f;

	UPROPERTY(EditAnywhere, Category = "Attribute")
	float MaxStamina = 100.f;

	UPROPERTY(EditAnywhere, Category = "Attribute")
	float Stamina = 100.f;

};
