// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/BaseCharacter.h"
#include "AITypes.h"
#include "CharacterState.h"
#include "EnemyCharacter.generated.h"

class UHealthBarComponent;
class UPawnSensingComponent;
class AAIController;
class UAnimMontage;
class UMotionWarpingComponent;

/**
 * 
 */
UCLASS()
class DEMO_1_API AEnemyCharacter : public ABaseCharacter
{
	GENERATED_BODY()
public:
	AEnemyCharacter();
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EEnemyState EnemyState;

protected:
	virtual void BeginPlay() override;

private:
	#pragma region HitReation_Animation
	virtual void GetHit_Implementation(const FVector& ImpactPoint) override;

	#pragma endregion

	#pragma region HitReation_Damage
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	virtual void GetDamage(float DamageAmount) override;
	#pragma endregion

	virtual void Die() override;

	#pragma region AI
	FAIMoveRequest MoveToTarget(AActor* Pawn);
	void Patroling();

	UFUNCTION()
	void SeePawn(APawn* Pawn);
	TObjectPtr<APawn> SeeThePawn;
	void FollowTarget();

	void Attack();
	bool TargetInRange(const AActor* SearchActor, float SearchRadius);
	void LoseTarget();

	UPROPERTY(VisibleDefaultsOnly)
	TObjectPtr<UHealthBarComponent> HealthBar;

	UPROPERTY(VisibleAnywhere, Category = "AI")
	TObjectPtr<AAIController> AIController;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "AI", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UPawnSensingComponent> PawnSensing;

	/* 
	* The actual distance between the player and enemy is PlayerCapsuleRaduis + EnemyCapsuleRaduis + DistanceToTarget
	* 34+34+50 = 118
	*/
	UPROPERTY(EditAnywhere, Category = "AI|Follow")
	float DistanceToTarget = 50.f;

	UPROPERTY(EditAnywhere, Category = "AI|Follow")
	float FollowTime = 5.f;

	FTimerHandle FollowTimer;

	// ∑¿÷πtick∑ËøÒ…Ë÷√∂® ±∆˜
	bool bFollowTimerRunOnece = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI|Attack", meta = (AllowPrivateAccess = "true"))
	float AttackRange = 130.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI|Attack", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<APawn> AttackTarget;

	UPROPERTY(EditAnywhere, Category = "AI|Attack")
	TObjectPtr<UAnimMontage> AttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI|Patrol", meta = (AllowPrivateAccess = "true"))
	float AlertRange = 1000.f;

	UPROPERTY(EditAnywhere, Category = "AI|Patrol")
	TArray<AActor*> PatrolTargets;

	int32 LastPatrolPoint = 0;

	FTimerHandle PatrolTimer;

	UPROPERTY(EditAnywhere, Category = "AI|Patrol")
	float StayAtPatrolPointTime = 3.f;

	FAIMoveRequest ReachedMoveRequest;
#pragma endregion

	void EquipWeapon();
	void UnEquipedWeapon();

	UPROPERTY(EditAnywhere, Category = "Attack")
	TSubclassOf<AWeapon> Weapon;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "AI|Attack", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UMotionWarpingComponent> MotionWarping;

	UPROPERTY(EditAnywhere)
	bool CanMove = false;
};
