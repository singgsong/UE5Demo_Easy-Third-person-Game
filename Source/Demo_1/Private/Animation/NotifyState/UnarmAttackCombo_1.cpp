// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation\NotifyState\UnarmAttackCombo_1.h"
#include "Demo_1/Public/Character/BaseCharacter.h"
#include "Demo_1/Public/CharacterState.h"
#include "GameFramework/CharacterMovementComponent.h"

void UUnarmAttackCombo_1::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	if (ABaseCharacter* BC = Cast<ABaseCharacter>(MeshComp->GetOwner()))
	{
		BC->GetCharacterMovement()->DisableMovement();
		//BC->bUseControllerRotationYaw = false;
	}
}

void UUnarmAttackCombo_1::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	if (ABaseCharacter* BC = Cast<ABaseCharacter>(MeshComp->GetOwner()))
	{
		BC->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
		//BC->bUseControllerRotationYaw = true;
	}
}

FString UUnarmAttackCombo_1::GetNotifyName_Implementation() const
{
	Super::GetNotifyName_Implementation();
	return Name;
}
