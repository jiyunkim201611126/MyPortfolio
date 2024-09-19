#include "UI/InventoryItemSlot.h"
#include "UI/InventoryTooltip.h"
#include "UI/DragItemVisual.h"
#include "UI/ItemDragDropOperation.h"

#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/Border.h"

#include "Component/InventoryComponent.h"
#include "Component/InventoryOwnerInterface.h"

#include "ItemBase.h"

void UInventoryItemSlot::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

void UInventoryItemSlot::NativeConstruct()
{
	Super::NativeConstruct();

	RefreshIconAndTooltip();
}

void UInventoryItemSlot::RefreshIconAndTooltip()
{
	if (ItemReference)
	{
		if (!ItemReference->isValidItem)
		{
			ResetIconAndTooltip();
			return;
		}

		UInventoryTooltip* ToolTip = CreateWidget<UInventoryTooltip>(this, TooltipClass);

		if (TooltipClass)
		{
			ToolTip->InventorySlotBeingHovered = this;
			// UWidget에서 제공하는 함수, 마우스를 올리면 매개변수로 전달된 위젯이 표시됨
			SetToolTip(ToolTip);
		}

		switch (ItemReference->ItemQuality)
		{
		case EItemQuality::Shoddy:
			ItemBorder->SetBrushColor(FLinearColor(0.25f, 0.25f, 0.25f, 1.0f)); // 회색
			ToolTip->ItemQuality->SetColorAndOpacity(FLinearColor(0.25f, 0.25f, 0.25f, 1.0f));
			ToolTip->SetItemQualityText(FText::FromString("Shoddy"));
			break;
		case EItemQuality::Common:
			ItemBorder->SetBrushColor(FLinearColor(0.f, 0.2f, 0.8f, 1.0f)); // 연파랑
			ToolTip->ItemQuality->SetColorAndOpacity(FLinearColor(0.f, 0.2f, 0.8f, 1.0f));
			ToolTip->SetItemQualityText(FText::FromString("Common"));
			break;
		case EItemQuality::Quality:
			ItemBorder->SetBrushColor(FLinearColor(0.55f, 0.05f, 1.f, 1.0f)); // 연보라
			ToolTip->ItemQuality->SetColorAndOpacity(FLinearColor(0.55f, 0.05f, 1.f, 1.0f));
			ToolTip->SetItemQualityText(FText::FromString("Quality"));
			break;
		case EItemQuality::Epic:
			ItemBorder->SetBrushColor(FLinearColor(1.f, 0.4f, 0.15f, 1.0f)); // 연주황
			ToolTip->ItemQuality->SetColorAndOpacity(FLinearColor(1.f, 0.4f, 0.15f, 1.0f));
			ToolTip->SetItemQualityText(FText::FromString("Epic"));
			break;
		case EItemQuality::Legend:
			ItemBorder->SetBrushColor(FLinearColor(1.f, 0.1f, 0.1f, 1.0f)); // 빨강
			ToolTip->ItemQuality->SetColorAndOpacity(FLinearColor(1.f, 0.1f, 0.1f, 1.0f));
			ToolTip->SetItemQualityText(FText::FromString("Legend"));
			break;
		default:;
		}

		ItemIcon->SetBrushFromTexture(ItemReference->AssetData.Icon);
		ItemIcon->SetColorAndOpacity(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f));

		if (ItemReference->NumericData.bIsStackable)
		{
			ItemQuantity->SetText(FText::AsNumber(ItemReference->Quantity));
			ItemQuantity->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			ItemQuantity->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

void UInventoryItemSlot::ResetIconAndTooltip()
{
	ItemBorder->SetBrushColor(FLinearColor(0.9f, 0.9f, 0.9f, 1.0f));
	SetToolTip(nullptr);
	ItemIcon->SetBrushFromTexture(nullptr);
	ItemIcon->SetColorAndOpacity(FLinearColor(0.025f, 0.025f, 0.025f, 1.0f));
	ItemQuantity->SetVisibility(ESlateVisibility::Collapsed);
}

int32 UInventoryItemSlot::GetItemQuantityText()
{
	return FCString::Atoi(*ItemQuantity->GetText().ToString());;
}

// 마우스 클릭 오버라이딩, FReply는 제대로 작동했을 때 Handled를, 아니면 Unhandled를 리턴한다
// FReply는 Slate UI(언리얼에서 제공하는 UI 시스템)에서 마우스와 키보드 입력의 처리에 쓰임.
FReply UInventoryItemSlot::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply Reply = Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);

	FKey WhatIsDown = InMouseEvent.GetEffectingButton();

	// 마우스 휠 클릭이 감지되면 아이템 나누기
	if (WhatIsDown == EKeys::MiddleMouseButton && ItemReference->isValidItem)
	{
		// 개수가 2개 이상이고, 빈 슬롯이 있는 경우
		if (ItemReference->Quantity > 1 && ItemReference->OwningInventory->GetInventoryNumberOfItems() < ItemReference->OwningInventory->GetSlotsCapacity())
		{
			AMainCharacter* PlayerCharacter = Cast<AMainCharacter>(GetOwningPlayerPawn());
			PlayerCharacter->DisplayInputNumberToSplitWidget(ItemReference);
		}

		return FReply::Handled();
	}
	// 마우스 우클릭이 감지되면
	else if (WhatIsDown == EKeys::RightMouseButton && ItemReference->isValidItem)
	{

	}
	// 마우스 좌클릭이 감지되면
	else if (WhatIsDown == EKeys::LeftMouseButton && ItemReference->isValidItem)
	{
		// 드래그 감지 시작
		return Reply.Handled().DetectDrag(TakeWidget(), EKeys::LeftMouseButton);
	}

	return Reply.Unhandled();
}

void UInventoryItemSlot::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
}

// 드래그가 감지됐을 때 실행되며, 내부에서 UDragDropOperation에 대해 초기화해주고 매개변수로 들어온 OutOperation에 할당해줌
void UInventoryItemSlot::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);

	// DragVisualClass를 갖고 있으면
	if (DragItemVisualClass)
	{
		// TObjectPtr<UDragItemVisual>은 UDragDropOperation의 변수이다.
		// ItemDragDropOperation가 갖고 있기 때문에 초기화시켜주기 위해 선언함
		const TObjectPtr<UDragItemVisual> DragVisual = CreateWidget<UDragItemVisual>(this, DragItemVisualClass);
		DragVisual->ItemIcon->SetBrushFromTexture(ItemReference->AssetData.Icon);
		DragVisual->ItemBorder->SetBrushColor(ItemBorder->GetBrushColor());
		
		if (ItemReference->NumericData.bIsStackable)
		{
			DragVisual->ItemQuantity->SetText(FText::AsNumber(ItemReference->Quantity));
		}
		else
		{
			DragVisual->ItemQuantity->SetVisibility(ESlateVisibility::Collapsed);
		}

		// 매개변수인 OutOperation의 초기화를 위해 선언
		UItemDragDropOperation* DragItemOperation = NewObject<UItemDragDropOperation>();
		DragItemOperation->SourceItem = ItemReference;
		DragItemOperation->SourceInventory = ItemReference->OwningInventory;

		DragItemOperation->DefaultDragVisual = DragVisual;
		DragItemOperation->Pivot = EDragPivot::TopLeft;

		// 최종적으로 매개변수로 들어온 OutOperation을 초기화해주면서 드래그 시 보여질 모습과 함께 Item 정보를 전달했음
		OutOperation = DragItemOperation;
	}
}

// 드래그 후 드랍이 감지되면 실행, false 반환 시 드랍을 감지한 다른 위젯도 드랍 이벤트가 실행되므로 주의
bool UInventoryItemSlot::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	// 매개변수로 들어온 DragDropOperation을 ItemDragDropOperation으로 캐스팅
	const UItemDragDropOperation* ItemDragDrop = Cast<UItemDragDropOperation>(InOperation);

	if (ItemDragDrop->SourceItem == ItemReference)
		return true;

	IInventoryOwnerInterface* FromInventoryOwner = Cast<IInventoryOwnerInterface>(ItemDragDrop->SourceInventory->GetOwner());
	IInventoryOwnerInterface* ToInventoryOwner = Cast<IInventoryOwnerInterface>(ItemReference->OwningInventory->GetOwner());

	if (FromInventoryOwner == ToInventoryOwner)
	{
		FromInventoryOwner->DisplayToChangeItem(ItemDragDrop->SourceItem, ItemReference);
		return true;
	}

	if (FromInventoryOwner && ItemDragDrop->SourceItem)
	{
		// From 아이템 개수가 1개가 아니면서
		// 빈 슬롯이거나, To 아이템이 꽉 차지 않은 경우
		if (ItemDragDrop->SourceItem->Quantity != 1
			&& (!ItemReference->isValidItem || (ItemDragDrop->SourceItem->ID == ItemReference->ID && ItemReference->Quantity < ItemReference->NumericData.MaxStackSize)))
		{
			AMainCharacter* PlayerCharacter = Cast<AMainCharacter>(GetOwningPlayerPawn());
			PlayerCharacter->DisplayInputNumberToDropWidget(ItemDragDrop->SourceItem, FromInventoryOwner, ItemReference, ToInventoryOwner);

			return true;
		}

		// 아이템 개수가 1개거나, 동일하지 않거나, 둘 중 하나라도 쌓을 수 없는 아이템인 경우
		int32 AddedAmount;
		AddedAmount = FromInventoryOwner->ChangeItemWithOtherInventory(ItemDragDrop->SourceItem, ItemReference, true, ItemDragDrop->SourceItem->Quantity);

		if (AddedAmount != 0)
			ToInventoryOwner->ChangeItemWithOtherInventory(ItemReference, ItemDragDrop->SourceItem, false, AddedAmount);

		return true;
	}
	return false;
}
