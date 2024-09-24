#include "UI/InventoryTooltip.h"
#include "UI/InventoryItemSlot.h"
#include "Components/TextBlock.h"

#include "ItemBase.h"

void UInventoryTooltip::NativeConstruct()
{
	Super::NativeConstruct();

	UItemBase* ItemBeingHovered = InventorySlotBeingHovered->GetItemReference();

	if (!ItemBeingHovered->isValidItem)
		return;

	switch (ItemBeingHovered->ItemType)
	{
	case EItemType::Armor:
		break;
	case EItemType::Weapon:
		break;
	case EItemType::Shield:
		break;
	case EItemType::Spell:
		break;
	case EItemType::Consumable:
		ItemType->SetText(FText::FromString("Consumable"));
		DamageValue->SetVisibility(ESlateVisibility::Collapsed);
		ArmorRating->SetVisibility(ESlateVisibility::Collapsed);
		break;
	case EItemType::Quest:
		break;
	case EItemType::Mundane:
		ItemType->SetText(FText::FromString("Mundane Item"));
		DamageValue->SetVisibility(ESlateVisibility::Collapsed);
		ArmorRating->SetVisibility(ESlateVisibility::Collapsed);
		UsageText->SetVisibility(ESlateVisibility::Collapsed);
		break;
	default:;
	}

	ItemName->SetText(ItemBeingHovered->TextData.Name);
	DamageValue->SetText(FText::AsNumber(ItemBeingHovered->ItemStatistics.DamageValue));
	ArmorRating->SetText(FText::AsNumber(ItemBeingHovered->ItemStatistics.ArmorRating));
	UsageText->SetText(ItemBeingHovered->TextData.UsageText);
	ItemDescription->SetText(ItemBeingHovered->TextData.Description);

	const FString WeightInfo = { "Weight: " + FString::SanitizeFloat(ItemBeingHovered->GetItemStackWeight()) };
	StackWeight->SetText(FText::FromString(WeightInfo));

	if (ItemBeingHovered->NumericData.bIsStackable)
	{
		const FString StackInfo = { "Max Stack Size: " + FString::FromInt(ItemBeingHovered->NumericData.MaxStackSize) };
		MaxStackSize->SetText(FText::FromString(StackInfo));
	}
	else
	{
		MaxStackSize->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UInventoryTooltip::SetItemQualityText(FText qualityText)
{
	ItemQuality->SetText(qualityText);
}
