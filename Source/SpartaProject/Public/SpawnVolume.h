// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemSpawnRow.h"
#include "SpawnVolume.generated.h"

class UBoxComponent;

UCLASS()
class SPARTAPROJECT_API ASpawnVolume : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpawnVolume();
	
	UFUNCTION(BlueprintCallable, Category = "Spawn")
	AActor* SpawnRandomItem();

protected:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawn");
	USceneComponent* Scene;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawn");
	UBoxComponent* SpawnBox;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawning")
	UDataTable* ItemDataTable;

	FVector GetRandomPointInVolume() const;
	FItemSpawnRow* GetRandmoItem() const;
	AActor* SpawnItem(TSubclassOf<AActor> ItemClass);

};
