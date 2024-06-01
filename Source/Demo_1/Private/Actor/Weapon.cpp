#include "Actor/Weapon.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Interface/HitInterface.h"

AWeapon::AWeapon()
{
	PrimaryActorTick.bCanEverTick = false;

	HitBox = CreateDefaultSubobject<UBoxComponent>(FName("HitBox"));
	HitBox->SetupAttachment(RootComponent);

	// HitDamage
	HitBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	HitBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	HitBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	HitBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	BoxTraceStartPoint = CreateDefaultSubobject<USceneComponent>(FName("BoxTraceStartPoint"));
	BoxTraceStartPoint->SetupAttachment(HitBox);

	BoxTraceEndPoint = CreateDefaultSubobject<USceneComponent>(FName("BoxTraceEndPoint"));
	BoxTraceEndPoint->SetupAttachment(HitBox);
}

void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	OnEquipWeaponMontage.BindUObject(this, &AWeapon::BeEquippedOn);
	HitBox->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::HitBoxBeginOverlap);
}

void AWeapon::SphereCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::SphereCollisionBeginOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}

void AWeapon::SphereCollisionEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::SphereCollisionEndOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
}
 
void AWeapon::PickedUp()
{
	SphereCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AWeapon::BeEquippedOn(USceneComponent* InParent, const FName& InSocketName, AActor* NewOwner, APawn* InInstigator)
{
	FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
	Mesh->AttachToComponent(InParent, AttachmentRules, InSocketName);
	PlaySound(true, this, SFX_Equip);

	VFX_Sign->Deactivate();

	// Damage
	SetOwner(NewOwner);	// 设置武器持有者
	SetInstigator(InInstigator);	// 设置伤害发起者 (也就是凶手为TakeDamage函数和ApplyDamage函数服务) 
}

#pragma region ApplyDamage
void AWeapon::HitBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	FHitResult HitedActor;
	ScanActor(HitedActor);
	if (HitedActor.GetActor())
	{
		if (GetOwner()->ActorHasTag(TEXT("Enemy")) == HitedActor.GetActor()->ActorHasTag(TEXT("Enemy"))) return;
		//if (HitedActor.GetActor()->ActorHasTag(FName("Enemy"))) return;  // 禁用友伤
			UGameplayStatics::ApplyDamage(HitedActor.GetActor(), Damage, GetInstigator()->GetController(), this, UDamageType::StaticClass());
		if (IHitInterface* HI = Cast<IHitInterface>(HitedActor.GetActor()))
		{
			HI->Execute_GetHit(HitedActor.GetActor(), HitedActor.ImpactPoint);
		}
	}
}

void AWeapon::ScanActor(FHitResult& OutHit)
{
	const FVector Start = BoxTraceStartPoint->GetComponentLocation();
	const FVector End = BoxTraceEndPoint->GetComponentLocation();
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);	// 不检测武器本身
	ActorsToIgnore.Add(GetOwner());	// 不检测武器持有者
	//for (AActor* i : IgnoreActor)		//将第一次扫到的对象放入忽略对象里，这样就不会再次扫到同一个人
	//{
	//	ActorsToIgnore.AddUnique(i);	
	//}
	UKismetSystemLibrary::BoxTraceSingle(
		this,
		Start, End,
		FVector(5.f, 5.f, 5.f),
		BoxTraceStartPoint->GetComponentRotation(),
		ETraceTypeQuery::TraceTypeQuery1,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::ForDuration,
		OutHit,
		true);  

	// 保存第一次扫到的对象
	IgnoreActor.AddUnique(OutHit.GetActor());
}
#pragma endregion
