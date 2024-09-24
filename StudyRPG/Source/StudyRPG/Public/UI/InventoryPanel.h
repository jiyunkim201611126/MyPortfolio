#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryPanel.generated.h"

class UTextBlock;
class UWrapBox;
class UInventoryComponent;
class UInventoryItemSlot;
class UItemBase;

UCLASS()
class STUDYRPG_API UInventoryPanel : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION()
	void RefreshInventory();

	void SetInventoryComponent(UInventoryComponent* OwnerInventory);

	UPROPERTY(meta = (BindWidget))
	UTextBlock* InventoryName;

	UPROPERTY(meta = (BindWidget))
	UWrapBox* InventoryPanel;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* WeightInfo;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* CapacityInfo;

	UPROPERTY()
	UInventoryComponent* InventoryReference;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UInventoryItemSlot> InventorySlotClass;

protected:
	void SetInfoText() const;
	void GenerateStartSlots();
	virtual void NativeOnInitialized() override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
};
