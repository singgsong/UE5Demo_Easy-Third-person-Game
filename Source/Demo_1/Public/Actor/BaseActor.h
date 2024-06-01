// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/TimelineComponent.h"
#include "BaseActor.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class UNiagaraComponent;
class UNiagaraSystem;

UCLASS()
class DEMO_1_API ABaseActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ABaseActor();
	virtual void Tick(float DeltaTime) override;

	void PlaySound(bool TrueIs2D_FalseIs3D, const UObject* WorldContextObject, USoundBase* Sound, FVector Location = FVector::ZeroVector);

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void SphereCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void SphereCollisionEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USphereComponent> SphereCollision;

	#pragma region FloatUpAndDown
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Timeline")
	TObjectPtr<UTimelineComponent> FloatUpAndDown;

	FOnTimelineFloat WeaponTimelineFloat;

	UPROPERTY(EditAnywhere, Category = "Timeline")
	TObjectPtr<UCurveFloat> WeaponTimelineCurve;
	#pragma endregion

	#pragma region FX
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UNiagaraComponent> VFX_Sign;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FX")
	TObjectPtr<UNiagaraSystem> VFX_PickUp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FX")
	TObjectPtr<USoundBase> SFX_PickUp;
	#pragma endregion

private:
	UFUNCTION()
	void WeaponFloatUpAndDown(float Value);
};
