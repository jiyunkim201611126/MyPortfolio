#include "Component/InventoryOwnerInterface.h"
#include "Component/InventoryComponent.h"
#include "ItemBase.h"

UInventoryComponent* IInventoryOwnerInterface::GetInventory() const
{
	return Inventory;
}

void IInventoryOwnerInterface::DropItem(UItemBase* ItemToDrop, const int32 QuantityToDrop)
{
}

int32 IInventoryOwnerInterface::ChangeItemWithOtherInventory(UItemBase* ChangeFrom, UItemBase* ChangeTo, bool check, int32 amount)
{
	return 0;
}

void IInventoryOwnerInterface::DisplayToChangeItem(UItemBase* ChangeFrom, UItemBase* ChangeTo)
{
}

void IInventoryOwnerInterface::SplitItem(UItemBase* ItemToSplit, const int32 QuantityToSplit)
{
}
