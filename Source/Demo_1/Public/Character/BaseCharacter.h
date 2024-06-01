// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interface/HitInterface.h"
#include "Demo_1/Public/CharacterState.h"
#include "BaseCharacter.generated.h"

class UAttributeComponent;
class AWeapon;

UCLASS()
class DEMO_1_API ABaseCharacter : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

public:
	ABaseCharacter();
	virtual void Tick(float DeltaTime) override;

	FORCEINLINE bool GetIsEquipped() { return bEquippedWeapon; }
	FORCEINLINE AWeapon* GetOwnedWeapon() { return OwnedWeapon; }

	FORCEINLINE UAttributeComponent* GetAttribute() { return Attribute; }

protected:
	virtual void BeginPlay() override;

	virtual void PlayMontage(class UAnimMontage* AnimMontage, const FName& SelectionName = FName("Default"));

	virtual void GetHit_Implementation(const FVector& ImpactPoint) override;
	// 根据受击方向播放受击蒙太奇
	FName GetHitDirection(const FVector& ImpactPoint);
	virtual void GetDamage(float DamageAmount);

	virtual void Die();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FX")
	TObjectPtr<USoundBase> SFX_Hit;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FX")
	TObjectPtr<UParticleSystem> VFX_Hit;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attribute")
	TObjectPtr<UAttributeComponent> Attribute;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HitReation", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAnimMontage> HitReactMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FX")
	TObjectPtr<UAnimMontage> DieMontage;

	UPROPERTY(BlueprintReadOnly, Category = "Fight")
	bool bIsDead = false;

	UPROPERTY(BlueprintReadOnly)
	//TEnumAsByte<EDeathPose> DeathPose = EDeathPose::EDP_Back;
	EDeathPose DeathPose = EDeathPose::EDP_Back;

	UPROPERTY(EditAnywhere, Category = "Fight")
	float BodyLifeTime = 5.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attack", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<AWeapon> OwnedWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attack", meta = (AllowPrivateAccess = "true"))
	bool bEquippedWeapon = false;

	UPROPERTY(EditAnywhere, Category = "Attack")
	TObjectPtr<UAnimMontage> EquipWeaponMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Locomotion", meta = (AllowPrivateAccess = "true"))
	float WalkSpeed = 300.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Locomotion", meta = (AllowPrivateAccess = "true"))
	float EquippedWalkSpeed = 250.f;

private:

};
