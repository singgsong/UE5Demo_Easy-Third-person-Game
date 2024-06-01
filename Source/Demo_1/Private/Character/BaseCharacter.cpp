// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BaseCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Component/AttributeComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include <Actor/Weapon.h>

// Sets default values
ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = false;
	
	// AttackCollision
	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetGenerateOverlapEvents(true);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);

	Attribute = CreateDefaultSubobject<UAttributeComponent>(FName("Attribute"));
	
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	bIsDead = false;

}

void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABaseCharacter::PlayMontage(class UAnimMontage* AnimMontage, const FName& SelectionName)
{
	if (!GetCurrentMontage())
	{
		PlayAnimMontage(AnimMontage, 1.f, SelectionName);
	}
}

void ABaseCharacter::GetHit_Implementation(const FVector& ImpactPoint)
{
	if (SFX_Hit) UGameplayStatics::PlaySound2D(GetWorld(), SFX_Hit);
	if (VFX_Hit) UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), VFX_Hit, ImpactPoint);

	FName Selection = GetHitDirection(ImpactPoint);
	if (Attribute->GetHealthPercent() == 0)
	{
		StopAnimMontage(GetCurrentMontage());
		PlayMontage(DieMontage, Selection);
		Die();
	}
	else
	{
		StopAnimMontage(GetCurrentMontage());
		PlayMontage(HitReactMontage, Selection);
	}
}

FName ABaseCharacter::GetHitDirection(const FVector& ImpactPoint)
{
	const FVector Forward = GetActorForwardVector();
	const FVector ImpactLowered(ImpactPoint.X, ImpactPoint.Y, GetActorLocation().Z);
	const FVector ToHit = (ImpactLowered - GetActorLocation()).GetSafeNormal();

	/**
	* 用点积计算出cos值。然后用反余弦计算出角度。
	* FMath::Acos 函数返回的是弧度，还要用 FMath::RadiansToDegrees 转换为角度
	* 点积公式：Forward * ToHit = cos(theta)
	* 反余弦公式：acos(cos(theta)) = theta
	*/
	const double CosTheta = FVector::DotProduct(Forward, ToHit);
	double Theta = FMath::Acos(CosTheta);
	Theta = FMath::RadiansToDegrees(Theta);

	/**
	* 用叉积得出垂直于Forward和ToHit的向量CrossPruduct，根据CrossPruduct判断Forward和ToHit的夹角所在象限，从而判断受击方向
	* 叉积的右手定则在 UE 里要换成左手！
	* Forward不变，CrossPruduct方向向下，则ToHit就是来自反方向的攻击
	* 叉积公式：Forward * ToHit = CrossPruduct
	*/
	const FVector CrossPruduct = FVector::CrossProduct(Forward, ToHit);
	if (CrossPruduct.Z < 0) Theta *= -1.f;

	/* 判断受击方向 */
	FName Section("HitFormFront");
	DeathPose = EDeathPose::EDP_Back;

	if (Theta >= 135.f && Theta < -135.f)
	{
		Section = FName("HitFormBack");
		DeathPose = EDeathPose::EDP_Front;
	}
	else if (Theta >= -135.f && Theta < -45.f)
	{
		Section = FName("HitFromLeft");
		DeathPose = EDeathPose::EDP_Right;
	}
	else if (Theta >= 45.f && Theta < 135.f)
	{
		Section = FName("HitFromRight");
		DeathPose = EDeathPose::EDP_Left;
	}
	return Section;
}

void ABaseCharacter::GetDamage(float DamageAmount)
{
	Attribute->LoseHealth(DamageAmount);
}

void ABaseCharacter::Die()
{
	bIsDead = true;
	//GetCharacterMovement()->DisableMovement();
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	if (OwnedWeapon) OwnedWeapon->Destroy();
	SetLifeSpan(BodyLifeTime);
}
