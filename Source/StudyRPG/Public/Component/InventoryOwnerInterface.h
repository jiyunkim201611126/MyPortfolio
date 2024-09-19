#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InventoryOwnerInterface.generated.h"

class UInventoryComponent;
class UItemBase;

UINTERFACE(MinimalAPI)
class UInventoryOwnerInterface : public UInterface
{
	GENERATED_BODY()
};

class STUDYRPG_API IInventoryOwnerInterface
{
	GENERATED_BODY()

public:
	UInventoryComponent* Inventory;

	FORCEINLINE virtual class UInventoryComponent* GetInventory() const;

	virtual void DropItem(UItemBase* ItemToDrop, const int32 QuantityToDrop);
	virtual int32 ChangeItemWithOtherInventory(UItemBase* ChangeFrom, UItemBase* ChangeTo, bool check, int32 amount);
	virtual void DisplayToChangeItem(UItemBase* ChangeFrom, UItemBase* ChangeTo);
	virtual void SplitItem(UItemBase* ItemToSplit, const int32 QuantityToSplit);

};
