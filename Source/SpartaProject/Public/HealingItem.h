﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseItem.h"
#include "HealingItem.generated.h"

UCLASS()
class SPARTAPROJECT_API AHealingItem : public ABaseItem
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHealingItem();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	float HealAmount;

	virtual void ActivateItem(AActor* Activator) override;

};
