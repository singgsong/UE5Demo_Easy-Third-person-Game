// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/HealthPack.h"
#include "Character/Player/PlayerCharacter.h"
#include "NiagaraFunctionLibrary.h"

AHealthPack::AHealthPack()
{
}

void AHealthPack::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AHealthPack::BeginPlay()
{
	Super::BeginPlay();

}

void AHealthPack::SphereCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (APlayerCharacter* PC = Cast<APlayerCharacter>(OtherActor))
	{
		PC->OnPickUpHealthPack.Execute(AddHealth);
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), VFX_PickUp, GetActorLocation());
		PlaySound(true, GetWorld(), SFX_PickUp);
		Destroy();
	}
}

