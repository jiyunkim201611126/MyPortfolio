#pragma once

#include "CoreMinimal.h"
#include "StructClass.h"
#include "MainCharacter.h"
#include "ItemBase.generated.h"

class UInventoryComponent;

UCLASS(Blueprintable)
class STUDYRPG_API UItemBase : public UObject
{
	GENERATED_BODY()
	
public:
	UPROPERTY()
	bool isValidItem;

	UPROPERTY()
	UInventoryComponent* OwningInventory;

	UPROPERTY(EditAnywhere, Category = "Item Data", meta = (UIMin = 1, UIMax = 100))
	int32 Quantity;

	UPROPERTY(EditAnywhere, Category = "Item Data")
	FName ID;

	UPROPERTY(EditAnywhere, Category = "Item Data")
	EItemType ItemType;

	UPROPERTY(EditAnywhere, Category = "Item Data")
	EItemQuality ItemQuality;

	UPROPERTY(EditAnywhere, Category = "Item Data")
	FItemStatistics ItemStatistics;

	UPROPERTY(EditAnywhere, Category = "Item Data")
	FItemTextData TextData;

	UPROPERTY(EditAnywhere, Category = "Item Data")
	FItemNumericData NumericData;

	UPROPERTY(EditAnywhere, Category = "Item Data")
	FItemAssetData AssetData;

	bool bIsCopy;
	bool bIsPickup;

	void ResetItemFlags();

	void InitializeItemBase(const FItemData* ItemData, const int32 InQuantity);

	UItemBase();

	UFUNCTION(Category = "Item")
	UItemBase* CreateItemCopy() const;

	UFUNCTION(Category = "Item")
	FORCEINLINE float GetItemStackWeight() const { return Quantity * NumericData.Weight; }

	UFUNCTION(Category = "Item")
	FORCEINLINE float GetItemSingleWeight() const { return NumericData.Weight; }

	UFUNCTION(Category = "Item")
	FORCEINLINE bool IsFullItemStack() const { return Quantity == NumericData.MaxStackSize; }

	UFUNCTION(Category = "Item")
	void SetQuantity(const int32 newQuantity);

	UFUNCTION(Category = "Item")
	virtual void Use(AMainCharacter* character);


protected:
	// '==' 연산자를 오버로딩. 매개변수로 들어온 FName과 ID를 비교한 bool을 반환
	bool operator==(const FName& OtherID) const
	{
		return ID == OtherID;
	}
};
