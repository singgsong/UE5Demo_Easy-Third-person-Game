// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseActor.h"
#include "Weapon.generated.h"

class UBoxComponent;
class USceneComponent;

DECLARE_DELEGATE_FourParams(FEquipWeaponMontageDelegate, USceneComponent*, const FName&, AActor*, APawn*);

UCLASS()
class DEMO_1_API AWeapon : public ABaseActor
{
	GENERATED_BODY()
	
public:	
	AWeapon();
	virtual void Tick(float DeltaTime) override;

	void PickedUp();

	UFUNCTION()
	void BeEquippedOn(USceneComponent* InParent, const FName& InSocketName, AActor* NewOwner, APawn* InInstigator);

	FORCEINLINE UBoxComponent* GetHitBox() { return HitBox; }
	FORCEINLINE TArray<AActor*> GetIgnoreActor() { return IgnoreActor; }

	FEquipWeaponMontageDelegate OnEquipWeaponMontage;

protected:
	virtual void BeginPlay() override;

	virtual void SphereCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	virtual void SphereCollisionEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

private:
	UFUNCTION()
	void HitBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void ScanActor(FHitResult& OutHit);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attack|Collision", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBoxComponent> HitBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attack|Collision", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> BoxTraceStartPoint;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attack|Collision", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> BoxTraceEndPoint;

	TArray<AActor*> IgnoreActor;

	UPROPERTY(EditAnywhere, Category = "Attack|Damage", meta = (AllowPrivateAccess = "true"))
	float Damage = 10.f;

	UPROPERTY(EditAnywhere, Category = "Attack|FX", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USoundBase> SFX_Equip;

};
