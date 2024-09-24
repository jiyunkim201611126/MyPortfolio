#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryMenu.generated.h"

class UInventoryPanel;
class UInventoryComponent;
class UInputNumberToDropItemWidget;
class UItemBase;
class IInventoryOwnerInterface;

UCLASS()
class STUDYRPG_API UInventoryMenu : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget))
	UInputNumberToDropItemWidget* InputNumberToDropItemWidget;

	UPROPERTY(meta = (BindWidget))
	UInventoryPanel* PlayerInventoryPanel;

	UPROPERTY(meta = (BindWidget))
	UInventoryPanel* OtherInventoryPanel;

	void SetPlayerInventoryPanelComponent(UInventoryComponent* OwnerInventory);
	void SetOtherInventoryPanelComponent(UInventoryComponent* OwnerInventory);
	void DisplayOtherInventory();
	void HideOtherInventory();
	void SetItemToChange(UItemBase* FromItem, IInventoryOwnerInterface* FromOwner, UItemBase* ToItem, IInventoryOwnerInterface* ToOwner, UUserWidget* HaveItemSlot);
	void SetItemToSplit(UItemBase* ItemToSplit, IInventoryOwnerInterface* ItemOwner, UUserWidget* HaveItemSlot);

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
};
