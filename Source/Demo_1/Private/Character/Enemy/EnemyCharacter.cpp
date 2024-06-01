// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Enemy/EnemyCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Component/AttributeComponent.h"
#include "UI/HealthBarComponent.h"
#include "AIController.h"
#include "Perception/PawnSensingComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "MotionWarpingComponent.h" 
#include "GameFramework/CharacterMovementComponent.h"
#include "Actor/Weapon.h"


AEnemyCharacter::AEnemyCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	// Damage
	HealthBar = CreateDefaultSubobject<UHealthBarComponent>(FName("HealthBar"));
	HealthBar->SetupAttachment(GetRootComponent());

	// AI
	PawnSensing = CreateDefaultSubobject<UPawnSensingComponent>(FName("PawnSensing"));
	MotionWarping = CreateDefaultSubobject<UMotionWarpingComponent>(FName("MotionWarping"));
}

void AEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	#pragma region StayAtPatrolPoint
	if (AIController->GetPathFollowingComponent()->HasReached(ReachedMoveRequest)) // 到达巡逻点等待PatrolTime秒，在前往下个巡逻点
	{
		ReachedMoveRequest = nullptr; // 清除当前目标点，准备下一次的检查
		GetWorldTimerManager().SetTimer(PatrolTimer, this, &AEnemyCharacter::Patroling, StayAtPatrolPointTime);
	}
	#pragma endregion

	#pragma region TargetHasDied
	if (AttackTarget && AttackTarget->ActorHasTag(FName("Died")))
	{
		AttackTarget = nullptr;
		GetWorldTimerManager().SetTimer(PatrolTimer, this, &AEnemyCharacter::Patroling, 1.f);
	}
	#pragma endregion

	//GEngine->AddOnScreenDebugMessage(2, 1.f, FColor::MakeRandomColor(), FString::Printf(TEXT("FollowTimer %f"), GetWorldTimerManager().GetTimerRemaining(FollowTimer)));
	#pragma region LoseTarget
	// 丢失目标
	if (!TargetInRange(AttackTarget, AlertRange) && bEquippedWeapon && !PawnSensing->CouldSeePawn(AttackTarget))
	{
		if (!bFollowTimerRunOnece)
		{
			bFollowTimerRunOnece = true;
			GetWorldTimerManager().SetTimer(FollowTimer, this, &AEnemyCharacter::LoseTarget, FollowTime);
		}
	}
	//GEngine->AddOnScreenDebugMessage(1, 1.f, FColor::MakeRandomColor(), FString::Printf(TEXT("CouldSeePawn %d"), PawnSensing->CouldSeePawn(AttackTarget)));
	#pragma endregion

	#pragma region BeSneaked
	if (AttackTarget && !PawnSensing->CouldSeePawn(AttackTarget)) // 被偷袭
	{
		MoveToTarget(AttackTarget);
	}
	#pragma endregion

	#pragma region Ai
	if (TargetInRange(AttackTarget, AlertRange)) // 目标进入侦擦范围
	{
		//FollowTarget();

		if (!bEquippedWeapon)
		{
			EquipWeapon();
		}
		else if (TargetInRange(AttackTarget, AttackRange))
		{
			EnemyState = EEnemyState::EES_Attacking;
			Attack();
		}

		if (GetWorldTimerManager().IsTimerActive(FollowTimer))
		{
			bFollowTimerRunOnece = false;
			GetWorldTimerManager().ClearTimer(FollowTimer);
		}
	}
	#pragma endregion
}

void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	HealthBar->SetVisibility(false);
	HealthBar->SetHealthPercent(Attribute->GetHealthPercent());

	#pragma region SpawnWeapon
	// 在背上生成武器
	if (Weapon)
	{
		OwnedWeapon = GetWorld()->SpawnActor<AWeapon>(Weapon);
		OwnedWeapon->BeEquippedOn(GetMesh(), FName("BackSocket"), this, this);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::MakeRandomColor(), FString::Printf(TEXT("先选择要生成的武器！不然没法生成！")));
	}
	#pragma endregion

	#pragma region AI
	PawnSensing->OnSeePawn.AddDynamic(this, &AEnemyCharacter::SeePawn);
	PawnSensing->SetSensingInterval(0.001f);
	PawnSensing->SightRadius = AlertRange;
	PawnSensing->SetPeripheralVisionAngle(70.f);

	AIController = Cast<AAIController>(GetController());
	Patroling();

	Tags.Add(FName("Enemy"));
	#pragma endregion
}

void AEnemyCharacter::GetHit_Implementation(const FVector& ImpactPoint)
{
	Super::GetHit_Implementation(ImpactPoint);
}

#pragma region Damage
float AEnemyCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	GetDamage(DamageAmount);
	if (!EventInstigator->GetPawn()->ActorHasTag(FName("Enemy")))
	{
		AttackTarget = EventInstigator->GetPawn();
	}
	return DamageAmount;
}

void AEnemyCharacter::GetDamage(float DamageAmount)
{
	Super::GetDamage(DamageAmount);

	HealthBar->SetVisibility(true);
	HealthBar->SetHealthPercent(Attribute->GetHealthPercent());
}
#pragma endregion

#pragma region AI
FAIMoveRequest AEnemyCharacter::MoveToTarget(AActor* Pawn)
{
	if (!CanMove) return NULL;
	if (!AIController || !Pawn) return NULL;
	if (bIsDead) return NULL;

	FAIMoveRequest MoveRequest;
	MoveRequest.SetGoalActor(Pawn);
	MoveRequest.SetAcceptanceRadius(DistanceToTarget);
	AIController->MoveTo(MoveRequest);
	return MoveRequest;
}

void AEnemyCharacter::Patroling()
{
	EnemyState = EEnemyState::EES_Patroling;
	int32 RandomPatrolPoint = 0;
	if (!PatrolTargets.Num()) return;
	if (PatrolTargets.Num() < 2)
	{
		RandomPatrolPoint = FMath::RandRange(0, PatrolTargets.Num() - 1);
	}
	else
	{
		while (RandomPatrolPoint == LastPatrolPoint)//防止此次和上一次的巡逻点相同
		{
			RandomPatrolPoint = FMath::RandRange(0, PatrolTargets.Num() - 1);
			//if (RandomPatrolPoint != LastPatrolPoint) break;
		}
	}
	LastPatrolPoint = RandomPatrolPoint;
	ReachedMoveRequest = MoveToTarget(PatrolTargets[RandomPatrolPoint]);

}

void AEnemyCharacter::SeePawn(APawn* Pawn)
{
	AttackTarget = Pawn;
	MoveToTarget(Pawn);
}

void AEnemyCharacter::FollowTarget()
{
	EnemyState = EEnemyState::EES_Following;
	GetWorldTimerManager().ClearTimer(PatrolTimer);
	MoveToTarget(AttackTarget);
}

bool AEnemyCharacter::TargetInRange(const AActor* SearchActor, float SearchRadius)
{
	if (!SearchActor) return false;

	const float Distance = (SearchActor->GetActorLocation() - GetActorLocation()).Size();
	return Distance <= SearchRadius;
}

void AEnemyCharacter::Attack()
{
	if (bIsDead) return;

	// MotionWarpping
	double DistanceBetweenTargets = (GetActorLocation() - AttackTarget->GetActorLocation()).Size();
	FVector HitVector = GetActorLocation() + GetActorForwardVector() * (DistanceBetweenTargets - 118);
	MotionWarping->AddOrUpdateWarpTargetFromLocation(FName("Location"), HitVector);
	MotionWarping->AddOrUpdateWarpTargetFromLocation(FName("Rotation"), AttackTarget->GetActorLocation());

	PlayMontage(AttackMontage);
}

void AEnemyCharacter::LoseTarget()
{
	AttackTarget = nullptr;
	bFollowTimerRunOnece = false;
	GetMovementComponent()->StopMovementImmediately();
	HealthBar->SetVisibility(false);
	UnEquipedWeapon();
	// 播放Must be wind音效
	GetWorldTimerManager().SetTimer(PatrolTimer, this, &AEnemyCharacter::Patroling, 1.f);
}

void AEnemyCharacter::EquipWeapon()
{
	if (!OwnedWeapon) return;
	EnemyState = EEnemyState::EES_Equipped;
	bEquippedWeapon = true;
	GetCharacterMovement()->MaxWalkSpeed = EquippedWalkSpeed;
	PlayAnimMontage(EquipWeaponMontage, 1.f, FName("Equip"));
}

void AEnemyCharacter::UnEquipedWeapon()
{
	if (!OwnedWeapon) return;
	EnemyState = EEnemyState::EES_UnEquipped;
	bEquippedWeapon = false;
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	PlayAnimMontage(EquipWeaponMontage, 1.f, FName("Sheathe"));
}
#pragma endregion

void AEnemyCharacter::Die()
{
	Super::Die();
	HealthBar->SetVisibility(false);
}
