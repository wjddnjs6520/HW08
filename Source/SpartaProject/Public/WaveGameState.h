// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "WaveGameState.generated.h"

/**
 * 
 */
UCLASS()
class SPARTAPROJECT_API AWaveGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
    AWaveGameState();

    virtual void BeginPlay() override;

    // 전역 점수를 저장하는 변수
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Score")
    int32 Score;

    // 전역 점수를 저장하는 변수
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Coin")
    int32 SpawnedCoinCount;
    // 전역 점수를 저장하는 변수
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Coin")
    int32 CollectedCoinCount;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level")
    float WaveDuration;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level")
    int32 CurrentWaveIndex;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level")
    int32 MaxWave;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Level")
    TArray<FName> LevelMapNames;

    FTimerHandle WaveTimerHandle;
    FTimerHandle HUDUpdateTimerHandle;

    // 현재 점수를 읽는 함수
    UFUNCTION(BlueprintPure, Category = "Score")
    int32 GetScore() const;

    // 점수를 추가해주는 함수
    UFUNCTION(BlueprintCallable, Category = "Score")
    void AddScore(int32 Amount);

    UFUNCTION(BlueprintCallable, Category = "Level")
    void OnGameOver();

    void OnLevleTimeUP();
    void OnCoinCollected();
    void EndWave();
    UFUNCTION(BlueprintCallable, Category = "Wave")
    void StartWave();

    void UpdateHUD();
};