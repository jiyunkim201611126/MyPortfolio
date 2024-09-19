#include "Component/InventoryComponent.h"
#include "ItemBase.h"

UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	InventoryName = FText::FromString("Player Inventory");
	InventoryNumberOfItems = 0;
}

void UInventoryComponent::GenerateNullItems()
{
	// 비어있는 element 추가
	for (int i = 0; i < InventorySlotsCapacity; i++)
	{
		UItemBase* NullItem = NewObject<UItemBase>(this, UItemBase::StaticClass());

		NullItem->isValidItem = false;
		NullItem->OwningInventory = this;
		InventoryContents.Add(NullItem);
	}
}

void UInventoryComponent::SetStartInventory()
{
	// 초기 인벤토리 구현

	// &가 없으면 복사본이 계속 생겨서 성능에 문제 생김
	// &가 있으면 ref로 가져온 element가 변경될 수 있음
	// const도 붙여서 변경되는 걸 방지
	for (const FStartInventoryData& Item : StartInventory)
	{
		const FItemData* ItemData = Item.StartInventoryItem.GetRow<FItemData>(Item.StartInventoryItem.RowName.ToString());

		UItemBase* NewItem = NewObject<UItemBase>(this, UItemBase::StaticClass());

		NewItem->InitializeItemBase(ItemData, Item.StartInventoryItemQuantity);

		AddNewItem(NewItem, NewItem->Quantity);
	}

	// 최적화를 위해 메모리 해제
	StartInventory.Empty();
}

UItemBase* UInventoryComponent::FindMatchingItem(UItemBase* ItemIn) const
{
	if (ItemIn)
	{
		if (InventoryContents.Contains(ItemIn))
		{
			return ItemIn;
		}
	}
	return nullptr;
}

UItemBase* UInventoryComponent::FindNextItemByID(UItemBase* ItemIn) const
{
	if (ItemIn)
	{
		if (const TArray<UItemBase*>::ElementType* Result = InventoryContents.FindByKey(ItemIn))
		{
			return *Result;
		}
	}

	return nullptr;
}

// Pickup한 아이템과 같은 아이템 중 최대 스택이 아닌 것을 탐색
UItemBase* UInventoryComponent::FindNextPartialStack(UItemBase* ItemIn) const
{
	if (const TArray<UItemBase*>::ElementType* Result = 
		// 아래 return에 적힌 조건과 일치하는 element의 포인터를 반환
		// [&ItemIn]은 캡쳐된 람다식의 외부 매개변수, 캡쳐 안 하면 람다 식 내부에서 못 씀
		// (const UItemBase* InventoryItem)은 반복문 실행 중 InventoryContents의 element
		InventoryContents.FindByPredicate([&ItemIn](const UItemBase* InventoryItem)
			{
				// 주어진 ItemIn의 ID와 일치하는 InventoryItem ID 즉, 같은 아이템 중 최대 스택이 아닌 아이템 탐색
				if (InventoryItem->isValidItem)
					return InventoryItem->ID == ItemIn->ID && !InventoryItem->IsFullItemStack();
				else
					return false;
			}
		))
	{
		return *Result;
	}
	return nullptr;
}

void UInventoryComponent::RefreshInventoryItem(UItemBase* ItemIn, int32 location, int32 remainingAmount)
{
	if (remainingAmount <= 0)
	{
		UItemBase* NullItem = NewObject<UItemBase>(this, UItemBase::StaticClass());
		NullItem->isValidItem = false;
		NullItem->OwningInventory = this;

		InventoryContents[location] = NullItem;
		InventoryNumberOfItems--;
	}
	else
	{
		InventoryContents[location]->SetQuantity(remainingAmount);
	}
}

// Weight에 따른 주울 수 있는 최대 개수 반환
int32 UInventoryComponent::CalculateWeightAddAmount(UItemBase* ItemIn, int32 RequestedAddAmount)
{
	// (인벤토리 최대 무게 - 인벤토리 현재 무게)를 Pickup하려는 아이템의 1개 무게로 나눔. 즉, 현재 주우려 하는 아이템의 주울 수 있는 최대 개수
	const int32 WeightMaxAddAmount = FMath::FloorToInt((GetWeightCapacity() - InventoryTotalWeight) / ItemIn->GetItemSingleWeight());
	// Pickup하려는 아이템 개수보다 방금 그 값이 큰 경우
	if (WeightMaxAddAmount >= RequestedAddAmount)
	{
		// 전부 Pickup할 수 있음
		return RequestedAddAmount;
	}
	// 그렇지 않은 경우 주울 수 있는 최대 개수 반환
	return WeightMaxAddAmount;
}

// FullStack이 되기 위해 몇 개의 Quantity가 필요한지 계산
int32 UInventoryComponent::CalculateNumberForFullStack(UItemBase* StackableItem, int32 InitialRequestedAddAmount)
{
	// 대상이 되는 아이템의 MaxStackSize - Quantity
	const int32 AddAmountToMakeFullStack = StackableItem->NumericData.MaxStackSize - StackableItem->Quantity;

	// 위 계산값과 더하려는 양 중 적은 걸 반환
	return FMath::Min(InitialRequestedAddAmount, AddAmountToMakeFullStack);
}

// 아이템 중 일부 개수를 제거
int32 UInventoryComponent::RemoveAmountOfItem(UItemBase* ItemIn, int32 DesiredAmountToRemove)
{
	// 현재 아이템 개수와 요청받은 제거 개수를 비교해 작은 것을 변수에 저장
	const int32 ActualAmountToRemove = FMath::Min(DesiredAmountToRemove, ItemIn->Quantity);
	const int32 RemainingAmount = ItemIn->Quantity - ActualAmountToRemove;

	int location = InventoryContents.IndexOfByKey(ItemIn);

	RefreshInventoryItem(ItemIn, location, RemainingAmount);

	// 뺀 만큼 Weight도 줄여줌
	InventoryTotalWeight -= ActualAmountToRemove * ItemIn->GetItemSingleWeight();

	// 인벤토리 새로고침
	OnInventoryUpdated.Broadcast();

	return ActualAmountToRemove;
}

// 두 아이템 위치 교환(다른 인벤토리끼리도 고려)
// check가 false면 ChangeFrom(드래그 감지)을 빼고, true면 ChangeFrom(2번째 들어온 거라 드랍 감지)에 더해야 함.
int32 UInventoryComponent::ChangeItemWithOtherInventory(UItemBase* ChangeFrom, UItemBase* ChangeTo, bool check, int32 amount)
{
	// 드랍 감지한 슬롯이 빈 슬롯이 아닐 때
	if (ChangeTo->isValidItem)
	{
		// 같은 아이템이면서 쌓을 수 있는 경우
		if (ChangeFrom->ID == ChangeTo->ID && ChangeTo->NumericData.bIsStackable)
		{
			int32 AbleAddAmount;

			// 처음 들어옴. 드래그 감지한 슬롯의 아이템을 빼야 하는 상황
			if (check)
			{
				// 드랍을 감지한 아이템에 드래그를 감지한 아이템 몇 개를 넣을 수 있는지 계산
				AbleAddAmount = CalculateNumberForFullStack(ChangeTo, amount);
				AbleAddAmount = ChangeTo->OwningInventory->CalculateWeightAddAmount(ChangeTo, AbleAddAmount);

				// 1개도 못 넣으면 교환 종료
				if (AbleAddAmount <= 0)
					return 0;

				// 넣을 수 있으면 그만큼 From에서 뺌
				RemoveAmountOfItem(ChangeFrom, AbleAddAmount);
			}
			// 2번째 들어옴. 드랍 감지한 슬롯의 아이템에 amount만큼 넣어야 하는 상황
			else
			{
				ChangeFrom->SetQuantity(ChangeFrom->Quantity + amount);
				InventoryTotalWeight += amount * ChangeFrom->GetItemSingleWeight();

				OnInventoryUpdated.Broadcast();

				// 아이템 교환 종료
				return 0;
			}

			// ChangeTo로 넘어갈 개수를 반환
			return AbleAddAmount;
		}
	}

	// 쌓을 수 없거나 다른 아이템이거나 빈 슬롯이 있는 경우
	// 처음 들어옴. 드래그 감지한 슬롯의 아이템을 빼야 하는 상황
	if (check)
	{
		// 드랍 감지한 슬롯이 빈 슬롯일 때
		if (!ChangeTo->isValidItem)
		{
			// 드랍 감지한 인벤토리에 넣을 수 있는 Quantity 확인
			int32 AbleAddAmount = amount;

			AbleAddAmount = ChangeTo->OwningInventory->CalculateWeightAddAmount(ChangeFrom, AbleAddAmount);

			// 못 넣으면 교환 종료하고, 넣을 수 있으면 드래그 감지한 슬롯의 아이템을 AbleAddAmount만큼 빼고 반환
			if (AbleAddAmount <= 0)
				return 0;
			else
			{
				RemoveAmountOfItem(ChangeFrom, AbleAddAmount);
				return AbleAddAmount;
			}
		}
		// 드랍 감지한 슬롯이 빈 슬롯이 아닐 때
		else if (ChangeTo->isValidItem)
		{
			// 교환 가능한지 확인하기 위한 변수들
			int32 ChangeFromWeight = ChangeFrom->Quantity * ChangeFrom->GetItemSingleWeight();
			int32 ChangeToWeight = ChangeTo->Quantity * ChangeTo->GetItemSingleWeight();

			int32 FromInventoryWeight = InventoryTotalWeight;
			int32 ToInventoryWeight = ChangeTo->OwningInventory->GetInventoryTotalWeight();

			// 인벤토리에서 나갈 아이템 무게를 빼고, 들어올 아이템 무게를 더함
			int32 FromAble = FromInventoryWeight - ChangeFromWeight + ChangeToWeight;
			int32 ToAble = ToInventoryWeight - ChangeToWeight + ChangeFromWeight;

			// 교환 가능한 경우, 즉 Able 변수들이 최대 인벤토리 무게보다 작거나 같을 때
			if (FromAble <= InventoryWeightCapacity && ToAble <= ChangeTo->OwningInventory->GetWeightCapacity())
			{
				UInventoryComponent* TempInventory = ChangeTo->OwningInventory;

				// 무게 변경
				InventoryTotalWeight = FromAble;
				TempInventory->InventoryTotalWeight = ToAble;

				// 포인터가 덜 바뀐 채로 밖으로 나가버리면 From이 To를 덮어씌우므로 여기서 모두 처리
				int location = InventoryContents.IndexOfByKey(ChangeFrom);
				InventoryContents[location] = ChangeTo;
				InventoryContents[location]->OwningInventory = this;

				location = TempInventory->InventoryContents.IndexOfByKey(ChangeTo);
				TempInventory->InventoryContents[location] = ChangeFrom;
				TempInventory->InventoryContents[location]->OwningInventory = TempInventory;

				OnInventoryUpdated.Broadcast();
				TempInventory->OnInventoryUpdated.Broadcast();

				// 아이템 교환 종료
				return 0;
			}
			// 교환 안 되면 2번째 못 들어가게 0 반환
			else
			{
				return 0;
			}
		}
	}
	// 2번째 들어옴. 드랍 감지한 슬롯의 아이템에 amount만큼 넣어야 하는 상황
	else
	{
		// amount만큼의 Quantity를 가진 새로운 아이템을 가리키도록 초기화
		UItemBase* NewItem = NewObject<UItemBase>(this, UItemBase::StaticClass());
		NewItem = ChangeTo->CreateItemCopy();
		NewItem->OwningInventory = this;
		NewItem->SetQuantity(amount);

		int location = InventoryContents.IndexOfByKey(ChangeFrom);
		InventoryContents[location] = NewItem;

		InventoryTotalWeight += amount * ChangeTo->GetItemSingleWeight();
		InventoryNumberOfItems++;
	}

	OnInventoryUpdated.Broadcast();

	return 0;
}

// 현재 인벤토리 내에서 아이템 위치 교환
void UInventoryComponent::ChangeItemOnSameInventory(UItemBase* ChangeFrom, UItemBase* ChangeTo)
{
	int fromLocation = InventoryContents.IndexOfByKey(ChangeFrom);
	int toLocation = InventoryContents.IndexOfByKey(ChangeTo);

	// 같은 아이템이면서 쌓을 수 있는 경우
	if (ChangeFrom->ID == ChangeTo->ID && ChangeTo->NumericData.bIsStackable)
	{
		int32 itemMaxStackSize = ChangeTo->NumericData.MaxStackSize;
		int32 sumQuantity = ChangeFrom->Quantity + ChangeTo->Quantity;

		ChangeTo->SetQuantity(sumQuantity >= itemMaxStackSize ? itemMaxStackSize : sumQuantity);
		ChangeFrom->SetQuantity(sumQuantity - itemMaxStackSize);

		RefreshInventoryItem(ChangeFrom, fromLocation, sumQuantity - itemMaxStackSize);

		OnInventoryUpdated.Broadcast();

		return;
	}

	InventoryContents[fromLocation] = ChangeTo;
	InventoryContents[toLocation] = ChangeFrom;

	OnInventoryUpdated.Broadcast();
}

// 아이템 개수 분할
void UInventoryComponent::SplitExistingStack(UItemBase* ItemIn, const int32 AmountToSplit)
{
	// 현재 인벤토리에 빈 칸이 있는 경우
	if (InventoryNumberOfItems < InventorySlotsCapacity)
	{
		// 아이템 분할. 아이템의 개수를 AmountToSplit만큼 줄이고 그 개수의 인벤 한 칸 차지하는 아이템 새로 생성
		RemoveAmountOfItem(ItemIn, AmountToSplit);
		AddNewItem(ItemIn, AmountToSplit);
	}
}

// 쌓을 수 없는 아이템을 인벤토리에 추가하는 함수
FItemAddResult UInventoryComponent::HandleNonStackableItems(UItemBase* InputItem, int32 RequestedAddAmount)
{
	// 인벤토리에 들어갈 아이템이 유효한 weight를 가지지 않았다면
	if (FMath::IsNearlyZero(InputItem->GetItemSingleWeight()) || InputItem->GetItemStackWeight() < 0)
	{
		return FItemAddResult::AddedNone(FText::Format(
			FText::FromString("Could not add {0} to the inventory. Item has invalid weight value."), InputItem->TextData.Name));
	}

	// 인벤토리에 들어갈 때 최대 무게를 넘으면
	if (InventoryTotalWeight + InputItem->GetItemSingleWeight() > GetWeightCapacity())
	{
		return FItemAddResult::AddedNone(FText::Format(
			FText::FromString("Could not add {0} to the inventory. Item would overflow weight limit."), InputItem->TextData.Name));
	}

	// 인벤토리가 꽉 찼다면
	if (InventoryNumberOfItems + 1 > InventorySlotsCapacity)
	{
		return FItemAddResult::AddedNone(FText::Format(
			FText::FromString("Could not add {0} to the inventory. All inventory slots are full."), InputItem->TextData.Name));
	}

	AddNewItem(InputItem, RequestedAddAmount);
	return FItemAddResult::AddedAll(RequestedAddAmount, FText::Format(
		FText::FromString("Successfully added {0} {1} to the inventory"), RequestedAddAmount, InputItem->TextData.Name));
}

// 쌓을 수 있는 아이템을 인벤토리에 추가하는 함수
// 반환값은 인벤토리에 추가된 개수
int32 UInventoryComponent::HandleStackableItems(UItemBase* ItemIn, int32 RequestedAddAmount)
{
	// 인벤토리에 들어갈 아이템이 유효한 개수, 혹은 Weight를 가지지 않았다면
	if (RequestedAddAmount <= 0 || FMath::IsNearlyZero(ItemIn->GetItemStackWeight()))
	{
		return 0;
	}

	/*    로직 흐름
	*  1. 인벤토리에서 주울 아이템과 동일한 아이템 탐색
	*  2. 최대 스택인 경우 다른 동일한 아이템 탐색
	*  3. 최대 스택 아닌 경우 해당 칸에 넣음
	*  4. 위 흐름을 마치거나, 동일한 아이템이 없는 경우
	*  5. 인벤토리에 빈칸 있는지 확인 후 새로운 칸에 넣음
	*  6. 모두 넣는 데에 성공했으면 들어온 int 매개변수 그대로 반환
	*  7. 일부만 넣은 경우 들어간 값을 반환
	*  8. 하나도 못 넣은 경우 0 반환
	*/ 

	int32 AmountToDistribute = RequestedAddAmount;

	// 같은 아이템 중 최대 스택 아닌 걸 가져옴
	UItemBase* ExistingItemStack = FindNextPartialStack(ItemIn);

	while (ExistingItemStack)
	{
		// 최대 스택 개수까지 몇 개 필요한지 계산 (64개 쌓는 게 가능하면, 64에서 현재 개수 빼줌)
		const int AmountToMakeFullStack = CalculateNumberForFullStack(ExistingItemStack, AmountToDistribute);
		// Weight 한도치까지 몇 개 넣을 수 있는지 (다 들어갈 수 있으면 AmountToMakeFullStack의 값이 그대로 나옴)
		const int WeightLimitAddAmount = CalculateWeightAddAmount(ExistingItemStack, AmountToMakeFullStack);

		// 1개 이상 넣을 수 있으면
		if (WeightLimitAddAmount > 0)
		{
			// 개수 더해주고
			ExistingItemStack->SetQuantity(ExistingItemStack->Quantity + WeightLimitAddAmount);

			// 무게 더해주고
			InventoryTotalWeight += (ExistingItemStack->GetItemSingleWeight() * WeightLimitAddAmount);
			// 주우려고 한 양에서 실제로 주워진 양 빼기
			AmountToDistribute -= WeightLimitAddAmount;
			// 바닥에 남겨진 아이템 개수 조정
			ItemIn->SetQuantity(AmountToDistribute);

			// 현재 인벤토리 무게 + 넣을 아이템의 1개 무게가, 인벤토리 최대 무게를 초과할 경우 함수 종료
			if (InventoryTotalWeight + ExistingItemStack->GetItemSingleWeight() > InventoryWeightCapacity)
			{
				OnInventoryUpdated.Broadcast();
				return RequestedAddAmount - AmountToDistribute;
			}
		}
		// 더이상 1개도 넣지 못 하면
		else if (WeightLimitAddAmount <= 0)
		{
			// 넣은 작업이 1번이라도 있다면
			if (AmountToDistribute != RequestedAddAmount)
			{
				OnInventoryUpdated.Broadcast();
				return RequestedAddAmount - AmountToDistribute;
			}

			// 그냥 못 넣는 경우
			return 0;
		}

		// 넣을 아이템이 남지 않은 경우
		if (AmountToDistribute <= 0)
		{
			OnInventoryUpdated.Broadcast();
			return RequestedAddAmount;
		}

		// 넣을 아이템이 남아있고, 더 넣을 수 있는 경우 같은 아이템 다시 탐색
		ExistingItemStack = FindNextPartialStack(ItemIn);
	}

	// 같은 아이템이 있지만 모두 최대 스택이거나, 같은 아이템이 없는 경우 여기에 도달
	
	// 빈 슬롯이 있으면
	if (InventoryNumberOfItems < InventorySlotsCapacity)
	{
		// Weight 한도치까지 몇 개 넣을 수 있는지 (다 들어갈 수 있으면 AmountToMakeFullStack의 값이 그대로 나옴)
		const int32 WeightLimitAddAmount = CalculateWeightAddAmount(ItemIn, AmountToDistribute);

		// 1개 이상 넣을 수 있으면
		if (WeightLimitAddAmount > 0)
		{
			// 일부만 넣을 수 있으면
			if (WeightLimitAddAmount < AmountToDistribute)
			{
				// 넣어야 하는 양에서 넣는 만큼 빼주고
				AmountToDistribute -= WeightLimitAddAmount;
				// 바닥에 남겨진 아이템 개수 조정하고
				ItemIn->SetQuantity(AmountToDistribute);

				// 복사본으로 인벤토리에 들어감
				AddNewItem(ItemIn->CreateItemCopy(), WeightLimitAddAmount);
				return RequestedAddAmount - AmountToDistribute;
			}
			
			// 다 넣을 수 있으면 그대로 들어감
			AddNewItem(ItemIn, AmountToDistribute);
			return RequestedAddAmount;
		}

		// 아이템 슬롯은 남았지만 무게는 남지 않았을 때 도달
		return RequestedAddAmount - AmountToDistribute;
	}

	// 인벤토리 내에 같은 아이템도 없고, 슬롯도 없는 경우 도달
	return 0;
}

// 인벤토리에 아이템 추가
FItemAddResult UInventoryComponent::HandleAddItem(UItemBase* InputItem)
{
	// 인벤토리 소지 액터가 유효한지 체크
	if (GetOwner())
	{
		// 추가하려는 아이템의 개수 가져옴
		const int32 InitialRequestedAddAmount = InputItem->Quantity;

		// 넣을 아이템이 쌓을 수 없는 아이템이면
		if (!InputItem->NumericData.bIsStackable)
		{
			return HandleNonStackableItems(InputItem, InitialRequestedAddAmount);
		}

		// 쌓을 수 있는 아이템이면
		const int32 StackableAmountAdded = HandleStackableItems(InputItem, InitialRequestedAddAmount);

		// 인벤토리에 전부 들어간 경우
		if (StackableAmountAdded == InitialRequestedAddAmount)
		{
			return FItemAddResult::AddedAll(InitialRequestedAddAmount, FText::Format(
				FText::FromString("Successfully added {0} {1} to the inventory."),
				InitialRequestedAddAmount, InputItem->TextData.Name));
		}

		// 인벤토리에 일부만 들어간 경우
		if (StackableAmountAdded < InitialRequestedAddAmount && StackableAmountAdded > 0)
		{
			return FItemAddResult::AddedPartial(StackableAmountAdded, FText::Format(
				FText::FromString("Partial amount of {0} added to the inventory. Number added = {1}"),
				InputItem->TextData.Name, StackableAmountAdded));
		}

		// 인벤토리에 들어가지 못 한 경우
		if (StackableAmountAdded <= 0)
		{
			return FItemAddResult::AddedNone(FText::Format(
				FText::FromString("Couldn't add {0} to the inventory. No remaining inventory slots, or invalid item."),
				InputItem->TextData.Name));
		}
	}

	// 그럴 일은 보통 없지만 캐릭터가 유효하지 않은데 여기 접근했을 수 없으니 프로그램을 종료해야 함
	// check는 매개변수가 false인 경우 프로그램을 종료하는 매크로
	check(false);
	return FItemAddResult::AddedNone(FText::FromString("TryAddItem fall through error. GetOwner() check somehow failed."));
}

// 아이템 추가
void UInventoryComponent::AddNewItem(UItemBase* Item, const int32 AmountToAdd)
{
	UItemBase* NewItem = NewObject<UItemBase>(this, UItemBase::StaticClass());

	// 바닥에 떨어진 아이템이면
	if (Item->bIsPickup)
	{
		NewItem = Item;
		NewItem->ResetItemFlags();
	}
	// 아이템을 나누거나 다른 인벤토리로(에서) 드래그하는 경우 아이템 복사
	else
	{
		NewItem = Item->CreateItemCopy();
	}
	
	// 만들어진 아이템에게 이 인벤토리를 참조시킴
	NewItem->OwningInventory = this;
	// 추가하도록 요청받은 개수로 Quantity 초기화
	NewItem->SetQuantity(AmountToAdd);

	// InventoryContents의 원소 중 무효한 원소 찾아서 NewItem을 가리키도록 할당
	int32 counter = 0;

	for (UItemBase*& EmptyItem : InventoryContents)
	{
		if (EmptyItem->isValidItem == false)
		{
			InventoryContents[counter] = NewItem;
			// 무게와 개수 증가
			InventoryTotalWeight += NewItem->GetItemStackWeight();
			InventoryNumberOfItems++;
			// 인벤토리 새로고침
			OnInventoryUpdated.Broadcast();
			return;
		}
		counter++;
	}
}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
}