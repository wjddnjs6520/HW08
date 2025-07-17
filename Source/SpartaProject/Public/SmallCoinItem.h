// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CoinItem.h"
#include "SmallCoinItem.generated.h"

UCLASS()
class SPARTAPROJECT_API ASmallCoinItem : public ACoinItem
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASmallCoinItem();

protected:

	virtual void ActivateItem(AActor* Activator) override;
};
