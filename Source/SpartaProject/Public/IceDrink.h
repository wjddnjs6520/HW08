// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "IceDrink.generated.h"

UCLASS()
class SPARTAPROJECT_API AIceDrink : public ABaseItem
{
	GENERATED_BODY()
public:
	AIceDrink();

	FTimerHandle FrozenTimerHandle;
	FTimerHandle DeleteTimerHandle;
		
	UFUNCTION()
	void DeleteState(AActor* Activator);

// 부모 클래스에서 상속받은 ActivateItem 함수를 오버라이드
	virtual void ActivateItem(AActor* Activator) override;
};
