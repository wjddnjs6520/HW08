// Fill out your copyright notice in the Description page of Project Settings.


#include "Magnetic.h"
#include "Components/SphereComponent.h"
#include "CoinItem.h"
#include "SpartaCharacter.h"
AMagnetic::AMagnetic()
{

	MagnetRadius = 1500.f;
	ItemType = "Magentic";
	bHasMagnet = false;

	MagnetCollision = CreateDefaultSubobject<USphereComponent>(TEXT("MagnetCollision"));
	MagnetCollision->InitSphereRadius(MagnetRadius);
	MagnetCollision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	MagnetCollision->SetupAttachment(Scene);
}

void AMagnetic::ActivateItem(AActor* Activator)
{
	if (bHasMagnet) return;
	Super::ActivateItem(Activator);

	
	if (ASpartaCharacter* Player = Cast<ASpartaCharacter>(Activator)) 
	{
		FTimerDelegate MagnetDelegate;
		MagnetDelegate.BindUFunction(this, FName("MagnetCoin"), Player);
		
		GetWorldTimerManager().SetTimer(MagnetTimerHandle, MagnetDelegate, 0.1f, true);
		GetWorldTimerManager().SetTimer(DeleteTimerHandle, this, &AMagnetic::EndMagnet, 3.0f, false);
	}
	bHasMagnet = true;
	SetActorHiddenInGame(true);
	StaticMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AMagnetic::MagnetCoin(ASpartaCharacter* Player)
{
	SetActorLocation(Player->GetActorLocation());
	TArray<AActor*> OverlappingActors;
	TArray<ACoinItem*> OverlapCoins;
	MagnetCollision->GetOverlappingActors(OverlappingActors);
	for (AActor* Actor : OverlappingActors)
	{
		if (ACoinItem* Coin = Cast<ACoinItem>(Actor))
		{
			OverlapCoins.Add(Coin);
		}
	}

	for (ACoinItem* Coin : OverlapCoins)
	{
		FVector Movedirection = GetActorLocation() - Coin->GetActorLocation();
		FVector NormalizedDir = Movedirection.GetSafeNormal();

		Coin->AddActorLocalOffset(NormalizedDir * 50.f);
	}
}

void AMagnetic::EndMagnet()
{
	GetWorld()->GetTimerManager().ClearTimer(MagnetTimerHandle);
	Destroy();
}
