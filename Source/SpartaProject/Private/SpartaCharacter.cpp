#include "SpartaCharacter.h"
#include "SpartaPlayerController.h"
#include "EnhancedInputComponent.h"
// 카메라, 스프링 암 함수가 필요한 경우라 include 추가
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/TextBlock.h"
#include "Hw08GameplayTags.h"
#include "SpartaGameState.h"

// Sets default values
ASpartaCharacter::ASpartaCharacter()
{
    // Tick 함수 사용 불가
    PrimaryActorTick.bCanEverTick = false;

    //스프링 암 생성
    SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    // 스프링 암을 루트 컴포넌트 (CapsuleComponent)에 부착
    SpringArmComp->SetupAttachment(RootComponent);
    // 캐릭터와 카메라 사이의 거리 기본값 300으로 설정
    SpringArmComp->TargetArmLength = 300.0f;
    // 컨트롤러 회전(화면 회전)에 따라 스프링 암도 회전하도록 설정
    SpringArmComp->bUsePawnControlRotation = true;

    // 카메라 생성
    CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    // 스프링 암의 소켓 위치에 카메라를 부착
    CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
    // 스프링암을 따라 회전하도록 회전 설정 끄기
    CameraComp->bUsePawnControlRotation = false;

    OverheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidget"));
    OverheadWidget->SetupAttachment(GetMesh());
    OverheadWidget->SetWidgetSpace(EWidgetSpace::Screen);


    NormalSpeed = 600.0f;
    SprintSpeedMultiplier = 1.5f;
    SprintSpeed = NormalSpeed * SprintSpeedMultiplier;
    OriginalSpeed = NormalSpeed;

    GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;

    MaxHealth = 100.0f;
    Health = MaxHealth;

}

void ASpartaCharacter::BeginPlay()
{
    Super::BeginPlay();

    UpdateOverheadHP();
}

int32 ASpartaCharacter::GetHealth() const
{
    return Health;
}

void ASpartaCharacter::AddHealth(float Amount)
{
    Health = FMath::Clamp(Health + Amount, 0.0f, MaxHealth);
    UpdateOverheadHP();
    UE_LOG(LogTemp, Log, TEXT("Health increased to : %f"), Health);
}

void ASpartaCharacter::ShowIceEffectUI()
{
    if (ASpartaPlayerController* PC = Cast<ASpartaPlayerController>(GetController())) 
    {
        PC->ShowIceUI();
    }
}

void ASpartaCharacter::ShowBlackEffectUI()
{
    if (ASpartaPlayerController* PC = Cast<ASpartaPlayerController>(GetController()))
    {
        
        PC->ShowBlackUI();
    }
}

void ASpartaCharacter::SetNormalSpeed(float value)
{
    if (NormalSpeed > 300.f)
    {
        NormalSpeed *= value;
    }
    GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
    SprintSpeed = NormalSpeed * SprintSpeedMultiplier;
}

void ASpartaCharacter::ResetSpeed()
{
    NormalSpeed = OriginalSpeed;
    GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
    SprintSpeed = NormalSpeed * SprintSpeedMultiplier;
}



void ASpartaCharacter::OnDeath()
{
    ASpartaGameState* SpartaGameState = GetWorld() ? GetWorld()->GetGameState<ASpartaGameState>() : nullptr;
    if (SpartaGameState) {
        SpartaGameState->OnGameOver();
    }
}

void ASpartaCharacter::UpdateOverheadHP()
{
    if (!OverheadWidget) return;

    UUserWidget* OverheadWidgetInstance = OverheadWidget->GetUserWidgetObject();
    if (!OverheadWidgetInstance) return;

    if (UTextBlock* HPText = Cast<UTextBlock>(OverheadWidgetInstance->GetWidgetFromName(TEXT("OverHeadHP"))))
    {
        HPText->SetText(FText::FromString(FString::Printf(TEXT("%.0f / %.0f"), Health, MaxHealth)));
    }
}

float ASpartaCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{

    float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

    // 체력을 데미지만큼 감소시키고, 0 이하로 떨어지지 않도록 Clamp
    Health = FMath::Clamp(Health - DamageAmount, 0.0f, MaxHealth);
    UE_LOG(LogTemp, Warning, TEXT("Health decreased to: %f"), Health);
    UpdateOverheadHP();
    if (Health <= 0.0f)
    {
        OnDeath();
    }


    return ActualDamage;
}

// Called to bind functionality to input
void ASpartaCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

    //플레이어 인풋 컴포넌트가 인핸스드 인풋 컴포넌트인지 확인
    if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {

        //소유중인 Controller를 만든 PlayerController로 설정
        if (ASpartaPlayerController* PlayerController = Cast<ASpartaPlayerController>(GetController())) {
            //W S A D 누르는 동안 Move() 호출
            if (PlayerController->MoveAction) {
                EnhancedInput->BindAction(PlayerController->MoveAction, ETriggerEvent::Triggered, this, &ASpartaCharacter::Move);
            }


            //마우스 움직이는 동안 Look() 호출
            if (PlayerController->LookAction) {
                EnhancedInput->BindAction(PlayerController->LookAction, ETriggerEvent::Triggered, this, &ASpartaCharacter::Look);
            }

            //스페이스바 누르면 점프 시작, 손을 때면 점프 중단.
            if (PlayerController->JumpAction) {
                EnhancedInput->BindAction(PlayerController->JumpAction, ETriggerEvent::Triggered, this, &ASpartaCharacter::StartJump);
                EnhancedInput->BindAction(PlayerController->JumpAction, ETriggerEvent::Completed, this, &ASpartaCharacter::StopJump);
            }

            //왼쪽 쉬프트 누르면 달리기 시작, 손을 때면 달리기 중단.
            if (PlayerController->SprintAction) {
                EnhancedInput->BindAction(PlayerController->SprintAction, ETriggerEvent::Triggered, this, &ASpartaCharacter::StartSprint);
                EnhancedInput->BindAction(PlayerController->SprintAction, ETriggerEvent::Completed, this, &ASpartaCharacter::StopSprint);
            }

            if (PlayerController->MenuAction) {
                EnhancedInput->BindAction(PlayerController->MenuAction, ETriggerEvent::Triggered, this, &ASpartaCharacter::StartMenu);
            }

        }
    }
}

void ASpartaCharacter::Move(const FInputActionValue& value)
{
    if (!Controller || CurrentStateTags.HasTag(TAG_State_Frozen)) return;

    const FVector2D MoveInput = value.Get<FVector2D>();


    if (!FMath::IsNearlyZero(MoveInput.X)) {
        AddMovementInput(GetActorForwardVector(), MoveInput.X);
    }

    if (!FMath::IsNearlyZero(MoveInput.Y)) {
        AddMovementInput(GetActorRightVector(), MoveInput.Y);
    }

}

void ASpartaCharacter::StartJump(const FInputActionValue& value)
{
    if (CurrentStateTags.HasTag(TAG_State_Frozen)) return;
    if (value.Get<bool>()) {
        Jump();
    }
}

void ASpartaCharacter::StopJump(const FInputActionValue& value)
{
    if (CurrentStateTags.HasTag(TAG_State_Frozen)) return;
    if (!value.Get<bool>()) {
        StopJumping();
    }
}

void ASpartaCharacter::Look(const FInputActionValue& value)
{
    if (CurrentStateTags.HasTag(TAG_State_Frozen)) return;
    FVector2D LookInput = value.Get<FVector2D>();

    AddControllerYawInput(LookInput.X);
    AddControllerPitchInput(LookInput.Y);
}

void ASpartaCharacter::StartSprint(const FInputActionValue& value)
{
    // Shift 키를 누른 순간 이 함수가 호출된다고 가정
     // 스프린트 속도를 적용
    if (GetCharacterMovement())
    {
        GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
    }
}

void ASpartaCharacter::StopSprint(const FInputActionValue& value)
{
    // Shift 키를 뗀 순간 이 함수가 호출
    // 평상시 속도로 복귀
    if (GetCharacterMovement())
    {
        GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
    }
}

void ASpartaCharacter::StartMenu(const FInputActionValue& value)
{
    if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
    {
        if (ASpartaPlayerController* SpartaPlayerController = Cast<ASpartaPlayerController>(PlayerController))
        {
            
            SpartaPlayerController->ShowMainMenu(true);
        }
    }
}


