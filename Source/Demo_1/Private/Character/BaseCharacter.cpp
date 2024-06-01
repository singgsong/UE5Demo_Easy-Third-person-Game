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
	* �õ�������cosֵ��Ȼ���÷����Ҽ�����Ƕȡ�
	* FMath::Acos �������ص��ǻ��ȣ���Ҫ�� FMath::RadiansToDegrees ת��Ϊ�Ƕ�
	* �����ʽ��Forward * ToHit = cos(theta)
	* �����ҹ�ʽ��acos(cos(theta)) = theta
	*/
	const double CosTheta = FVector::DotProduct(Forward, ToHit);
	double Theta = FMath::Acos(CosTheta);
	Theta = FMath::RadiansToDegrees(Theta);

	/**
	* �ò���ó���ֱ��Forward��ToHit������CrossPruduct������CrossPruduct�ж�Forward��ToHit�ļн��������ޣ��Ӷ��ж��ܻ�����
	* ��������ֶ����� UE ��Ҫ�������֣�
	* Forward���䣬CrossPruduct�������£���ToHit�������Է�����Ĺ���
	* �����ʽ��Forward * ToHit = CrossPruduct
	*/
	const FVector CrossPruduct = FVector::CrossProduct(Forward, ToHit);
	if (CrossPruduct.Z < 0) Theta *= -1.f;

	/* �ж��ܻ����� */
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
