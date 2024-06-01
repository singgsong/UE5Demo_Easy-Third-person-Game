#pragma once

UENUM(BlueprintType)
enum class EActionState : uint8
{
	EAS_UnCombo UMETA(DisplayName = "UnCombo"),
	EAS_ComboAttack1 UMETA(DisplayName = "ComboAttack1"),
	EAS_ComboAttack2 UMETA(DisplayName = "ComboAttack2"),
	EAS_ComboAttack3 UMETA(DisplayName = "ComboAttack3"),

	EAS_count
};

UENUM(BlueprintType)
enum class EDeathPose : uint8
{
	EDP_Front UMETA(DisplayName = "Front"),
	EDP_Back UMETA(DisplayName = "Back"),
	EDP_Left UMETA(DisplayName = "Left"),
	EDP_Right UMETA(DisplayName = "Right"),
};

UENUM(BlueprintType)
enum class EEnemyState : uint8
{
	EES_Patroling UMETA(DisplayName = "Patroling"),
	EES_Following UMETA(DisplayName = "Following"),
	EES_Equipped UMETA(DisplayName = "Euqipped"),
	EES_UnEquipping UMETA(DisplayName = "Euqipping"),
	EES_UnEquipped UMETA(DisplayName = "UnEuqipped"),
	EES_Alerting UMETA(DisplayName = "Alerting"),
	EES_Attacking UMETA(DisplayName = "Attacking"),
	EES_AttackEnd UMETA(DisplayName = "AttackEnd"),
	EES_Died UMETA(DisplayName = "Died"),
};

