// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/NotifyState/HitBoxTraceNotify.h"
#include "Actor/Weapon.h"
#include "Components/BoxComponent.h"
#include "Character/Player/PlayerCharacter.h"

void UHitBoxTraceNotify::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	if (ABaseCharacter* BC = Cast<ABaseCharacter>(MeshComp->GetOwner()))
	{
		BC->GetOwnedWeapon()->GetHitBox()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}
}

void UHitBoxTraceNotify::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	if (ABaseCharacter* BC = Cast<ABaseCharacter>(MeshComp->GetOwner()))
	{
		BC->GetOwnedWeapon()->GetHitBox()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		BC->GetOwnedWeapon()->GetIgnoreActor().Empty();
	}
}
