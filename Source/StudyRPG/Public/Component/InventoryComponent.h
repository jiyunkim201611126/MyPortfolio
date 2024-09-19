#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

// 멀티캐스트 델리게이트를 이용한 옵저버 패턴, InventoryPanel의 RefreshInventory가 등록되어있음
// 한 번의 실행으로 안에 등록된 함수를 모두 실행
DECLARE_MULTICAST_DELEGATE(FOnInventoryUpdated);

class UItemBase;

UENUM(BlueprintType)
enum class EItemAddResult : uint8
{
	IAR_NoItemAdded UMETA(DisplayName = "No item added"),
	IAR_PartialAmountItemAdded UMETA(DisplayName = "Partial amount of item added"),
	IAR_AllItemAdded UMETA(DisplayName = "All of item added")
};

USTRUCT(BlueprintType)
struct FItemAddResult
{
	GENERATED_BODY()

	FItemAddResult() : 
		ActualAmountAdded(0),
		OperationResult(EItemAddResult::IAR_NoItemAdded),
		ResultMessage(FText::GetEmpty())
	{};

	// 실제로 인벤토리에 추가될 아이템 양
	UPROPERTY(BlueprintReadOnly, Category = "Item Add Result")
	int32 ActualAmountAdded;

	// 아이템 추가 작업의 종료 상태를 나타내는 Enum
	UPROPERTY(BlueprintReadOnly, Category = "Item Add Result")
	EItemAddResult OperationResult;

	// 결과에 따라 출력될 메시지
	UPROPERTY(BlueprintReadOnly, Category = "Item Add Result")
	FText ResultMessage;

	static FItemAddResult AddedNone(const FText& ErrorText)
	{
		FItemAddResult AddedNoneResult;
		AddedNoneResult.ActualAmountAdded = 0;
		AddedNoneResult.OperationResult = EItemAddResult::IAR_NoItemAdded;
		AddedNoneResult.ResultMessage = ErrorText;
		return AddedNoneResult;
	};
	static FItemAddResult AddedPartial(const int32 PartialAmountAdded, const FText& ErrorText)
	{
		FItemAddResult AddedPartialResult;
		AddedPartialResult.ActualAmountAdded = PartialAmountAdded;
		AddedPartialResult.OperationResult = EItemAddResult::IAR_PartialAmountItemAdded;
		AddedPartialResult.ResultMessage = ErrorText;
		return AddedPartialResult;
	};
	static FItemAddResult AddedAll(const int32 AmountAdded, const FText& Message)
	{
		FItemAddResult AddedAllResult;
		AddedAllResult.ActualAmountAdded = AmountAdded;
		AddedAllResult.OperationResult = EItemAddResult::IAR_AllItemAdded;
		AddedAllResult.ResultMessage = Message;
		return AddedAllResult;
	};
};

USTRUCT(BlueprintType)
struct FStartInventoryData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StartInventory")
	FDataTableRowHandle StartInventoryItem;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StartInventory")
	int32 StartInventoryItemQuantity;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class STUDYRPG_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	FOnInventoryUpdated OnInventoryUpdated;

	UInventoryComponent();

	void GenerateNullItems();
	void SetStartInventory();

	UFUNCTION(Category = "Inventory")
	FItemAddResult HandleAddItem(UItemBase* InputItem);

	UFUNCTION(Category = "Inventory")
	UItemBase* FindMatchingItem(UItemBase* ItemIn) const;

	UFUNCTION(Category = "Inventory")
	UItemBase* FindNextItemByID(UItemBase* ItemIn) const;

	UFUNCTION(Category = "Inventory")
	UItemBase* FindNextPartialStack(UItemBase* ItemIn) const;

	UFUNCTION(Category = "Inventory")
	void RefreshInventoryItem(UItemBase* ItemIn, int32 location, int32 remainingAmount);

	UFUNCTION(Category = "Inventory")
	int32 RemoveAmountOfItem(UItemBase* ItemIn, int32 DesiredAmountToRemove);

	UFUNCTION(Category = "Inventory")
	int32 ChangeItemWithOtherInventory(UItemBase* ChangeFrom, UItemBase* ChangeTo, bool check, int32 amount);

	UFUNCTION(Category = "Inventory")
	void ChangeItemOnSameInventory(UItemBase* ChangeFrom, UItemBase* ChangeTo);

	UFUNCTION(Category = "Inventory")
	void SplitExistingStack(UItemBase* ItemIn, const int32 AmountToSplit);

	// getters
	UFUNCTION(Category = "Inventory")
	FORCEINLINE FText GetInventoryName() { return InventoryName; };

	UFUNCTION(Category = "Inventory")

	FORCEINLINE int32 GetInventoryNumberOfItems() const { return InventoryNumberOfItems; };
	UFUNCTION(Category = "Inventory")
	FORCEINLINE float GetInventoryTotalWeight() const { return InventoryTotalWeight; };
	UFUNCTION(Category = "Inventory")

	FORCEINLINE int32 GetSlotsCapacity() const { return InventorySlotsCapacity; };
	UFUNCTION(Category = "Inventory")
	FORCEINLINE float GetWeightCapacity() const { return InventoryWeightCapacity; };
	UFUNCTION(Category = "Inventory")
	FORCEINLINE TArray<UItemBase*> GetInventoryContents() const { return InventoryContents; };

	// setters
	UFUNCTION(Category = "Inventory")
	FORCEINLINE void SetInventoryName(const FText NewInventoryName) { InventoryName = NewInventoryName; };

	UFUNCTION(Category = "Inventory")
	FORCEINLINE void SetSlotsCapacity(const int32 NewSlotsCapacity) { InventorySlotsCapacity = NewSlotsCapacity; };
	UFUNCTION(Category = "Inventory")
	FORCEINLINE void SetWeightCapacity(const float NewWeightCapacity) { InventoryWeightCapacity = NewWeightCapacity; };

protected:

	UPROPERTY(VisibleAnywhere, Category = "Inventory")
	FText InventoryName;

	UPROPERTY(VisibleAnywhere, Category = "Inventory")
	int32 InventoryNumberOfItems;

	UPROPERTY(VisibleAnywhere, Category = "Inventory")
	float InventoryTotalWeight;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	int32 InventorySlotsCapacity;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	float InventoryWeightCapacity;

	UPROPERTY(VisibleAnywhere, Category = "Inventory")
	TArray<UItemBase*> InventoryContents;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	TArray<FStartInventoryData> StartInventory;

	virtual void BeginPlay() override;
	
	FItemAddResult HandleNonStackableItems(UItemBase* ItemIn, int32 RequestedAddAmount);
	int32 HandleStackableItems(UItemBase* ItemIn, int32 RequestedAddAmount);
	int32 CalculateWeightAddAmount(UItemBase* ItemIn, int32 RequestedAddAmount);
	int32 CalculateNumberForFullStack(UItemBase* StackableItem, int32 InitialRequestedAddAmount);

	void AddNewItem(UItemBase* Item, const int32 AmountToAdd);
};
