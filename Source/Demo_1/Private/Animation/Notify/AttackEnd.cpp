// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/Notify/AttackEnd.h"
#include "Character/Enemy/EnemyCharacter.h"

void UAttackEnd::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	if (AEnemyCharacter* EC = Cast<AEnemyCharacter>(MeshComp->GetOwner()))
	{
		EC->EnemyState = EEnemyState::EES_AttackEnd;
	}
}
