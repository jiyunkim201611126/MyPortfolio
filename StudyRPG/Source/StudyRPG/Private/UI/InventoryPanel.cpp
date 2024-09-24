#include "UI/InventoryPanel.h"
#include "UI/InventoryItemSlot.h"
#include "Component/InventoryComponent.h"
#include "Components/TextBlock.h"
#include "Components/WrapBox.h"
#include "ItemBase.h"

void UInventoryPanel::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

void UInventoryPanel::SetInventoryComponent(UInventoryComponent* OwnerInventory)
{
	if (InventoryReference != OwnerInventory)
	{
		InventoryReference = OwnerInventory;

		if (InventoryReference)
		{
			// InventoryComponent의 멀티캐스트 델리게이트에 등록된 함수 제거 후 이 클래스의 RefreshInventory 등록
			InventoryReference->OnInventoryUpdated.RemoveAll(this);
			InventoryReference->OnInventoryUpdated.AddUObject(this, &UInventoryPanel::RefreshInventory);
			GenerateStartSlots();
		}
	}
}

// 인벤토리 상단 이름, 하단의 무게와 칸 새로고침
void UInventoryPanel::SetInfoText() const
{
	const FText InventoryNameText = InventoryReference->GetInventoryName();

	const FString WeightInfoValue{
		FString::SanitizeFloat(InventoryReference->GetInventoryTotalWeight()) + " / "
		+ FString::SanitizeFloat(InventoryReference->GetWeightCapacity())
	};

	const FString CapacityInfoValue{
		FString::FromInt(InventoryReference->GetInventoryNumberOfItems()) + " / "
		+ FString::FromInt(InventoryReference->GetSlotsCapacity())
	};

	InventoryName->SetText(InventoryNameText);
	WeightInfo->SetText(FText::FromString(WeightInfoValue));
	CapacityInfo->SetText(FText::FromString(CapacityInfoValue));
}

void UInventoryPanel::GenerateStartSlots()
{
	InventoryPanel->ClearChildren();

	if (InventoryReference && InventorySlotClass)
	{
		for (UItemBase* const& InventoryItem : InventoryReference->GetInventoryContents())
		{
			UInventoryItemSlot* ItemSlot = CreateWidget<UInventoryItemSlot>(this, InventorySlotClass);

			ItemSlot->SetItemReference(InventoryItem);

			InventoryPanel->AddChildToWrapBox(ItemSlot);
		}
		RefreshInventory();
	}
}

// 인벤토리 새로고침, InventoryComponent에서 아이템 습득, 드랍 등의 일이 있을 때 델리게이트로 불러줌
void UInventoryPanel::RefreshInventory()
{
	if (InventoryReference)
	{
		int counter = 0;

		for (UItemBase* const InventoryItem : InventoryReference->GetInventoryContents())
		{
			// InventoryContents와 InventoryPanel의 동일 인덱스 원소와 자손이 다른 경우
			// 혹은 적힌 QuantityText와 실제 Quantity가 일치하지 않으면 Refresh

			UInventoryItemSlot* ItemSlot = Cast<UInventoryItemSlot>(InventoryPanel->GetChildAt(counter));

			if (ItemSlot->GetItemReference() != InventoryItem)
			{
				ItemSlot->SetItemReference(InventoryItem);
				ItemSlot->RefreshIconAndTooltip();
			}
			else if (ItemSlot->GetItemReference())
			{
				if (ItemSlot->GetItemReference()->Quantity != ItemSlot->GetItemQuantityText())
				{
					ItemSlot->RefreshIconAndTooltip();
				}
			}
			counter++;
		}
		
		SetInfoText();
	}
}

bool UInventoryPanel::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	return false;
}
