// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/AttributeComponent.h"

UAttributeComponent::UAttributeComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}

void UAttributeComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void UAttributeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UAttributeComponent::LoseHealth(float DamageAmount)
{
	Health = FMath::Clamp(Health - DamageAmount, 0.f, MaxHealth);
}

void UAttributeComponent::AddHealth(float ValueAmount)
{
	Health = FMath::Clamp(Health + ValueAmount, 0.f, MaxHealth);
}

void UAttributeComponent::UseStamina(float Value)
{
	Stamina = FMath::Clamp(Stamina - Value, 0.f, MaxStamina);
}

void UAttributeComponent::RecoverStamina(float Value)
{
	Stamina = FMath::Clamp(Stamina + Value, 0.f, MaxStamina);
}
