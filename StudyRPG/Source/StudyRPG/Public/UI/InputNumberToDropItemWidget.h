#pragma once

#include "CoreMinimal.h"
#include "UI/InputTextPopupWidget.h"
#include "ItemBase.h"
#include "Component/InventoryOwnerInterface.h"
#include "UI/InventoryItemSlot.h"
#include "InputNumberToDropItemWidget.generated.h"

UENUM()
enum class EWhatToDo : uint8
{
	Drop,
	Change,
	Split,
};

UCLASS()
class STUDYRPG_API UInputNumberToDropItemWidget : public UInputTextPopupWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeOnInitialized();

	int32 numberToDrop = 0;
	EWhatToDo WhatToDo;

	// 취소 버튼
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (BindWidget))
	UButton* cancelButton;

	UItemBase* Item1;
	IInventoryOwnerInterface* ItemOwner1;
	UItemBase* Item2;
	IInventoryOwnerInterface* ItemOwner2;
	UInventoryItemSlot* ItemSlot;

public:
	UFUNCTION()
	void SendNumberToDrop();
	UFUNCTION()
	void ResetPopup();

	void SetItemToDrop(UItemBase* Item, IInventoryOwnerInterface* Owner, UUserWidget* HaveItemSlot);
	void SetItemToChange(UItemBase* FromItem, IInventoryOwnerInterface* FromOwner, UItemBase* ToItem, IInventoryOwnerInterface* ToOwner, UUserWidget* HaveItemSlot);
	void SetItemToSplit(UItemBase* ItemToSplit, IInventoryOwnerInterface* ItemOwner, UUserWidget* HaveItemSlot);
};
