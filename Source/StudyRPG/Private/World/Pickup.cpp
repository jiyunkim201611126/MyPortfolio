#include "World/Pickup.h"
#include "Component/InventoryComponent.h"
#include "ItemBase.h"

// Sets default values
APickup::APickup()
{
	PrimaryActorTick.bCanEverTick = false;

	PickupMesh = CreateDefaultSubobject<UStaticMeshComponent>("PickupMesh");
	PickupMesh->SetSimulatePhysics(true);
	SetRootComponent(PickupMesh);

	InteractionCollision = CreateDefaultSubobject<USphereComponent>("InteractionCollision");
	InteractionCollision->SetupAttachment(PickupMesh);
	InteractionCollision->SetSphereRadius(64.0f);

	PickupMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	PickupMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
	InteractionCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	InteractionCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Block);
}

void APickup::BeginPlay()
{
	Super::BeginPlay();

	InitializePickup(UItemBase::StaticClass(), ItemQuantity);
}

void APickup::InitializePickup(const TSubclassOf<UItemBase> BaseClass, const int32 InQuantity)
{
	// ItemDataTable과 DesiredItemID가 제대로 기입됐는지 확인
	if (!ItemRowHandle.IsNull())
	{
		// ItemDataTable에서 DesiredItemID와 일치하는 행을 탐색
		const FItemData* ItemData = ItemRowHandle.GetRow<FItemData>(ItemRowHandle.RowName.ToString());

		// 화면에 보이는 정보(메쉬 등)가 아닌, 실제 아이템으로서 역할을 해낼 정보들을 초기화
		// CreateDefaultSubobject는 블루프린트에서 보기 위해 생성자에서 실행하는 것
		// NewObject는 그 외 모든 곳에서 사용, 여기서 this는 생성될 오브젝트의 소유자
		ItemReference = NewObject<UItemBase>(this, BaseClass);
		ItemReference->InitializeItemBase(ItemData, InQuantity);

		// 메쉬 세팅
		PickupMesh->SetStaticMesh(ItemData->AssetData.Mesh);

		UpdateInteractableData();
	}
}


void APickup::InitializeDrop(UItemBase* ItemToDrop, const int32 InQuantity)
{
	UItemBase* NewItem = NewObject<UItemBase>(this, UItemBase::StaticClass());
	NewItem = ItemToDrop->CreateItemCopy();

	ItemReference = NewItem;
	InQuantity <= 0 ? ItemReference->SetQuantity(1) : ItemReference->SetQuantity(InQuantity);
	ItemReference->NumericData.Weight = NewItem->GetItemSingleWeight();
	ItemReference->OwningInventory = nullptr;
	PickupMesh->SetStaticMesh(NewItem->AssetData.Mesh);

	UpdateInteractableData();
}

// InteractionWidget에 적힐 문장을 위한 초기화
void APickup::UpdateInteractableData()
{
	InstanceInteractableData.InteractableType = EInteractableType::Pickup;
	InstanceInteractableData.Name = ItemReference->TextData.Name;
	InstanceInteractableData.Action = ItemReference->TextData.InteractionText;
	InstanceInteractableData.Quantity = ItemReference->Quantity;
	InteractableData = InstanceInteractableData;
}

// 상호작용 가능함을 알리는 아웃라인 생성
void APickup::BeginFocus()
{
	if (PickupMesh)
	{
		PickupMesh->SetRenderCustomDepth(true);
	}
}

// 아웃라인 제거
void APickup::EndFocus()
{
	if (PickupMesh)
	{
		PickupMesh->SetRenderCustomDepth(false);
	}
}

void APickup::Interact(AMainCharacter* PlayerCharacter)
{
	if (PlayerCharacter)
	{
		TakePickup(PlayerCharacter);
	}
}

void APickup::TakePickup(const AMainCharacter* Taker)
{
	// Pickup할 인스턴스가 유효하면
	if (!IsPendingKillPending())
	{
		// 해당 인스턴스의 ItemReference가 유효하면
		if (ItemReference)
		{
			// 플레이어 인벤토리(지역 변수로 선언하며)가 유효하면
			if (UInventoryComponent* PlayerInventory = Taker->GetInventory())
			{
				// 플레이어 인벤토리에 자신의 ItemReference를 추가해달라고 요청, 결과가 담긴 구조체를 반환받음
				const FItemAddResult AddResult = PlayerInventory->HandleAddItem(ItemReference);

				// 구조체 안의 내용에 따른 작업
				switch (AddResult.OperationResult)
				{
					// 인벤토리에 아무것도 추가되지 못 했다면
				case EItemAddResult::IAR_NoItemAdded:
					break;
					// 인벤토리에 일부만 추가됐다면 InteractionWidget에 반영
				case EItemAddResult::IAR_PartialAmountItemAdded:
					UpdateInteractableData();
					Taker->UpdateInteractionWidget();
					break;
					// 인벤토리에 모두 추가됐다면 자신을 Destory
				case EItemAddResult::IAR_AllItemAdded:
					Destroy();
					break;
				}
				UE_LOG(LogTemp, Warning, TEXT("%s"), *AddResult.ResultMessage.ToString());
			}
			else
			{
				// BP_MainCharacter에 InventoryComponent를 추가하지 않은 경우임
				UE_LOG(LogTemp, Warning, TEXT("Player inventory component is null"));
			}
		}
		else
		{
			// 보통 그럴 일은 없지만 Pickup클래스의 ItemReference가 null인 경우 출력
			UE_LOG(LogTemp, Warning, TEXT("Pickup internal item reference was somehow null"));
		}
	}
}

void APickup::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	const FName ChangedPropertyName = PropertyChangedEvent.Property ? PropertyChangedEvent.Property->GetFName() : NAME_None;

	if (ChangedPropertyName == GET_MEMBER_NAME_CHECKED(FDataTableRowHandle, RowName))
	{
		if (!ItemRowHandle.IsNull())
		{
			const FItemData* ItemData = ItemRowHandle.GetRow<FItemData>(ItemRowHandle.RowName.ToString());
			PickupMesh->SetStaticMesh(ItemData->AssetData.Mesh);
		}
	}
}

