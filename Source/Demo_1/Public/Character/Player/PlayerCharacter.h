// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/BaseCharacter.h"
#include "Demo_1/Public/CharacterState.h"
#include "PlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
class ABaseActor;
class UHUDWidget;

DECLARE_DELEGATE_OneParam(FPickableActorDelegate, ABaseActor*);
DECLARE_DELEGATE_OneParam(FPickUpHealthPackDelegate, float);

UCLASS()
class DEMO_1_API APlayerCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:
	APlayerCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	FPickableActorDelegate OnPickableActor;
	FPickUpHealthPackDelegate OnPickUpHealthPack;

protected:
	virtual void BeginPlay() override;

private:
	#pragma region Locomotion
	void Look(const FInputActionValue& Value);
	void Move(const FInputActionValue& Value);
	void Run();
	void Crouch();
	void Jump();
	void StopJumping();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> Camera;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputMappingContext> DemoInputContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> RunAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> CrouchAction;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Locomotion", meta = (AllowPrivateAccess = "true"))
	bool bRunning = false;

	bool bCrouch = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Locomotion", meta = (AllowPrivateAccess = "true"))
	float RunSpeed = 1000.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Locomotion", meta = (AllowPrivateAccess = "true"))
	float CrouchSpeed = 200.f;
	#pragma endregion

	#pragma region Fight
	void PlayAttackMontage();
	void UnCombo();
	void Combo();
	void ResetComboCount();

	virtual void GetHit_Implementation(const FVector& ImpactPoint) override;
	virtual void GetDamage(float DamageAmount) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;


	UPROPERTY(EditAnywhere, Category = "Attack")
	TObjectPtr<UInputAction> AttackAction;

	UPROPERTY(EditAnywhere, Category = "Attack")
	TObjectPtr<UAnimMontage> UnarmAttackMontage;
	
	bool bComboing = false;

	int32 ComboCount = 0;

	FTimerHandle ComboTimer;

	UPROPERTY(EditAnywhere, Category = "Attack")
	float Time_CanCombo = .6f;

	virtual void Die() override;

	#pragma endregion

	#pragma region InteractWithActor
	void HasPickableActor(ABaseActor* Actor);	
	void PickUpWeapon();
	void EquipWeapon() ;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> PickUpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> EquipAction;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<ABaseActor> PickableActor;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAnimMontage> ArmAttackMontage;
	#pragma endregion

	void Dodge();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> DodgeAction;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UAnimMontage> DodgeMontage;

	UPROPERTY(EditAnywhere)
	float DodgeCost = 20.f;

	UPROPERTY(EditAnywhere)
	float StaminaRecoverRate = 5.f;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UHUDWidget> HUDWidget;

	int32 CoinsAmount = 0;
public:
	FORCEINLINE bool GetIsDead() { return bIsDead; }
	FORCEINLINE UHUDWidget* GetHUDWidget() { return HUDWidget; }
	FORCEINLINE int32 GetCoinsAmount() { return CoinsAmount; }
	FORCEINLINE void SetCoinsAmount(int32 Value) { CoinsAmount += Value; }
};
