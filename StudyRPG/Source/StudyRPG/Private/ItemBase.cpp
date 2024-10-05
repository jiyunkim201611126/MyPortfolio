#include "ItemBase.h"
#include "Component/InventoryComponent.h"

UItemBase::UItemBase() : bIsPickup(false)
{
	isValidItem = true;
}

void UItemBase::ResetItemFlags()
{
	isValidItem = true;
	bIsPickup = false;
}

void UItemBase::InitializeItemBase(const FItemData* ItemData, const int32 InQuantity)
{
	this->isValidItem = true;
	this->ID = ItemData->ID;
	this->ItemType = ItemData->ItemType;
	this->ItemQuality = ItemData->ItemQuality;
	this->ItemStatistics = ItemData->ItemStatistics;
	this->TextData = ItemData->TextData;
	this->NumericData = ItemData->NumericData;
	this->AssetData = ItemData->AssetData;
	this->ItemEffectData = ItemData->ItemEffectData;

	this->NumericData.bIsStackable = ItemData->NumericData.MaxStackSize > 1;
	InQuantity <= 0 ? this->SetQuantity(1) : this->SetQuantity(InQuantity);
}

UItemBase* UItemBase::CreateItemCopy() const
{
	UItemBase* itemCopy = NewObject<UItemBase>(StaticClass());

	itemCopy->isValidItem = true;
	itemCopy->ID = this->ID;
	itemCopy->ItemType = this->ItemType;
	itemCopy->ItemQuality = this->ItemQuality;
	itemCopy->ItemStatistics = this->ItemStatistics;
	itemCopy->TextData = this->TextData;
	itemCopy->NumericData = this->NumericData;
	itemCopy->AssetData = this->AssetData;
	itemCopy->ItemEffectData = this->ItemEffectData;

	itemCopy->Quantity = this->Quantity;

	return itemCopy;
}

void UItemBase::SetQuantity(const int32 newQuantity)
{
	if (newQuantity != Quantity)
	{
		Quantity = FMath::Clamp(newQuantity, 0, NumericData.bIsStackable ? NumericData.MaxStackSize : 1);
	}
}

void UItemBase::Use(AMainCharacter* character)
{
	// 다른 인벤토리에 있는 아이템은 사용 못 하게 하고,
	// 착용, 소모 등 사용할 수 있는 아이템만 들어갈 수 있게 거름
	if (OwningInventory == character->GetInventory()
		&& (ItemType == EItemType::Consumable || 
			ItemType == EItemType::Armor || 
			ItemType == EItemType::Weapon))
	{
		switch (ItemType)
		{
		case EItemType::Consumable:
			character->UseItem(this);
			break;
		case EItemType::Armor:
			break;
		case EItemType::Weapon:
			character->UseItem(this);
			break;
		}


		OwningInventory->RemoveAmountOfItem(this, 1);
	}
}
