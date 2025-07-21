// Fill out your copyright notice in the Description page of Project Settings.


#include "WaveGameState.h"
#include "SpartaPlayerController.h"
#include "SpawnVolume.h"
#include "SpartaCharacter.h"
#include "Hw08GameplayTags.h"
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
    WaveDuration = 10.0f;
    CurrentWaveIndex = 0;
    MaxWave = 3;

}

void AWaveGameState::BeginPlay()
{
    Super::BeginPlay();
    UpdateHUD();

    FString MapName = GetWorld()->GetMapName();
    if (!MapName.Contains("MenuLevel"))  // 게임 레벨일 경우에만 시작
    {
        StartWave();
    }

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
            SpartaPlayerController->PlayWaveComentAnim();
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

    APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
    if (ASpartaCharacter* Player = Cast<ASpartaCharacter>(PlayerPawn))
    {
        Player->CurrentStateTags.RemoveTag(TAG_State_Frozen);
        Player->ResetSpeed();
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
            SpartaPlayerController->ShowMainMenu(true, true);
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
                if (UTextBlock* NextWaveText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("NextWave"))))
                {
                    if (CurrentWaveIndex + 2 <= MaxWave)
                    {
                        NextWaveText->SetText(FText::FromString(FString::Printf(TEXT("Wave %d"), CurrentWaveIndex + 2)));
                    }
                    else
                    {
                        NextWaveText->SetText(FText::FromString(FString::Printf(TEXT("Game Over!"))));
                    }
                }

                if (UTextBlock* CurrentWaveText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("CurrentWave"))))
                {
                    CurrentWaveText->SetText(FText::FromString(FString::Printf(TEXT("Wave %d"), CurrentWaveIndex + 1)));
                }

                if (UTextBlock* RemainTimeText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("RemainTime"))))
                {
                    float RemainingTime = GetWorldTimerManager().GetTimerRemaining(WaveTimerHandle);
                    RemainTimeText->SetText(FText::FromString(FString::Printf(TEXT("%.1f"), RemainingTime)));
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

                if (UTextBlock* CollectCoinText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("CurrentCoinText"))))
                {
                    CollectCoinText->SetText(FText::FromString(FString::Printf(TEXT("%d"), CollectedCoinCount)));
                }

                if (UTextBlock* CurrentCoinText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("TotalCoinText"))))
                {
                    CurrentCoinText->SetText(FText::FromString(FString::Printf(TEXT("/ %d"), SpawnedCoinCount)));
                }

                if (UTextBlock* MissionText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("MissionText"))))
                {
                    if (CurrentWaveIndex == 0) {
                        MissionText->SetText(FText::FromString(FString::Printf(TEXT("조심히 코인을 모아!!"), SpawnedCoinCount)));
                    }
                    else if (CurrentWaveIndex == 1) {
                        MissionText->SetText(FText::FromString(FString::Printf(TEXT("함정을 피하면서! \n더!더! 모으는거야!"), SpawnedCoinCount)));
                    }
                    else if (CurrentWaveIndex == 2) {
                        MissionText->SetText(FText::FromString(FString::Printf(TEXT("이런! 들켜버렸어. \n최대한 많은 코인을 모아!!"), SpawnedCoinCount)));
                    }
                }

                if (UTextBlock* WaveComentText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("WaveComentText"))))
                {
                    WaveComentText->SetText(FText::FromString(FString::Printf(TEXT("Wave %d"), CurrentWaveIndex + 1)));
                }

            }
        }
    }
}