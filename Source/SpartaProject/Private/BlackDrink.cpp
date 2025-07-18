// Fill out your copyright notice in the Description page of Project Settings.


#include "BlackDrink.h"
#include "SpartaCharacter.h"

ABlackDrink::ABlackDrink()
{
    ItemType = "BlackDrink";
}


void ABlackDrink::ActivateItem(AActor* Activator)
{
    Super::ActivateItem(Activator);

    if (ASpartaCharacter* Player = Cast<ASpartaCharacter>(Activator)) {
        
        Player->ShowBlackEffectUI();
    }
    Destroy();
}
