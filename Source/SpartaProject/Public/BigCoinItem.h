// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CoinItem.h"
#include "BigCoinItem.generated.h"

UCLASS()
class SPARTAPROJECT_API ABigCoinItem : public ACoinItem
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABigCoinItem();

protected:
	
	virtual void ActivateItem(AActor* Activator) override;
};
