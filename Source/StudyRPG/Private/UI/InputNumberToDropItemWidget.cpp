#include "UI/InputNumberToDropItemWidget.h"
#include "Misc/DefaultValueHelper.h"

void UInputNumberToDropItemWidget::SendNumberToDrop()
{
	if (!FDefaultValueHelper::ParseInt(GetInputText().ToString(), numberToDrop))
	{
		ResetPopup();
		return;
	}

	switch (WhatToDo)
	{
	case EWhatToDo::Drop:
		ItemOwner1->DropItem(Item1, numberToDrop);
		break;
	case EWhatToDo::Change:
		// 3번째 매개변수는 쌓을 수 있는 같은 아이템에서 쓰임.
		// false면 드래그를 감지한 아이템에서 뺄 수 있는 만큼 빼고, 빠진 만큼 AddedAmount에 할당함.
		// 그 후 AddedAmount를 매개변수로 넣어서 드랍을 감지한 아이템에 더해줌
		int32 AddedAmount;
		AddedAmount = ItemOwner1->ChangeItemWithOtherInventory(Item1, Item2, true, numberToDrop);

		if (AddedAmount != 0)
			ItemOwner2->ChangeItemWithOtherInventory(Item2, Item1, false, AddedAmount);
		break;
	case EWhatToDo::Split:
		if (numberToDrop != Item1->Quantity)
			ItemOwner1->SplitItem(Item1, numberToDrop);
		break;
	}

	ResetPopup();
}

void UInputNumberToDropItemWidget::ResetPopup()
{
	numberToDrop = 0;
	Item1 = nullptr;
	ItemOwner1 = nullptr;
	Item2 = nullptr;
	ItemOwner2 = nullptr;
	this->SetVisibility(ESlateVisibility::Collapsed);
}

void UInputNumberToDropItemWidget::SetItemToDrop(UItemBase* Item, IInventoryOwnerInterface* Owner)
{
	WhatToDo = EWhatToDo::Drop;
	Item1 = Item;
	ItemOwner1 = Owner;
}

void UInputNumberToDropItemWidget::SetItemToChange(UItemBase* FromItem, IInventoryOwnerInterface* FromOwner, UItemBase* ToItem, IInventoryOwnerInterface* ToOwner)
{
	WhatToDo = EWhatToDo::Change;
	Item1 = FromItem;
	ItemOwner1 = FromOwner;
	Item2 = ToItem;
	ItemOwner2 = ToOwner;
}

void UInputNumberToDropItemWidget::SetItemToSplit(UItemBase* ItemToSplit, IInventoryOwnerInterface* ItemOwner)
{
	WhatToDo = EWhatToDo::Split;
	Item1 = ItemToSplit;
	ItemOwner1 = ItemOwner;
}

void UInputNumberToDropItemWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	confirmButton = Cast<UButton>(GetWidgetFromName(TEXT("confirmButton")));

	confirmButton->OnClicked.AddDynamic(this, &UInputNumberToDropItemWidget::SendNumberToDrop);

	cancelButton = Cast<UButton>(GetWidgetFromName(TEXT("cancelButton")));

	cancelButton->OnClicked.AddDynamic(this, &UInputNumberToDropItemWidget::ResetPopup);

	this->SetVisibility(ESlateVisibility::Collapsed);
}