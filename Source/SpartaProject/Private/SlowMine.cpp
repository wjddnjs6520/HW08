// Fill out your copyright notice in the Description page of Project Settings.


#include "SlowMine.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"
#include "SpartaCharacter.h"
// Sets default values
ASlowMine::ASlowMine()
{
	SlowValue = 0.5;
	ItemType = "SlowMine";

}

void ASlowMine::ActivateItem(AActor* Activator)
{
	Super::ActivateItem(Activator);

	if (ASpartaCharacter* Player = Cast<ASpartaCharacter>(Activator)) {

		Player->SetNormalSpeed(SlowValue);

		FTimerDelegate EndSlowDelegate;
		EndSlowDelegate.BindUFunction(this, FName("EndSlow"), Player);
		GetWorldTimerManager().SetTimer(SlowTimerHandle, EndSlowDelegate, 2.f, false);

		GetWorldTimerManager().SetTimer(DeleteTimerHandle, this, &ABaseItem::DestroyItem, 2.1f, false);
	}
	SetActorHiddenInGame(true);	
	SetActorEnableCollision(false);
}

void ASlowMine::EndSlow(ASpartaCharacter* Activator)
{
	Activator->ResetSpeed();
}
