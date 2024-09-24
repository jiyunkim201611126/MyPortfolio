#include "UI/MainHUD.h"
#include "UI/InventoryMenu.h"
#include "UI/InteractionWidget.h"
#include "UI/Stat/StatWidget.h"
#include "Component/InventoryOwnerInterface.h"
#include "ItemBase.h"

AMainHUD::AMainHUD()
{
}

void AMainHUD::BeginPlay()
{
	Super::BeginPlay();

	// 디폴트 패널에서 설정해준 WBP를 통해 위젯 생성 후 화면에 생성, 일단 숨겨둠
	if (InventoryMenuClass)
	{
		InventoryMenuWidget = CreateWidget<UInventoryMenu>(GetWorld(), InventoryMenuClass);
		InventoryMenuWidget->AddToViewport(4);
		InventoryMenuWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
	if (InteractionWidgetClass)
	{
		InteractionWidget = CreateWidget<UInteractionWidget>(GetWorld(), InteractionWidgetClass);
		InteractionWidget->AddToViewport(-1);
		InteractionWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
	if (CrosshairWidgetClass)
	{
		CrosshairWidget = CreateWidget<UUserWidget>(GetWorld(), CrosshairWidgetClass);
		CrosshairWidget->AddToViewport(0);
		CrosshairWidget->SetVisibility(ESlateVisibility::Visible);
	}
	if (StatWidgetClass)
	{
		StatWidget = CreateWidget<UStatWidget>(GetWorld(), StatWidgetClass);
		StatWidget->AddToViewport(-1);
		StatWidget->SetVisibility(ESlateVisibility::Visible);
	}
}

void AMainHUD::DisplayInventory()
{
	if (InventoryMenuWidget)
	{
		bIsInventoryVisible = true;
		InventoryMenuWidget->SetVisibility(ESlateVisibility::Visible);
		InventoryMenuWidget->HideOtherInventory();
	}
}

void AMainHUD::HideInventory()
{
	if (InventoryMenuWidget)
	{
		GetOwningPlayerController()->SetIgnoreMoveInput(false);
		bIsInventoryVisible = false;
		InventoryMenuWidget->SetVisibility(ESlateVisibility::Collapsed);
		InventoryMenuWidget->HideOtherInventory();
	}
}

void AMainHUD::ToggleInventory()
{
	bIsOtherInventoryVisible = false;

	if (bIsInventoryVisible)
	{
		HideInventory();

		const FInputModeGameOnly InputMode;
		GetOwningPlayerController()->SetInputMode(InputMode);
		GetOwningPlayerController()->SetShowMouseCursor(false);
	}
	else
	{
		DisplayInventory();

		const FInputModeGameAndUI InputMode;
		GetOwningPlayerController()->SetInputMode(InputMode);
		GetOwningPlayerController()->SetShowMouseCursor(true);
	}
}

void AMainHUD::DisplayOtherInventory()
{
	if (InventoryMenuWidget)
	{
		GetOwningPlayerController()->SetIgnoreMoveInput(true);
		bIsInventoryVisible = true;
		bIsOtherInventoryVisible = true;
		InventoryMenuWidget->SetVisibility(ESlateVisibility::Visible);
		InventoryMenuWidget->DisplayOtherInventory();
	}
}

void AMainHUD::HideOtherInventory()
{
	if (InventoryMenuWidget)
	{
		GetOwningPlayerController()->SetIgnoreMoveInput(false);
		bIsInventoryVisible = false;
		bIsOtherInventoryVisible = false;
		InventoryMenuWidget->SetVisibility(ESlateVisibility::Collapsed);
		InventoryMenuWidget->HideOtherInventory();
	}
}

void AMainHUD::ToggleOtherInventory()
{
	if (bIsOtherInventoryVisible)
	{
		HideOtherInventory();

		const FInputModeGameOnly InputMode;
		GetOwningPlayerController()->SetInputMode(InputMode);
		GetOwningPlayerController()->SetShowMouseCursor(false);
	}
	else
	{
		DisplayOtherInventory();

		const FInputModeGameAndUI InputMode;
		GetOwningPlayerController()->SetInputMode(InputMode);
		GetOwningPlayerController()->SetShowMouseCursor(true);
	}
}

void AMainHUD::DisplayInputNumberToDropWidget(UItemBase* FromItem, IInventoryOwnerInterface* FromOwner, UItemBase* ToItem, IInventoryOwnerInterface* ToOwner, UUserWidget* HaveItemSlot)
{
	InventoryMenuWidget->SetItemToChange(FromItem, FromOwner, ToItem, ToOwner, HaveItemSlot);
}

void AMainHUD::DisplayInputNumberToSplitWidget(UItemBase* ItemToSplit, IInventoryOwnerInterface* ItemOwner, UUserWidget* HaveItemSlot)
{
	InventoryMenuWidget->SetItemToSplit(ItemToSplit, ItemOwner, HaveItemSlot);
}

void AMainHUD::ShowCrosshair()
{
	if (CrosshairWidget)
		CrosshairWidget->SetVisibility(ESlateVisibility::Visible);
}

void AMainHUD::HideCrosshair()
{
	if (CrosshairWidget)
		CrosshairWidget->SetVisibility(ESlateVisibility::Collapsed);
}

void AMainHUD::ShowInteractionWidget() const
{
	if (InteractionWidget)
	{
		InteractionWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void AMainHUD::HideInteractionWidget() const
{
	if (InteractionWidget)
	{
		InteractionWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void AMainHUD::UpdateInteractionWidget(const FInteractableData* InteractableData) const
{
	if (InteractionWidget)
	{
		if (InteractionWidget->GetVisibility() == ESlateVisibility::Collapsed)
		{
			InteractionWidget->SetVisibility(ESlateVisibility::Visible);
		}

		InteractionWidget->UpdateWidget(InteractableData);
	}
}

void AMainHUD::SetPlayerInventory(UInventoryComponent* OwnerInventory)
{
	InventoryMenuWidget->SetPlayerInventoryPanelComponent(OwnerInventory);
}

void AMainHUD::SetOtherInventory(UInventoryComponent* OwnerInventory)
{
	InventoryMenuWidget->SetOtherInventoryPanelComponent(OwnerInventory);
}

void AMainHUD::SetPlayerStatComponent(UStatComponent* OwnerStatComponent)
{
	StatWidget->SetStatComponent(OwnerStatComponent);
}
