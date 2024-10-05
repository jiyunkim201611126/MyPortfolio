#include "MainCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputAction.h"
#include "InputActionValue.h"

#include "Kismet/GameplayStatics.h"
#include "NameTagWidgetComponent.h"
#include "NameTagWidget.h"
#include "Component/InventoryComponent.h"
#include "Component/StatComponent.h"
#include "World/Pickup.h"
#include "Components/TimelineComponent.h"
#include "ItemEffect/ItemEffectBase.h"

#include "DrawDebugHelpers.h"

AMainCharacter::AMainCharacter()
{
	// 캡슐 콜리전 사이즈 설정
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// 카메라 회전 관련 변수
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	PlayerStatComponent = CreateDefaultSubobject<UStatComponent>(TEXT("PlayerStat"));
	PlayerStatComponent->SetWalkSpeed(200.f);
	PlayerStatComponent->SetRunSpeed(500.f);
	PlayerStatComponent->SetAimingWalkSpeed(200.f);

	PlayerStatComponent->SetMaxHPValue(100.f);
	PlayerStatComponent->SetCurrentHPValue(50.f);

	// 캐릭터 무브먼트 설정
	GetCharacterMovement()->bOrientRotationToMovement = true; // 캐릭터 이동 방향을 자동으로 움직이는 방향에 맞춰줌
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // 회전 속도

	// 각종 움직임의 속도와 관련된 변수들
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = PlayerStatComponent->GetWalkSpeed();
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// 카메라 붐 생성, RootComponent에 붙임
	cameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("cameraBoom"));
	cameraBoom->SetupAttachment(RootComponent);
	cameraBoom->SetRelativeLocation(FVector{ 0.f, 0.f, 75.f });
	cameraBoom->TargetArmLength = 300.0f; // 카메라와 캐릭터의 거리
	cameraBoom->bUsePawnControlRotation = true;
	cameraBoom->SocketOffset = FVector{ 0.0f, 50.0f, 0.0f };

	// 카메라 생성
	tpsCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("tpsCamera"));
	tpsCamera->SetupAttachment(cameraBoom, USpringArmComponent::SocketName); // 카메라 붐에 붙임
	tpsCamera->bUsePawnControlRotation = true;

	fpsCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("fpsCamera"));
	fpsCamera->SetupAttachment(GetMesh(), FName("head"));
	fpsCamera->bUsePawnControlRotation = true; // 1인칭이 되면 카메라 따라 캐릭터가 회전해야 함
	fpsCamera->SetRelativeRotation(FRotator{ 0.f, 90.f, -90.f });
	fpsCamera->SetRelativeLocation(FVector{ 3.f, 12.f, 0.f });

	// 위젯 컴포넌트 생성
	nameTagWidgetComponent = CreateDefaultSubobject<UNameTagWidgetComponent>(TEXT("nameTag"));
	nameTagWidgetComponent->SetupAttachment(RootComponent);
	nameTagWidgetComponent->SetCastShadow(false);

	// 인벤토리 컴포넌트 생성
	PlayerInventory = CreateDefaultSubobject<UInventoryComponent>(TEXT("PlayerInventory"));
	PlayerInventory->SetSlotsCapacity(10);
	PlayerInventory->SetWeightCapacity(50.0f);
	Inventory = PlayerInventory;

	// Weapon 메쉬 생성, 처음엔 비어있음
	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetupAttachment(GetMesh(), FName("Weapon_R"));

	// 상호작용 인스턴스 검출 주기와 거리
	interactionCheckFrequency = 0.1;
	interactionCheckDistance = 225.0f;
}

void AMainCharacter::ChangePlayerNameTag(FText name)
{
	UNameTagWidget* nameTag = Cast<UNameTagWidget>(nameTagWidgetComponent->GetWidget());
	nameTag->ChangePlayerName(name);
}

//////////////////////////////////////////////////////////////////////////
// 인풋 액션 매핑

void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// 인풋 매핑 컨텍스트 추가
	// 인풋 액션과 인풋 매핑 컨텍스트는 블루프린트 클래스 디폴트에서 설정해줌
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(defaultMappingContext, 0);
		}
	}

	// 액션 바인딩 설정
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{

		// 점프
		EnhancedInputComponent->BindAction(jumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(jumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// 이동
		EnhancedInputComponent->BindAction(moveAction, ETriggerEvent::Triggered, this, &AMainCharacter::Move);

		// 달리기
		EnhancedInputComponent->BindAction(run, ETriggerEvent::Started, this, &AMainCharacter::Run);
		EnhancedInputComponent->BindAction(run, ETriggerEvent::Completed, this, &AMainCharacter::StopRunning);

		// 카메라 회전
		EnhancedInputComponent->BindAction(lookAction, ETriggerEvent::Triggered, this, &AMainCharacter::Look);

		// 1인칭 전환
		EnhancedInputComponent->BindAction(aim, ETriggerEvent::Started, this, &AMainCharacter::Aim);

		// 상호작용
		EnhancedInputComponent->BindAction(interactAction, ETriggerEvent::Started, this, &AMainCharacter::BeginInteract);
		EnhancedInputComponent->BindAction(interactAction, ETriggerEvent::Completed, this, &AMainCharacter::EndInteract);

		// 메인 메뉴
		EnhancedInputComponent->BindAction(toggleInventoryAction, ETriggerEvent::Started, this, &AMainCharacter::ToggleInventory);
	}
	else
	{
		// IMC 못 찾으면 던지는 메시지
		//UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();

	fpsCamera->SetActive(false);

	Inventory->GenerateNullItems();
	Inventory->SetStartInventory();

	HUD = Cast<AMainHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());

	HUD->SetPlayerInventory(Inventory);

	HUD->SetPlayerStatComponent(PlayerStatComponent);
}

void AMainCharacter::Move(const FInputActionValue& Value)
{
	// 입력은 Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// 카메라 전방 가져오기
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// 전방 벡터 가져오기
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// 우측 벡터 가져오기
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// 무브먼트 추가
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AMainCharacter::Run()
{
	GetCharacterMovement()->MaxWalkSpeed = PlayerStatComponent->GetRunSpeed();
}

void AMainCharacter::StopRunning()
{
	GetCharacterMovement()->MaxWalkSpeed = PlayerStatComponent->GetWalkSpeed();
}

void AMainCharacter::Look(const FInputActionValue& Value)
{
	// 입력은 Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// 카메라 회전
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(-LookAxisVector.Y);
	}
}

void AMainCharacter::InterpolateCameraRotation(UCameraComponent* FromCamera, UCameraComponent* ToCamera)
{
	FVector TraceStart{ FVector::ZeroVector };
	FVector TraceEnd{ FVector::ZeroVector };
	float Distance = 1000.f;

	TraceStart = FromCamera->GetComponentLocation();
	TraceEnd = { TraceStart + (FromCamera->GetComponentRotation().Vector() * Distance) };

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	FHitResult TraceHit;

	if (GetWorld()->LineTraceSingleByChannel(TraceHit, TraceStart, TraceEnd, ECC_Visibility, QueryParams))
	{
		// 스칼라곱을 이용한 카메라 보간 정도 구하기
		FVector Direction = (TraceHit.ImpactPoint - ToCamera->GetComponentLocation());
		FRotator NewRotation = Direction.Rotation();

		Controller->SetControlRotation(NewRotation);
	}
	else
	{
		// 카메라 보간 필요 없는 경우
		return;
	}
}

void AMainCharacter::Aim()
{
	if (bAiming)
		StopAiming();
	else
		Aiming();
}

void AMainCharacter::Aiming()
{
	if (!HUD->bIsInventoryVisible)
	{
		bAiming = true;
		bUseControllerRotationYaw = true;
		GetCharacterMovement()->MaxWalkSpeed = PlayerStatComponent->GetAimingWalkSpeed();

		fpsCamera->Activate();
		tpsCamera->Deactivate();

		InterpolateCameraRotation(tpsCamera, fpsCamera);
	}
}

void AMainCharacter::StopAiming()
{
	bAiming = false;
	if (!bWeaponEquip)
		bUseControllerRotationYaw = false;
	GetCharacterMovement()->MaxWalkSpeed = PlayerStatComponent->GetWalkSpeed();

	tpsCamera->Activate();
	fpsCamera->Deactivate();

	InterpolateCameraRotation(fpsCamera, tpsCamera);
}

void AMainCharacter::ToggleInventory()
{
	HUD->ToggleInventory();
}

void AMainCharacter::ToggleOtherInventory()
{
	HUD->ToggleOtherInventory();

	if (HUD->bIsOtherInventoryVisible)
	{
		StopAiming();
	}
}

void AMainCharacter::DisplayInputNumberToDropWidget(UItemBase* FromItem, IInventoryOwnerInterface* FromOwner, UItemBase* ToItem, IInventoryOwnerInterface* ToOwner, UUserWidget* HaveItemSlot)
{
	HUD->DisplayInputNumberToDropWidget(FromItem, FromOwner, ToItem, ToOwner, HaveItemSlot);
}

void AMainCharacter::DisplayInputNumberToSplitWidget(UItemBase* ItemToSplit, IInventoryOwnerInterface* ItemOwner, UUserWidget* HaveItemSlot)
{
	HUD->DisplayInputNumberToSplitWidget(ItemToSplit, ItemOwner, HaveItemSlot);
}

FActorSpawnParameters AMainCharacter::GetSpawnParam()
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.bNoFail = true;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	return SpawnParams;
}

FTransform AMainCharacter::GetSpawnTransform()
{
	const FVector SpawnLocation{ GetActorLocation() + (GetActorForwardVector() * 50.0f) };
	const FTransform SpawnTransform(GetActorRotation(), SpawnLocation);

	return SpawnTransform;
}

void AMainCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	FVector CameraLocation = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->GetCameraLocation();
	nameTagWidgetComponent->LookCamera(CameraLocation);

	// 마지막 InteractionCheck 이후 interactionCheckFrequency 이상의 시간이 지났을 때
	if (GetWorld()->TimeSince(InteractionData.lastInteractionCheckTime) > interactionCheckFrequency)
	{
		PerformInteractionCheck();
	}
}

void AMainCharacter::PerformInteractionCheck()
{
	// 마지막 InteractionCheck 갱신
	InteractionData.lastInteractionCheckTime = GetWorld()->GetTimeSeconds();

	FVector TraceStart{ FVector::ZeroVector };
	FVector TraceEnd{ FVector::ZeroVector };

	if (!bAiming)
	{
		interactionCheckDistance = 500.f;
		//TraceStart = GetPawnViewLocation();
		// 카메라가 캐릭터 우측에 위치하므로 방향 보간
		//TraceEnd = { TraceStart + ((GetViewRotation().Vector() + FVector(0.f, 0.2f, 0.f)) * interactionCheckDistance) };

		TraceStart = tpsCamera->GetComponentLocation();
		TraceEnd = { TraceStart + (GetViewRotation().Vector() * interactionCheckDistance) };
	}
	else
	{
		interactionCheckDistance = 250.f;
		TraceStart = fpsCamera->GetComponentLocation();
		TraceEnd = { TraceStart + (GetViewRotation().Vector()* interactionCheckDistance) };
	}

	// 캐릭터와 카메라 방향이 일치할 때만 상호작용이 가능하게끔 하기 위함
	// 이게 없으면 캐릭터 방향과 관계 없이 카메라 방향만으로 상호작용 가능
	double lookDirection = FVector::DotProduct(GetActorForwardVector(), GetViewRotation().Vector());

	if (lookDirection > 0)
	{
		// 디버그라인 생성
		//DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Red, false, 1.0f, 0, 2.0f);

		// 충돌 검출 과정에서 캐릭터는 제외
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(this);
		FHitResult TraceHit;

		// Pawn의 머리로부터 카메라 방향으로 Interaction 채널 검출
		// 코드상으로는 ECC_GameTraceChannel1 이라 적혀있으나, ProjectSetting 에서 추가한 채널임
		if (GetWorld()->LineTraceSingleByChannel(TraceHit, TraceStart, TraceEnd, ECC_GameTraceChannel1, QueryParams))
		{
			// 검출된 액터의 부모 클래스가 InteractionInterface면
			if (TraceHit.GetActor()->GetClass()->ImplementsInterface(UInteractionInterface::StaticClass()))
			{
				// 검출된 액터가 현재 액터와 다르면
				if (TraceHit.GetActor() != InteractionData.CurrentInteractable)
				{
					FoundInteractable(TraceHit.GetActor());
					return;
				}

				// 검출된 액터가 현재 액터와 같으면 그냥 리턴
				if (TraceHit.GetActor() == InteractionData.CurrentInteractable)
				{
					return;
				}
			}
		}
	}

	// 아무것도 발견하지 못 한 경우
	NoInteractableFound();
}

void AMainCharacter::FoundInteractable(AActor* NewInteractable)
{
	// 이미 상호작용 중인 경우
	if (IsInteracting())
	{
		// 기존 인스턴스로부터 상호작용 종료
		EndInteract();
	}

	// 이미 발견된 상호작용 가능 인스턴스가 있는 경우
	if (InteractionData.CurrentInteractable)
	{
		// 해당 인스턴스 EndFocus (아웃라인 제거)
		targetInteractable = InteractionData.CurrentInteractable;
		targetInteractable->EndFocus();
	}

	// 발견한 상호작용 가능 인스턴스를 변수에 담고
	InteractionData.CurrentInteractable = NewInteractable;
	targetInteractable = NewInteractable;

	// 상호작용 위젯 업데이트
	HUD->UpdateInteractionWidget(&targetInteractable->InteractableData);

	// 해당 인스턴스 BeginFocus (아웃라인 생성)
	targetInteractable->BeginFocus();
}

void AMainCharacter::NoInteractableFound()
{
	// 이미 상호작용 중이던 인스턴스가 있는 경우
	if (IsInteracting())
	{
		// 타이머를 해제하고
		GetWorldTimerManager().ClearTimer(TimerHandle_Interaction);
	}

	// 변수에 해당 인스턴스가 남아있는지 체크 후
	if (InteractionData.CurrentInteractable)
	{
		// 인스턴스가 유효한지 체크 후
		if (IsValid(targetInteractable.GetObject()))
		{
			// 해당 인스턴스 EndFocus
			targetInteractable->EndFocus();
		}

		// 상호작용 위젯 숨김
		HUD->HideInteractionWidget();

		// 변수가 비어있도록 설정
		InteractionData.CurrentInteractable = nullptr;
		targetInteractable = nullptr;
	}
}

// 상호작용 시작
void AMainCharacter::BeginInteract()
{
	// 상호작용 가능 인스턴스 확실하게 체크하기 위함
	PerformInteractionCheck();

	// 없으면 그냥 리턴
	if (InteractionData.CurrentInteractable == nullptr)
	{
		return;
	}

	if (IsValid(targetInteractable.GetObject()))
	{
		// 인스턴스의 BeginInteract 실행
		targetInteractable->BeginInteract();

		// 상호작용까지 걸리는 시간(InteractionDuration)이 0에 가까우면 바로 Interact실행
		if (FMath::IsNearlyZero(targetInteractable->InteractableData.InteractionDuration, 0.1f))
		{
			Interact();
		}
		else
		{
			// 인스턴스와 상호작용하기까지 걸리는 시간 타이머로 등록, 시간이 지나면 자동으로 Interact실행
			GetWorldTimerManager().SetTimer(TimerHandle_Interaction,
				this,
				&AMainCharacter::Interact,
				targetInteractable->InteractableData.InteractionDuration,
				false);
		}
	}
}

// 키 입력을 종료했을 때 실행
void AMainCharacter::EndInteract()
{
	// ClearTimer를 실행해서 키를 뗐음에도 Interact가 실행되는 걸 방지
	GetWorldTimerManager().ClearTimer(TimerHandle_Interaction);

	if (IsValid(targetInteractable.GetObject()))
	{
		targetInteractable->EndInteract();
	}
}

// 상호작용까지 걸리는 시간 충족하면 실행
void AMainCharacter::Interact()
{
	// 상호작용 들어왔으므로 ClearTimer
	GetWorldTimerManager().ClearTimer(TimerHandle_Interaction);

	// 인스턴스 유효한지 확인
	if (IsValid(targetInteractable.GetObject()))
	{
		// 인스턴스가 가진 Interact 실행, 캐릭터를 매개변수로 넘겨줌
		targetInteractable->Interact(this);
	}
}

int32 AMainCharacter::ChangeItemWithOtherInventory(UItemBase* ChangeFrom, UItemBase* ChangeTo, bool check, int32 amount)
{
	return Inventory->ChangeItemWithOtherInventory(ChangeFrom, ChangeTo, check, amount);
}

void AMainCharacter::UpdateInteractionWidget() const
{
	if (IsValid(targetInteractable.GetObject()))
	{
		HUD->UpdateInteractionWidget(&targetInteractable->InteractableData);
	}
}

void AMainCharacter::DropItem(UItemBase* ItemToDrop, const int32 QuantityToDrop)
{
	if (Inventory->FindMatchingItem(ItemToDrop))
	{
		const int32 RemovedQuantity = Inventory->RemoveAmountOfItem(ItemToDrop, QuantityToDrop);

		APickup* Pickup = GetWorld()->SpawnActor<APickup>(APickup::StaticClass(), GetSpawnTransform(), GetSpawnParam());

		Pickup->InitializeDrop(ItemToDrop, RemovedQuantity);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Item to drop was somehow null!"));
	}
}

void AMainCharacter::ChangeItem(UItemBase* ChangeFrom, UItemBase* ChangeTo)
{
	Inventory->ChangeItemOnSameInventory(ChangeFrom, ChangeTo);
}

void AMainCharacter::SplitItem(UItemBase* ItemToSplit, const int32 QuantityToSplit)
{
	Inventory->SplitExistingStack(ItemToSplit, QuantityToSplit);
}

void AMainCharacter::UseItem(UItemBase* ItemToUse)
{
	for (TSubclassOf<AItemEffectBase> NewItemEffect : ItemToUse->ItemEffectData.ItemEffect)
	{
		AItemEffectBase* SpawnItemEffect = GetWorld()->SpawnActor<AItemEffectBase>(NewItemEffect, GetSpawnTransform(), GetSpawnParam());

		if (SpawnItemEffect)
			SpawnItemEffect->ItemEffect(ItemToUse, this);
	}
}

void AMainCharacter::CharacterTakeDamage(float damage)
{
	float actualDamage;
	actualDamage = PlayerStatComponent->TakeDamage(damage);

	UE_LOG(LogTemp, Warning, TEXT("Player Take Damage of %f"), actualDamage);
}

void AMainCharacter::SetWeapon(UStaticMesh* weaponMesh)
{
	WeaponMesh->SetStaticMesh(weaponMesh);
}
