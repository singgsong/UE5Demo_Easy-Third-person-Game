// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/BaseActor.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include <Character/Player/PlayerCharacter.h>
#include "NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"

ABaseActor::ABaseActor()
{
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject< UStaticMeshComponent>(FName("StaticMeshComp"));
	SetRootComponent(Mesh);
	// 网格体不需要碰撞，由SphereCollision处理碰撞事件
	Mesh->SetCollisionResponseToAllChannels(ECR_Ignore);

	SphereCollision = CreateDefaultSubobject< USphereComponent>(FName("SphereComp"));
	SphereCollision->SetupAttachment(GetRootComponent());

	VFX_Sign = CreateDefaultSubobject<UNiagaraComponent>(FName("NiagaraComp"));
	VFX_Sign->SetupAttachment(GetRootComponent());

	FloatUpAndDown = CreateDefaultSubobject<UTimelineComponent>(FName("TimelineComp"));
}

void ABaseActor::BeginPlay()
{
	Super::BeginPlay();
	// Actor在世界中上下浮动，可关闭，在蓝图调用开启
	WeaponTimelineFloat.BindDynamic(this, &ABaseActor::WeaponFloatUpAndDown);
	FloatUpAndDown->AddInterpFloat(WeaponTimelineCurve, WeaponTimelineFloat);
	FloatUpAndDown->SetLooping(true);
	FloatUpAndDown->Play();

	SphereCollision->OnComponentBeginOverlap.AddDynamic(this, &ABaseActor::SphereCollisionBeginOverlap);
	SphereCollision->OnComponentEndOverlap.AddDynamic(this, &ABaseActor::SphereCollisionEndOverlap);
}

void ABaseActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABaseActor::WeaponFloatUpAndDown(float Value)
{
	AddActorWorldOffset(FVector(0.f, 0.f, Value));
}

void ABaseActor::SphereCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor))
	{
		Player->OnPickableActor.ExecuteIfBound(this);
	}
}

void ABaseActor::SphereCollisionEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor))
	{
		Player->OnPickableActor.ExecuteIfBound(nullptr);
	}
}

void ABaseActor::PlaySound(bool TrueIs2D_FalseIs3D, const UObject* WorldContextObject, USoundBase* Sound, FVector Location)
{
	if (TrueIs2D_FalseIs3D)
	{
		UGameplayStatics::PlaySound2D(WorldContextObject, Sound);
	}
	else
	{
		UGameplayStatics::PlaySoundAtLocation(WorldContextObject, Sound, Location);
	}
}