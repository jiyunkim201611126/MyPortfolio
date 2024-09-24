#include "UI/InventoryMenu.h"
#include "UI/ItemDragDropOperation.h"
#include "UI/InventoryPanel.h"
#include "UI/InputNumberToDropItemWidget.h"
#include "UI/InventoryItemSlot.h"

#include "ItemBase.h"

#include "Component/InventoryComponent.h"
#include "Component/InventoryOwnerInterface.h"

void UInventoryMenu::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

void UInventoryMenu::NativeConstruct()
{
	Super::NativeConstruct();

	OtherInventoryPanel->SetVisibility(ESlateVisibility::Collapsed);
	InputNumberToDropItemWidget->SetVisibility(ESlateVisibility::Collapsed);
}

// 드래그 후 드랍이 감지되면 실행. 드래그는 InventoryItemSlot이 직접 감지함.
bool UInventoryMenu::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	// 매개변수로 들어온 DragDropOperation을 ItemDragDropOperation으로 캐스팅
	const UItemDragDropOperation* ItemDragDrop = Cast<UItemDragDropOperation>(InOperation);

	IInventoryOwnerInterface* InventoryOwner = Cast<IInventoryOwnerInterface>(ItemDragDrop->SourceInventory->GetOwner());

	if (InventoryOwner && ItemDragDrop->SourceItem)
	{
		int32 ItemToDropQuantity = ItemDragDrop->SourceItem->Quantity;

		if (ItemToDropQuantity > 1)
		{
			ItemDragDrop->HaveItemSlot->SetIsCanClick(false);
			InputNumberToDropItemWidget->SetItemToDrop(ItemDragDrop->SourceItem, InventoryOwner, ItemDragDrop->HaveItemSlot);
			InputNumberToDropItemWidget->inputText->SetText(FText::FromString(FString::FromInt(ItemToDropQuantity)));
			InputNumberToDropItemWidget->SetVisibility(ESlateVisibility::Visible);

			return true;
		}

		// ItemDragDropOperation의 Item 정보를 통해 월드상에 Pickup 인스턴스 생성
		InventoryOwner->DropItem(ItemDragDrop->SourceItem, ItemToDropQuantity);

		return true;
	}
	return false;
}

void UInventoryMenu::SetPlayerInventoryPanelComponent(UInventoryComponent* OwnerInventory)
{
	PlayerInventoryPanel->SetInventoryComponent(OwnerInventory);
}

void UInventoryMenu::SetOtherInventoryPanelComponent(UInventoryComponent* OwnerInventory)
{
	OtherInventoryPanel->SetInventoryComponent(OwnerInventory);
}

void UInventoryMenu::DisplayOtherInventory()
{
	OtherInventoryPanel->SetVisibility(ESlateVisibility::Visible);
}

void UInventoryMenu::HideOtherInventory()
{
	OtherInventoryPanel->SetVisibility(ESlateVisibility::Collapsed);
}

void UInventoryMenu::SetItemToChange(UItemBase* FromItem, IInventoryOwnerInterface* FromOwner, UItemBase* ToItem, IInventoryOwnerInterface* ToOwner, UUserWidget* HaveItemSlot)
{
	InputNumberToDropItemWidget->SetItemToChange(FromItem, FromOwner, ToItem, ToOwner, HaveItemSlot);
	InputNumberToDropItemWidget->inputText->SetText(FText::FromString(FString::FromInt(FromItem->Quantity)));
	InputNumberToDropItemWidget->SetVisibility(ESlateVisibility::Visible);
}

void UInventoryMenu::SetItemToSplit(UItemBase* ItemToSplit, IInventoryOwnerInterface* ItemOwner, UUserWidget* HaveItemSlot)
{
	InputNumberToDropItemWidget->SetItemToSplit(ItemToSplit, ItemOwner, HaveItemSlot);
	InputNumberToDropItemWidget->inputText->SetText(FText::FromString(FString::FromInt(ItemToSplit->Quantity)));
	InputNumberToDropItemWidget->SetVisibility(ESlateVisibility::Visible);
}
