// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "SpartaCharacter.h"
#include "SlowMine.generated.h"


UCLASS()
class SPARTAPROJECT_API ASlowMine : public ABaseItem
{
	GENERATED_BODY()
public:
    // Sets default values for this actor's properties
    ASlowMine();


    // 느려지는 속도
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mine")
    float SlowValue;


    FTimerHandle SlowTimerHandle;
    FTimerHandle DeleteTimerHandle;
    virtual void ActivateItem(AActor* Activator) override;
    UFUNCTION()
    void EndSlow(ASpartaCharacter* Activator);
};
