﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "SpartaGameState.h"
#include "SpartaPlayerController.h"
#include "SpawnVolume.h"
#include "Kismet/GameplayStatics.h"
#include "CoinItem.h"
#include "SpartaGameInstance.h"
#include "Components/TextBlock.h"
#include "Blueprint/UserWidget.h"

ASpartaGameState::ASpartaGameState()
{
    Score = 0;
    SpawnedCoinCount = 0;
    CollectedCoinCount = 0;
    LevelDuration = 30.0f;
    CurrentLevelIndex = 0;
    MaxLevel = 3;

}

void ASpartaGameState::BeginPlay()
{
    Super::BeginPlay();
    UpdateHUD();
    StartLevel();

    GetWorldTimerManager().SetTimer(
        HUDUpdateTimerHandle,
        this,
        &ASpartaGameState::UpdateHUD,
        0.1f,
        true
    );
}

int32 ASpartaGameState::GetScore() const
{
    return Score;
}

void ASpartaGameState::AddScore(int32 Amount)
{
    if (UGameInstance* GameInstance = GetGameInstance())
    {
        USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance);
        if (SpartaGameInstance)
        {
            SpartaGameInstance->AddToScore(Amount);
        }
    }
}

void ASpartaGameState::OnCoinCollected() {
    CollectedCoinCount++;
    UE_LOG(LogTemp, Warning, TEXT("Coin Collected: %d / %d"),
        CollectedCoinCount,
        SpawnedCoinCount);
    if (SpawnedCoinCount > 0 && CollectedCoinCount >= SpawnedCoinCount) {
        EndLevel();
    }
}

void ASpartaGameState::StartLevel() 
{
    if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
    {
        if (ASpartaPlayerController* SpartaPlayerController = Cast<ASpartaPlayerController>(PlayerController))
        {
            SpartaPlayerController->ShowGameHUD();
        }
    }

    
    if (UGameInstance* GameInstance = GetGameInstance())
    {
        USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance);
        if (SpartaGameInstance)
        {
            CurrentLevelIndex = SpartaGameInstance->CurrentLevelIndex;
        }
    }

    SpawnedCoinCount = 0;
    CollectedCoinCount = 0;

    TArray<AActor*> FoundVolumes;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnVolume::StaticClass(), FoundVolumes);

    const int ItemToSpawn = 40;

    for (int32 i = 0; i < ItemToSpawn; i++) {
        if (FoundVolumes.Num() > 0) {
            ASpawnVolume* SpawnVolume = Cast<ASpawnVolume>(FoundVolumes[0]);
            if (SpawnVolume)
            {
                AActor* SpawnedActor = SpawnVolume->SpawnRandomItem();
                if (SpawnedActor && SpawnedActor->IsA(ACoinItem::StaticClass()))
                {
                    SpawnedCoinCount++;
                }
            }
        }
    }

    GetWorldTimerManager().SetTimer(
        LevelTimerHandle,        
        this,
        &ASpartaGameState::OnLevleTimeUP,
        LevelDuration,
        false
    );

    UpdateHUD();

    UE_LOG(LogTemp, Warning, TEXT("Level %d Start!, Spawned %d Coin"),
        CurrentLevelIndex + 1,
        SpawnedCoinCount);

}




void ASpartaGameState::OnLevleTimeUP() 
{
    EndLevel();
}

void ASpartaGameState::EndLevel() 
{
    GetWorldTimerManager().ClearTimer(LevelTimerHandle);
    CurrentLevelIndex++;
    UE_LOG(LogTemp, Warning, TEXT("%d"), CurrentLevelIndex);
    if (UGameInstance* GameInstance = GetGameInstance())
    {
        USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance);
        if (SpartaGameInstance)
        {
            AddScore(Score);
            SpartaGameInstance->CurrentLevelIndex = CurrentLevelIndex ;
        }
    }
    if (CurrentLevelIndex > MaxLevel) 
    {
        OnGameOver();
        return;
    }
    
    if (LevelMapNames.IsValidIndex(CurrentLevelIndex))
    {
        UGameplayStatics::OpenLevel(GetWorld(), LevelMapNames[CurrentLevelIndex]);
    }
    else {
        OnGameOver();
    }
}

void ASpartaGameState::OnGameOver() 
{
    if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
    {
        if (ASpartaPlayerController* SpartaPlayerController = Cast<ASpartaPlayerController>(PlayerController))
        {
            SpartaPlayerController->SetPause(true);
            SpartaPlayerController->ShowMainMenu(true);
        }
    }
}

void ASpartaGameState::UpdateHUD()
{
    if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
    {
        if (ASpartaPlayerController* SpartaPlayerController = Cast<ASpartaPlayerController>(PlayerController))
        {
            if (UUserWidget* HUDWidget = SpartaPlayerController->GetHUDWidget())
            {
                if (UTextBlock* TimeText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("TimeValue"))))
                {
                    float RemainingTime = GetWorldTimerManager().GetTimerRemaining(LevelTimerHandle);
                    TimeText->SetText(FText::FromString(FString::Printf(TEXT("Time: %.1f"), RemainingTime)));
                }

                if (UTextBlock* ScoreText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("ScoreValue"))))
                {
                    if (UGameInstance* GameInstance = GetGameInstance()) 
                    {
                        USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance);
                        if (SpartaGameInstance) 
                        {
                            ScoreText->SetText(FText::FromString(FString::Printf(TEXT("Score: %d"), SpartaGameInstance->TotalScore)));
                        }
                    }
                }
                if (UTextBlock* LevelText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Level"))))
                {
                    if (UGameInstance* GameInstance = GetGameInstance())
                    {
                        USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance);
                        if (SpartaGameInstance)
                        {
                            LevelText->SetText(FText::FromString(FString::Printf(TEXT("Level: %d"), CurrentLevelIndex+1)));
                        }
                    }
                }
            }
        }
    }
}