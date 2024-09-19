#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MainHUD.generated.h"

struct FInteractableData;
class UInteractionWidget;
class UInventoryMenu;
class UInventoryComponent;
class IInventoryOwnerInterface;
class UItemBase;

UCLASS()
class STUDYRPG_API AMainHUD : public AHUD
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UInventoryMenu> InventoryMenuClass;

	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UInteractionWidget> InteractionWidgetClass;

	UPROPERTY()
	UInventoryMenu* InventoryMenuWidget;

	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UUserWidget> CrosshairWidgetClass;

	bool bIsInventoryVisible;
	bool bIsOtherInventoryVisible;

	AMainHUD();

	void DisplayInventory();
	void HideInventory();
	void ToggleInventory();
	void DisplayOtherInventory();
	void HideOtherInventory();
	void ToggleOtherInventory();
	void DisplayInputNumberToDropWidget(UItemBase* FromItem, IInventoryOwnerInterface* FromOwner, UItemBase* ToItem, IInventoryOwnerInterface* ToOwner);
	void DisplayInputNumberToSplitWidget(UItemBase* ItemToSplit, IInventoryOwnerInterface* ItemOwner);

	void ShowCrosshair();
	void HideCrosshair();

	void ShowInteractionWidget() const;
	void HideInteractionWidget() const;
	void UpdateInteractionWidget(const FInteractableData* InteractableData) const;

	FORCEINLINE class UInventoryMenu* GetPlayerInventoryMenuWidget() { return InventoryMenuWidget; };

	void SetPlayerInventory(UInventoryComponent* OwnerInventory);
	void SetOtherInventory(UInventoryComponent* OwnerInventory);

protected:

	UPROPERTY()
	UInteractionWidget* InteractionWidget;

	UPROPERTY()
	UUserWidget* CrosshairWidget;

	virtual void BeginPlay() override;
};
