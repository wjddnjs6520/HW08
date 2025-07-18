// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SpartaCharacter.h"
#include "BaseItem.h"
#include "CoinItem.h"
#include "Magnetic.generated.h"

/**
 * 
 */
UCLASS()
class SPARTAPROJECT_API AMagnetic : public ABaseItem
{
	GENERATED_BODY()
public:
	AMagnetic();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item|Component")
	USphereComponent* MagnetCollision;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mine")
	float MagnetRadius;

	FTimerHandle MagnetTimerHandle;
	FTimerHandle DeleteTimerHandle;

	virtual void ActivateItem(AActor* Activator) override;

	UFUNCTION()
	void MagnetCoin(ASpartaCharacter* Player);
	UFUNCTION()
	void EndMagnet();
	bool bHasMagnet;

};
