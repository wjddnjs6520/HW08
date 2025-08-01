﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "CoinItem.h"
#include "Components/SphereComponent.h"
#include "WaveGameState.h"
// Sets default values
ACoinItem::ACoinItem()
{
    // 점수 기본값을 0으로 설정
    PointValue = 0;
    ItemType = "DefaultCoin";
    Collision->SetGenerateOverlapEvents(true);
}

void ACoinItem::ActivateItem(AActor* Activator)
{
    Super::ActivateItem(Activator);
    // 플레이어 태그 확인
    if (Activator && Activator->ActorHasTag("Player"))
    {
        if (UWorld* World = GetWorld())
        {
            if (AWaveGameState* GameState = World->GetGameState<AWaveGameState>())
            {
                GameState->AddScore(PointValue);
                GameState->OnCoinCollected();
            }
        }

        // 부모 클래스 (BaseItem)에 정의된 아이템 파괴 함수 호출
        DestroyItem();
    }
}



