#include "ItemBase.h"
#include "Component/InventoryComponent.h"

UItemBase::UItemBase() : bIsCopy(false), bIsPickup(false)
{
	isValidItem = true;
}

void UItemBase::ResetItemFlags()
{
	isValidItem = true;
	bIsCopy = false;
	bIsPickup = false;
}

void UItemBase::InitializeItemBase(const FItemData* ItemData, const int32 InQuantity)
{
	this->isValidItem = true;
	this->ID = ItemData->ID;
	this->ItemType = ItemData->ItemType;
	this->ItemQuality = ItemData->ItemQuality;
	this->NumericData = ItemData->NumericData;
	this->TextData = ItemData->TextData;
	this->AssetData = ItemData->AssetData;

	this->NumericData.bIsStackable = ItemData->NumericData.MaxStackSize > 1;
	InQuantity <= 0 ? this->SetQuantity(1) : this->SetQuantity(InQuantity);
}

UItemBase* UItemBase::CreateItemCopy() const
{
	UItemBase* itemCopy = NewObject<UItemBase>(StaticClass());

	itemCopy->isValidItem = true;
	itemCopy->ID = this->ID;
	itemCopy->Quantity = this->Quantity;
	itemCopy->ItemQuality = this->ItemQuality;
	itemCopy->ItemType = this->ItemType;
	itemCopy->TextData = this->TextData;
	itemCopy->NumericData = this->NumericData;
	itemCopy->ItemStatistics = this->ItemStatistics;
	itemCopy->AssetData = this->AssetData;

	itemCopy->bIsCopy = true;

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
}
