// Fill out your copyright notice in the Description page of Project Settings.


#include "IceDrink.h"
#include "Hw08GameplayTags.h"
#include "SpartaCharacter.h"
AIceDrink::AIceDrink()
{
	ItemType = "IceDrink";
}


void AIceDrink::ActivateItem(AActor* Activator)
{
    Super::ActivateItem(Activator);

    if (ASpartaCharacter* Player = Cast<ASpartaCharacter>(Activator)) {
        Player->CurrentStateTags.AddTag(TAG_State_Frozen);
        Player->ShowIceEffectUI();
        FTimerDelegate StunDelegate;
        StunDelegate.BindUFunction(this, FName("DeleteState"), Activator);
        GetWorldTimerManager().SetTimer(FrozenTimerHandle, StunDelegate, 3.f, false);
        GetWorldTimerManager().SetTimer(DeleteTimerHandle, this, &ABaseItem::DestroyItem, 3.1f, false);
    }
    SetActorHiddenInGame(true);
}

void AIceDrink::DeleteState(AActor* Activator)
{
    
    if (ASpartaCharacter* Player = Cast<ASpartaCharacter>(Activator)) {
        Player->CurrentStateTags.RemoveTag(TAG_State_Frozen);
    }
}
