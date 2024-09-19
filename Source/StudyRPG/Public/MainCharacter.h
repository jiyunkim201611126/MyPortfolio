#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Component/InventoryOwnerInterface.h"
#include "InteractionInterface.h"
#include "UI/MainHUD.h"
#include "MainCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class UNameTagWidgetComponent;
class UNameTagWidget;
class UActorComponent;
class UInventoryComponent;
class UTimelineComponent;
struct FInputActionValue;

USTRUCT()
struct FInteractionData
{
	GENERATED_USTRUCT_BODY()

	FInteractionData() : CurrentInteractable(nullptr), lastInteractionCheckTime(0.0f)
	{

	};

	UPROPERTY()
	AActor* CurrentInteractable;

	UPROPERTY()
	float lastInteractionCheckTime;
};

UCLASS()
class STUDYRPG_API AMainCharacter : public ACharacter, public IInventoryOwnerInterface
{
	GENERATED_BODY()

public:
	bool bAiming;

	// 생성자
	AMainCharacter();
	// 카메라 붐 반환
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return cameraBoom; }
	// 카메라 반환
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return followCamera; }

	FORCEINLINE bool IsInteracting() const { return GetWorldTimerManager().IsTimerActive(TimerHandle_Interaction); };

	FORCEINLINE class AMainHUD* GetMainHUD() const { return HUD; }

	// 플레이어 이름 변경
	void ChangePlayerNameTag(FText name);

	// Pickup했을 때 위젯 업데이트
	void UpdateInteractionWidget() const;

	virtual void DropItem(UItemBase* ItemToDrop, const int32 QuantityToDrop) override;
	virtual int32 ChangeItemWithOtherInventory(UItemBase* ChangeFrom, UItemBase* ChangeTo, bool check, int32 amount) override;

	void ToggleOtherInventory();
	void DisplayInputNumberToDropWidget(UItemBase* FromItem, IInventoryOwnerInterface* FromOwner, UItemBase* ToItem, IInventoryOwnerInterface* ToOwner);
	void DisplayInputNumberToSplitWidget(UItemBase* ItemToSplit);
	virtual void DisplayToChangeItem(UItemBase* ChangeFrom, UItemBase* ChangeTo) override;
	virtual void SplitItem(UItemBase* ItemToSplit, const int32 QuantityToSplit) override;

protected:

	float walkSpeed = 200.f;
	float runSpeed = 500.f;
	float aimingSpeed = 200.f;

	UPROPERTY()
	AMainHUD* HUD;

	// 카메라 붐
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* cameraBoom;

	// 캐릭터를 따라다니는 카메라
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* followCamera;

	UPROPERTY(VisibleAnywhere, Category = "Character | Interaction")
	TScriptInterface<IInteractionInterface> targetInteractable;

	UPROPERTY(VisibleAnywhere, Category = "Character | Inventory")
	UInventoryComponent* PlayerInventory;

	float interactionCheckFrequency;
	float interactionCheckDistance;
	FTimerHandle TimerHandle_Interaction;
	FInteractionData InteractionData;

	UPROPERTY(VisibleAnyWhere, Category = "Character | Camera")
	FVector DefaultCameraLocation;
	UPROPERTY(VisibleAnyWhere, Category = "Character | Camera")
	FVector AimingCameraLocation;

	TObjectPtr<UTimelineComponent> AimingCameraTimeline;

	UPROPERTY(EditDefaultsOnly, Category = "Character | Aim Timeline")
	UCurveFloat* AimingCameraCurve;

	// 인풋 액션 가상함수
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void BeginPlay() override;

	// 무브먼트 인풋을 통해 실행됨
	void Move(const FInputActionValue& Value);

	void Run();
	void StopRunning();

	// 마우스 움직임을 통해 실행됨
	void Look(const FInputActionValue& Value);

	void Aim();
	void StopAiming();
	UFUNCTION()
	void UpdateCameraTimeline(const float TimelineValue) const;
	UFUNCTION()
	void CameraTimelineEnd();

	void ToggleInventory();
	void PerformInteractionCheck();
	void FoundInteractable(AActor* NewInteractable);
	void NoInteractableFound();
	void BeginInteract();
	void EndInteract();
	void Interact();

	virtual void Tick(float DeltaSeconds) override;

private:
	// AllowPrivateAccess = "true"로 접근제한자가 private지만 에디터에서 볼 수 있음
	// 인풋 액션
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* defaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* jumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* moveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* run;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* lookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* aim;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* interactAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* toggleInventoryAction;

	// 네임태그 표시를 위한 위젯 컴포넌트
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = NameTag, meta = (AllowPrivateAccess = "true"))
	UNameTagWidgetComponent* nameTagWidgetComponent;

};
