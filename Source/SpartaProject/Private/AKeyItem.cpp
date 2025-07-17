// Fill out your copyright notice in the Description page of Project Settings.


#include "AKeyItem.h"

// Sets default values
AAKeyItem::AAKeyItem()
{
	ItemType = "Key";
}


//오버랩시 로직
void AAKeyItem::OnItemOverlap(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("Key collected!"));
}


//아이템 사용
void AAKeyItem::ActivateItem(AActor* Activator)
{
	UE_LOG(LogTemp, Warning, TEXT("Key used to unlock the door!"));
	DestroyItem();
}