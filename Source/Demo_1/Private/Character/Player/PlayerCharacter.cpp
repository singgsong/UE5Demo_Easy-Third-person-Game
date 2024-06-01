// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Player/PlayerCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputSubsystems.h"
#include <EnhancedInputComponent.h>
#include "InputActionValue.h"
#include "Actor/Weapon.h"
#include "Animation/AnimMontage.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Component/AttributeComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UI/HUD/DemoHUD.h"
#include "UI/Widget/HUDWidget.h"

APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(FName("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 500.f;
	SpringArm->bUsePawnControlRotation = true;

	Camera = CreateDefaultSubobject<UCameraComponent>(FName("Camera"));
	Camera->SetupAttachment(SpringArm);
	Camera->bUsePawnControlRotation = true;
	
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
	GetCharacterMovement()->bOrientRotationToMovement = true; 

}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	// SetupInputMappingContext
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
			Subsystem->AddMappingContext(DemoInputContext, 0);

	//if (APlayerController* APC = Cast<APlayerController>(GetController()))
	//{
	//	if (ADemoHUD* DemoHUD = Cast<ADemoHUD>(APC->GetHUD()))
	// 和上面两条语句的功能一样，因为是单机所以这样更方便，如果是多人用上面的更好
	if (ADemoHUD* DemoHUD = Cast<ADemoHUD>(GetWorld()->GetFirstPlayerController()->GetHUD())) 
		{
			HUDWidget = DemoHUD->GetHUDWidget(); // 初始化该Actor的HUD
			HUDWidget->UpdateHealthBar(Attribute->GetHealthPercent());
			HUDWidget->UpdateStamina(Attribute->GetStaminaPercent());
			HUDWidget->UpdateCoinsCount(CoinsAmount);
		}

	OnPickableActor.BindUObject(this, &APlayerCharacter::HasPickableActor);
	OnPickUpHealthPack.BindLambda(
		[this](float Value) {
			Attribute->AddHealth(Value);
			HUDWidget->UpdateHealthBar(Attribute->GetHealthPercent());
		});

	Tags.Add(TEXT("Player"));
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	Attribute->RecoverStamina(DeltaTime * StaminaRecoverRate);
	HUDWidget->UpdateStamina(Attribute->GetStaminaPercent());
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	if (!DemoInputContext)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2, FColor::MakeRandomColor(), FString::Printf(TEXT("DemoInputContext is nullptr")));
	}
	else
	{
		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
		{
			EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Look);

			// Locomotion
			EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Move);
			EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Started, this, &APlayerCharacter::Run);
			EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &APlayerCharacter::Jump);
			EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &APlayerCharacter::StopJumping);
			EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this, &APlayerCharacter::Crouch);

			// UnarmAttack
			EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Completed, this, &APlayerCharacter::UnCombo);
			EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &APlayerCharacter::PlayAttackMontage);
			EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Canceled, this, &APlayerCharacter::Combo);

			EnhancedInputComponent->BindAction(PickUpAction, ETriggerEvent::Started, this, &APlayerCharacter::PickUpWeapon);
			EnhancedInputComponent->BindAction(EquipAction, ETriggerEvent::Started, this, &APlayerCharacter::EquipWeapon);

			EnhancedInputComponent->BindAction(DodgeAction, ETriggerEvent::Started, this, &APlayerCharacter::Dodge);
			
		}
	}
}

#pragma region Locomotion
void APlayerCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookVector = Value.Get<FVector2D>();
	if (Controller)
	{
		AddControllerYawInput(LookVector.X);
		AddControllerPitchInput(LookVector.Y);
	}
}

void APlayerCharacter::Move(const FInputActionValue& Value)
{
	const FVector2D MovementVector = Value.Get<FVector2D>();
	if (Controller)
	{
		const FRotator Yaw(0.f, Controller->GetControlRotation().Yaw, 0.f);
		const FVector ForwardVector = FRotationMatrix(Yaw).GetUnitAxis(EAxis::X);
		const FVector RightVector = FRotationMatrix(Yaw).GetUnitAxis(EAxis::Y);

		AddMovementInput(ForwardVector, MovementVector.X);
		AddMovementInput(RightVector, MovementVector.Y);
	}
}

void APlayerCharacter::Run()
{
	if (bEquippedWeapon) return;

	if (!bRunning)
	{
		bRunning = true;
		bCrouch = false;
		ACharacter::UnCrouch();
		GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
	}
	else
	{
		bRunning = false;
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	}
}

void APlayerCharacter::Crouch()
{
	if (GetCharacterMovement()->IsFalling()) return;
	if (!bCrouch)
	{
		bCrouch = true;
		bRunning = false;
		Super::Crouch();
		GetCharacterMovement()->MaxWalkSpeed = CrouchSpeed;
	}
	else
	{
		bCrouch = false;
		Super::UnCrouch();
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	}
}

void APlayerCharacter::Jump()
{
	if (GetCurrentMontage()) return;
	ACharacter::Jump();
	// 通过跳跃键取消蹲伏
	ACharacter::UnCrouch();
	bCrouch = false;
}

void APlayerCharacter::StopJumping()
{
	ACharacter::StopJumping();
	// 掉落时可以进入奔跑状态
	if (bRunning)
	{
		GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
	}
}
#pragma endregion

#pragma region Fight
#pragma region PlayAttackMontage
void APlayerCharacter::PlayAttackMontage()
{
	if (!UnarmAttackMontage || !ArmAttackMontage) return;
	if (ComboCount > 0) return;
	bEquippedWeapon ?
		PlayMontage(ArmAttackMontage, FName("ArmAttackCombo")) :
		PlayMontage(UnarmAttackMontage, FName("UnarmAttack"));
}

void APlayerCharacter::UnCombo()
{
	bComboing = false;
	ComboCount = 0;
}

void APlayerCharacter::Combo()
{
	bComboing = true;
	ComboCount++;
	if (ComboCount == 1)
	{
		GetWorldTimerManager().SetTimer(ComboTimer, this, &APlayerCharacter::ResetComboCount, Time_CanCombo, false);
	}
	else if (ComboCount == 2)
	{
		StopAnimMontage(GetCurrentMontage());

		bEquippedWeapon ?
			PlayMontage(ArmAttackMontage, FName("ArmAttackCombo1")) :
			PlayMontage(UnarmAttackMontage, FName("UnarmAttack_1"));
		GetWorldTimerManager().SetTimer(ComboTimer, this, &APlayerCharacter::ResetComboCount, Time_CanCombo, false);
	}
	else if (ComboCount == 3)
	{
		StopAnimMontage(GetCurrentMontage());

		bEquippedWeapon ?
			PlayMontage(ArmAttackMontage, FName("ArmAttackCombo2")) :
			PlayMontage(UnarmAttackMontage, FName("UnarmAttack_2"));

		GetWorldTimerManager().SetTimer(ComboTimer, this, &APlayerCharacter::ResetComboCount, Time_CanCombo, false);
	}
}

void APlayerCharacter::ResetComboCount()
{
	ComboCount = 0;
	GetWorldTimerManager().ClearTimer(ComboTimer);
}
#pragma endregion

#pragma region PlayHitReactMontage
void APlayerCharacter::GetHit_Implementation(const FVector& ImpactPoint)
{
	Super::GetHit_Implementation(ImpactPoint);
	if (!Attribute->GetHealthPercent())
	{
		Die();
	}
}
void APlayerCharacter::GetDamage(float DamageAmount)
{
	Super::GetDamage(DamageAmount);
	HUDWidget->UpdateHealthBar(Attribute->GetHealthPercent());
}
#pragma endregion

#pragma region TakeDamage
float APlayerCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	GetDamage(DamageAmount);
	return DamageAmount;
}
#pragma endregion
#pragma endregion

#pragma region InteractWithActor
void APlayerCharacter::Die()
{
	Super::Die();
	DisableInput(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	Tags.Add(FName("Died"));
}

void APlayerCharacter::HasPickableActor(ABaseActor* Actor)
{
	PickableActor = Actor;
} 

void APlayerCharacter::PickUpWeapon()
{
	AWeapon* Weapon = Cast<AWeapon>(PickableActor);
	if (Weapon)
	{
		if (OwnedWeapon) OwnedWeapon->Destroy();
		Weapon->PickedUp();
		Weapon->BeEquippedOn(GetMesh(), FName("BackSocket"), this, this);
		PickableActor = nullptr;
		OwnedWeapon = Weapon;
	}
}

void APlayerCharacter::EquipWeapon()
{
	if (GetCurrentMontage()) return; // 防止短期内被重复调用
	if (!OwnedWeapon) return;
	if (!bEquippedWeapon)
	{
		bEquippedWeapon = true;
		GetCharacterMovement()->MaxWalkSpeed = EquippedWalkSpeed;
		PlayMontage(EquipWeaponMontage, FName("Equip"));
	}
	else
	{
		bEquippedWeapon = false;
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
		PlayMontage(EquipWeaponMontage, FName("Sheathe"));
	}

	if (!OwnedWeapon)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::MakeRandomColor(), FString::Printf(TEXT("武器都没有，你想装备空气吗？")));
	}
}
#pragma endregion

void APlayerCharacter::Dodge()
{
	if (GetCurrentMontage()) return; // 防止重复调用
	if (Attribute->GetStaminaPercent() * 100.f < DodgeCost) return;

	Attribute->UseStamina(DodgeCost);
	HUDWidget->UpdateStamina(Attribute->GetStaminaPercent());
	PlayMontage(DodgeMontage);
}

