// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/CoinActor.h"
#include "Character/Player/PlayerCharacter.h"
#include "UI/Widget/HUDWidget.h"
#include <NiagaraFunctionLibrary.h>

void ACoinActor::BeginPlay()
{
	Super::BeginPlay();

}

void ACoinActor::SphereCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("SphereCollisionBeginOverlap"));

	if (APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor))
	{
		Player->SetCoinsAmount(Coins);
		Player->GetHUDWidget()->UpdateCoinsCount(Player->GetCoinsAmount());
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), VFX_PickUp, GetActorLocation());
		PlaySound(true, GetWorld(), SFX_PickUp);
		Destroy();
	}
}
