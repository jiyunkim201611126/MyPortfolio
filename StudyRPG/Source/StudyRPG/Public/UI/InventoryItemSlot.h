#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryItemSlot.generated.h"

class UBorder;
class UImage;
class UTextBlock;
class UInventoryTooltip;
class UDragItemVisual;
class UItemBase;

UCLASS()
class STUDYRPG_API UInventoryItemSlot : public UUserWidget
{
	GENERATED_BODY()

public:

	FORCEINLINE void SetItemReference(UItemBase* ItemIn) { ItemReference = ItemIn; };
	FORCEINLINE UItemBase* GetItemReference() const { return ItemReference; };

	void SetIsCanClick(bool check);

	void RefreshIconAndTooltip();
	void ResetIconAndTooltip();
	int32 GetItemQuantityText();

protected:

	bool isCanClick = true;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory Slot")
	TSubclassOf<UDragItemVisual> DragItemVisualClass;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory Slot")
	TSubclassOf<UInventoryTooltip> TooltipClass;

	UPROPERTY(VisibleAnywhere, Category = "Inventory Slot")
	UItemBase* ItemReference;

	UPROPERTY(VisibleAnywhere, Category = "Inventory Slot", meta = (BindWidget))
	UBorder* ItemBorder;

	UPROPERTY(VisibleAnywhere, Category = "Inventory Slot", meta = (BindWidget))
	UImage* ItemIcon;

	UPROPERTY(VisibleAnywhere, Category = "Inventory Slot", meta = (BindWidget))
	UTextBlock* ItemQuantity;

	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

};
