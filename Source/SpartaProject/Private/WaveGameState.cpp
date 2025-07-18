// Fill out your copyright notice in the Description page of Project Settings.


#include "WaveGameState.h"
#include "SpartaPlayerController.h"
#include "SpawnVolume.h"
#include "BaseItem.h"
#include "Kismet/GameplayStatics.h"
#include "CoinItem.h"
#include "SpartaGameInstance.h"
#include "Components/TextBlock.h"
#include "Blueprint/UserWidget.h"

AWaveGameState::AWaveGameState()
{
    Score = 0;
    SpawnedCoinCount = 0;
    CollectedCoinCount = 0;
    WaveDuration = 60.0f;
    CurrentWaveIndex = 0;
    MaxWave = 3;

}

void AWaveGameState::BeginPlay()
{
    Super::BeginPlay();
    UpdateHUD();
    StartWave();

    GetWorldTimerManager().SetTimer(
        HUDUpdateTimerHandle,
        this,
        &AWaveGameState::UpdateHUD,
        0.1f,
        true
    );
}

int32 AWaveGameState::GetScore() const
{
    return Score;
}

void AWaveGameState::AddScore(int32 Amount)
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

void AWaveGameState::OnCoinCollected() 
{
    CollectedCoinCount++;
    UE_LOG(LogTemp, Warning, TEXT("Coin Collected: %d / %d"),
        CollectedCoinCount,
        SpawnedCoinCount);
    if (SpawnedCoinCount > 0 && CollectedCoinCount >= SpawnedCoinCount) {
        EndWave();
    }
}

void AWaveGameState::StartWave()
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
            CurrentWaveIndex = SpartaGameInstance->CurrentWaveIndex;
        }
    }

    SpawnedCoinCount = 0;
    CollectedCoinCount = 0;
    const int ItemToSpawn = 40;
    TArray<AActor*> FoundVolumes;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnVolume::StaticClass(), FoundVolumes);

    if (FoundVolumes.IsValidIndex(CurrentWaveIndex))
    {
        ASpawnVolume* SpawnVolume = Cast<ASpawnVolume>(FoundVolumes[CurrentWaveIndex]);
        if (SpawnVolume)
        {
            for (int32 i = 0; i < ItemToSpawn; i++) {
                AActor* SpawnedActor = SpawnVolume->SpawnRandomItem();
                if (SpawnedActor && SpawnedActor->IsA(ACoinItem::StaticClass()))
                {
                    SpawnedCoinCount++;
                }
            }
        }
    }

    GetWorldTimerManager().SetTimer(
        WaveTimerHandle,
        this,
        &AWaveGameState::OnLevleTimeUP,
        WaveDuration,
        false
    );

    UpdateHUD();

    UE_LOG(LogTemp, Warning, TEXT("Level %d Start!, Spawned %d Coin"),
        CurrentWaveIndex + 1,
        SpawnedCoinCount);
}




void AWaveGameState::OnLevleTimeUP()
{
    EndWave();
}

void AWaveGameState::EndWave()
{
    TArray<AActor*> RemainItems;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABaseItem::StaticClass(), RemainItems);

    UE_LOG(LogTemp, Warning, TEXT("총 아이템 수: %d"), RemainItems.Num());

    for (AActor* ItemActor : RemainItems)
    {
        if (ItemActor)
        {
            ItemActor->Destroy();
        }
    }

    GetWorldTimerManager().ClearTimer(WaveTimerHandle);
    CurrentWaveIndex++;
    UE_LOG(LogTemp, Warning, TEXT("%d"), CurrentWaveIndex);
    if (UGameInstance* GameInstance = GetGameInstance())
    {
        USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance);
        if (SpartaGameInstance)
        {
            AddScore(Score);
            SpartaGameInstance->CurrentWaveIndex = CurrentWaveIndex;
        }
    }
    if (CurrentWaveIndex >= MaxWave)
    {
        OnGameOver();
        return;
    }

    StartWave();
}

void AWaveGameState::OnGameOver()
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

void AWaveGameState::UpdateHUD()
{
    if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
    {
        if (ASpartaPlayerController* SpartaPlayerController = Cast<ASpartaPlayerController>(PlayerController))
        {
            if (UUserWidget* HUDWidget = SpartaPlayerController->GetHUDWidget())
            {
                if (UTextBlock* TimeText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("TimeValue"))))
                {
                    float RemainingTime = GetWorldTimerManager().GetTimerRemaining(WaveTimerHandle);
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
                            LevelText->SetText(FText::FromString(FString::Printf(TEXT("Level: %d"), CurrentWaveIndex + 1)));
                        }
                    }
                }
            }
        }
    }
}