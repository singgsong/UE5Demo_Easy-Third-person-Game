// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/Notify/EquipWeapon.h"
#include "Character/BaseCharacter.h"
#include "Actor/Weapon.h"

void UEquipWeapon::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	if (ABaseCharacter* BC = Cast<ABaseCharacter>(MeshComp->GetOwner()))
	{
		if (BC->GetIsEquipped())
		{
			BC->GetOwnedWeapon()->OnEquipWeaponMontage.ExecuteIfBound(MeshComp, FName("rHandSocket"), BC, BC);
		}
		else
		{
			BC->GetOwnedWeapon()->OnEquipWeaponMontage.ExecuteIfBound(MeshComp, FName("BackSocket"), BC, BC);
		}
	}
}
