#include "SpartaPlayerController.h"
#include "EnhancedInputSubsystems.h" // Local Player Subsystem 사용
#include "Blueprint/UserWidget.h" // 헤더 추가
#include "SpartaGameState.h"
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
    MainMenuWidgetInstance(nullptr)
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
        ShowMainMenu(false);
    }
}

UUserWidget* ASpartaPlayerController::GetHUDWidget() const
{
    return HUDWidgetInstance;
}

void ASpartaPlayerController::ShowMainMenu(bool bIsRestart)
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
        if (StartButtonWidget && RestartButtonWidget && MainCloseButtonWidget)
        {
            if (bIsRestart)
            {
                StartButtonWidget->SetVisibility(ESlateVisibility::Hidden);
                RestartButtonWidget->SetVisibility(ESlateVisibility::Visible);
                MainCloseButtonWidget->SetVisibility(ESlateVisibility::Visible);
                UFunction* PlayAnimFunc = MainMenuWidgetInstance->FindFunction(FName("PlayGameOverAnim"));
                if (PlayAnimFunc)
                {
                    MainMenuWidgetInstance->ProcessEvent(PlayAnimFunc, nullptr);
                }

                if (UTextBlock* TotalScoreText = Cast<UTextBlock>(MainMenuWidgetInstance->GetWidgetFromName("TotalScoreText")))
                {
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

            ASpartaGameState* SpartaGameState = GetWorld() ? GetWorld()->GetGameState<ASpartaGameState>() : nullptr;

            if (SpartaGameState)
            {
                SpartaGameState->UpdateHUD();
            }
        }
    }

}

void ASpartaPlayerController::StartGame()
{
    if (USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(UGameplayStatics::GetGameInstance(this)))
    {
        SpartaGameInstance->CurrentLevelIndex = 0;
        SpartaGameInstance->TotalScore = 0;
    }

    UGameplayStatics::OpenLevel(GetWorld(), FName("BasicLevel"));
    SetPause(false);
}