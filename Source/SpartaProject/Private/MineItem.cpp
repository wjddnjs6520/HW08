// Fill out your copyright notice in the Description page of Project Settings.


#include "MineItem.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"
#include "Particles/ParticleSystemComponent.h"


// Sets default values
AMineItem::AMineItem()
{
	ExplosionDelay = 5.0f;
	ExplosionRadius = 300.0f;
	ExplosionDamage = 30.0f;
	ItemType = "Mine";
	bHasExploded = false;

	//폭발 콜리전
	ExplosionCollision = CreateDefaultSubobject<USphereComponent>(TEXT("ExplosionCollision"));
	//폭발 반경만큼 크기 설정
	ExplosionCollision->InitSphereRadius(ExplosionRadius);
	//콜리전 모든 항목 오버랙
	ExplosionCollision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	//루트 컴포넌트에 부착
	ExplosionCollision->SetupAttachment(Scene);
}

void AMineItem::ActivateItem(AActor* Activator)
{
	if (bHasExploded) return;
	Super::ActivateItem(Activator);

	GetWorld()->GetTimerManager().SetTimer(
		ExplosionTimerHandle,
		this,
		&AMineItem::Explode,
		ExplosionDelay,
		false
	);

	bHasExploded = true;
}

void AMineItem::Explode()
{
	UParticleSystemComponent* Particle = nullptr;

	if (ExplosionParticle)
	{
		Particle = UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			ExplosionParticle,
			GetActorLocation(),
			GetActorRotation(),
			false
		);
	}

	if (ExplosionSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			GetWorld(),
			ExplosionSound,
			GetActorLocation()
		);
	}

	//오버랩된 액터들을 저장할 Array
	TArray<AActor*> OverlappingActors;
	//현재 폭발 반경 내 오버랩된 모든 액터를 Array에 불러옴
	ExplosionCollision->GetOverlappingActors(OverlappingActors);

	//Array 내부 원소 탐색
	for (AActor* Actor : OverlappingActors)
	{
		//플레이어가 있다면 데미지
		if (Actor && Actor->ActorHasTag("Player"))
		{
			UGameplayStatics::ApplyDamage(
				Actor,                      // 데미지를 받을 액터
				ExplosionDamage,            // 데미지 양
				nullptr,                    // 데미지를 유발한 주체 (지뢰를 설치한 캐릭터가 없으므로 nullptr)
				this,                       // 데미지를 유발한 오브젝트(지뢰)
				UDamageType::StaticClass()  // 기본 데미지 유형
			);
		}
	}

	// 아이템 폭발 후 지뢰 제거
	DestroyItem();

	if (Particle)
	{
		FTimerHandle DestroyParticleTimerHandle;
		TWeakObjectPtr<UParticleSystemComponent> WeakParticle = Particle;

		GetWorld()->GetTimerManager().SetTimer(
			DestroyParticleTimerHandle,
			[WeakParticle]()
			{
				if (WeakParticle.IsValid())
				{
					WeakParticle->DestroyComponent();
				}
			},
			2.0f,
			false
		);
	}
}


