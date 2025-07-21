#include "SpartaPlayerController.h"
#include "EnhancedInputSubsystems.h" // Local Player Subsystem 사용
#include "Blueprint/UserWidget.h" // 헤더 추가
#include "WaveGameState.h"
#include "SpartaGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

ASpartaPlayerController::ASpartaPlayerController() 
	: InputMappingContext(nullptr),
	MoveAction(nullptr),
	JumpAction(nullptr),
	LookAction(nullptr),
	SprintAction(nullptr),
    MenuAction(nullptr),
    HUDWidgetClass(nullptr),
    HUDWidgetInstance(nullptr),
    MainMenuWidgetClass(nullptr),
    MainMenuWidgetInstance(nullptr),
    IceWidgetClass(nullptr),
    IceWidgetInstance(nullptr),
    BlackWidgetClass(nullptr),
    BlackWidgetInstance(nullptr)
{

}

void ASpartaPlayerController::BeginPlay()
{
    Super::BeginPlay();

    // 현재 PlayerController에 연결된 Local Player 객체를 가져옴    
    if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
    {
        // Local Player에서 EnhancedInputLocalPlayerSubsystem을 획득
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
        {
            if (InputMappingContext)
            {
                // Subsystem을 통해 우리가 할당한 IMC를 활성화
                // 우선순위(Priority)는 0이 가장 높은 우선순위
                Subsystem->AddMappingContext(InputMappingContext, 0);
            }
        }
    }

    FString CurrentMapName = GetWorld()->GetMapName();
    if (CurrentMapName.Contains("MenuLevel")) {
        ShowMainMenu(false, false);
    }
}

UUserWidget* ASpartaPlayerController::GetHUDWidget() const
{
    return HUDWidgetInstance;
}

void ASpartaPlayerController::ShowMainMenu(bool bIsRestart, bool bPlayAnim)
{
    if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
    {
        // Local Player에서 EnhancedInputLocalPlayerSubsystem을 획득
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
        {
            if (InputMappingContext)
            {
                // Subsystem을 통해 우리가 할당한 IMC를 활성화
                // 우선순위(Priority)는 0이 가장 높은 우선순위
                Subsystem->RemoveMappingContext(InputMappingContext);

            }
        }
    }
    if (HUDWidgetInstance) 
    {
        HUDWidgetInstance->RemoveFromParent();
        HUDWidgetInstance = nullptr;
    }

    if (MainMenuWidgetInstance) {
        MainMenuWidgetInstance->RemoveFromParent();
        MainMenuWidgetInstance = nullptr;
    }

    if (MainMenuWidgetClass) {
        MainMenuWidgetInstance = CreateWidget<UUserWidget>(this, MainMenuWidgetClass);
        if (MainMenuWidgetInstance)
        {
            MainMenuWidgetInstance->AddToViewport();
            bShowMouseCursor = true;
            SetInputMode(FInputModeUIOnly());
        }

        UWidget* StartButtonWidget = MainMenuWidgetInstance->GetWidgetFromName(TEXT("StartButton"));
        UWidget* RestartButtonWidget = MainMenuWidgetInstance->GetWidgetFromName(TEXT("RestartButton"));
        UWidget* MainCloseButtonWidget = MainMenuWidgetInstance->GetWidgetFromName(TEXT("MainCloseButton"));
        UWidget* ExitButtonWidget = MainMenuWidgetInstance->GetWidgetFromName(TEXT("ExitButton"));

        if (StartButtonWidget && RestartButtonWidget && MainCloseButtonWidget)
        {
            if (bIsRestart)
            {
                StartButtonWidget->SetVisibility(ESlateVisibility::Hidden);
                RestartButtonWidget->SetVisibility(ESlateVisibility::Visible);
                MainCloseButtonWidget->SetVisibility(ESlateVisibility::Visible);
                ExitButtonWidget->SetVisibility(ESlateVisibility::Hidden);
                if(bPlayAnim)
                {
                    UFunction* PlayAnimFunc = MainMenuWidgetInstance->FindFunction(FName("PlayGameOverAnim"));
                    if (PlayAnimFunc)
                    {
                        MainMenuWidgetInstance->ProcessEvent(PlayAnimFunc, nullptr);
                    }
                    MainCloseButtonWidget->SetVisibility(ESlateVisibility::Hidden);
                }
                

                if (UTextBlock* TotalScoreText = Cast<UTextBlock>(MainMenuWidgetInstance->GetWidgetFromName("TotalScoreText")))
                {
                    TotalScoreText->SetVisibility(ESlateVisibility::Visible);
                    if (USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(UGameplayStatics::GetGameInstance(this)))
                    {
                        TotalScoreText->SetText(FText::FromString(
                            FString::Printf(TEXT("Total Score: %d"), SpartaGameInstance->TotalScore)
                        ));
                    }
                }

            }
            else
            {
                StartButtonWidget->SetVisibility(ESlateVisibility::Visible);
                RestartButtonWidget->SetVisibility(ESlateVisibility::Hidden);
                MainCloseButtonWidget->SetVisibility(ESlateVisibility::Hidden);
                ExitButtonWidget->SetVisibility(ESlateVisibility::Visible);
            }
        }

    }
}

void ASpartaPlayerController::ShowGameHUD()
{
    if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
    {
        // Local Player에서 EnhancedInputLocalPlayerSubsystem을 획득
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
        {
            if (InputMappingContext)
            {
                // Subsystem을 통해 우리가 할당한 IMC를 활성화
                // 우선순위(Priority)는 0이 가장 높은 우선순위
                Subsystem->AddMappingContext(InputMappingContext, 0);

            }
        }
    }
    // HUD가 켜져 있다면 닫기
    if (HUDWidgetInstance)
    {
        HUDWidgetInstance->RemoveFromParent();
        HUDWidgetInstance = nullptr;
    }

    // 이미 메뉴가 떠 있으면 제거
    if (MainMenuWidgetInstance)
    {
        MainMenuWidgetInstance->RemoveFromParent();
        MainMenuWidgetInstance = nullptr;
    }

    if (HUDWidgetClass)
    {
        HUDWidgetInstance = CreateWidget<UUserWidget>(this, HUDWidgetClass);
        if (HUDWidgetInstance)
        {
            HUDWidgetInstance->AddToViewport();

            bShowMouseCursor = false;
            SetInputMode(FInputModeGameOnly());

            AWaveGameState* WaveGameState = GetWorld() ? GetWorld()->GetGameState<AWaveGameState>() : nullptr;

            if (WaveGameState)
            {
                WaveGameState->UpdateHUD();
            }
        }
    }

}

void ASpartaPlayerController::StartGame()
{
    if (USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(UGameplayStatics::GetGameInstance(this)))
    {
        SpartaGameInstance->CurrentWaveIndex = 0;
        SpartaGameInstance->TotalScore = 0;
    }

    UGameplayStatics::OpenLevel(GetWorld(), FName("BasicLevel"));
    SetPause(false);
}

void ASpartaPlayerController::ShowIceUI()
{
    if (ULocalPlayer* LocalPlayer = GetLocalPlayer()) 
    {
        if (IceWidgetClass && !IceWidgetInstance)
        {
            IceWidgetInstance = CreateWidget<UUserWidget>(GetWorld(), IceWidgetClass);
            if (IceWidgetInstance)
            {
                IceWidgetInstance->AddToViewport(-1);

                // 자동 제거 (3초 후)
                FTimerHandle IceUITimer;
                GetWorld()->GetTimerManager().SetTimer(IceUITimer, [this]()
                    {
                        if (IceWidgetInstance)
                        {
                            IceWidgetInstance->RemoveFromParent();
                            IceWidgetInstance = nullptr;
                        }
                    }, 3.f, false);
            }
        }
    }
}

void ASpartaPlayerController::ShowBlackUI()
{
    if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
    {
        if (BlackWidgetClass && !BlackWidgetInstance)
        {
            UWorld* World = GetWorld();
            if (!World) return;

            BlackWidgetInstance = CreateWidget<UUserWidget>(World, BlackWidgetClass);
            if (BlackWidgetInstance)
            {
                BlackWidgetInstance->AddToViewport(-1);

                // 자동 제거 (3초 후)
                World->GetTimerManager().SetTimer(BlackUITimer, [this]()
                    {
                        if (BlackWidgetInstance)
                        {
                            BlackWidgetInstance->RemoveFromParent();
                            BlackWidgetInstance = nullptr;
                        }
                    }, 3.f, false);
            }
        }
    }
}

void ASpartaPlayerController::PlayWaveComentAnim()
{
    if (HUDWidgetInstance)
    {
        UFunction* Func = HUDWidgetInstance->FindFunction(TEXT("PlayComentAnim"));
        if (Func)
        {
            HUDWidgetInstance->ProcessEvent(Func, nullptr);
        }
    }
}
