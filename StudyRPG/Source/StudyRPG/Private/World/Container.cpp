#include "World/Container.h"
#include "ContainerBase.h"
#include "Components/SphereComponent.h"
#include "Component/InventoryComponent.h"
#include "MainCharacter.h"
#include "World/Pickup.h"

// Sets default values
AContainer::AContainer()
{
	PrimaryActorTick.bCanEverTick = false;

	ContainerMesh = CreateDefaultSubobject<UStaticMeshComponent>("ContainerMesh");
	ContainerMesh->SetSimulatePhysics(true);
	SetRootComponent(ContainerMesh);

	InteractionCollision = CreateDefaultSubobject<USphereComponent>("InteractionCollision");
	InteractionCollision->SetupAttachment(ContainerMesh);
	InteractionCollision->SetSphereRadius(64.0f);

	ContainerMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	ContainerMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
	InteractionCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	InteractionCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Block);

	// 인벤토리 컴포넌트 생성
	ContainerInventory = CreateDefaultSubobject<UInventoryComponent>(TEXT("ContainerInventory"));
	Inventory = ContainerInventory;
}

void AContainer::InitializeContainer(const TSubclassOf<UContainerBase> BaseClass)
{
	// ItemDataTable과 DesiredItemID가 제대로 기입됐는지 확인
	if (!ContainerRowHandle.IsNull())
	{
		// ItemDataTable에서 DesiredItemID와 일치하는 행을 탐색
		const FContainerData* ContainerData = ContainerRowHandle.GetRow<FContainerData>(ContainerRowHandle.RowName.ToString());

		// 화면에 보이는 정보(메쉬 등)가 아닌, 실제 아이템으로서 역할을 해낼 정보들을 초기화
		// CreateDefaultSubobject는 블루프린트에서 보기 위해 생성자에서 실행하는 것
		// NewObject는 그 외 모든 곳에서 사용, 여기서 this는 생성될 오브젝트의 소유자
		ContainerReference = NewObject<UContainerBase>(this, BaseClass);

		ContainerReference->ID = ContainerData->ID;
		ContainerReference->NumericData = ContainerData->NumericData;
		ContainerReference->TextData = ContainerData->TextData;

		// 메쉬 세팅
		ContainerMesh->SetStaticMesh(ContainerData->AssetData.Mesh);

		Inventory->SetInventoryName(ContainerReference->TextData.Name);
		Inventory->SetSlotsCapacity(ContainerReference->NumericData.SlotsCapacity);
		Inventory->SetWeightCapacity(ContainerReference->NumericData.WeightCapacity);
		Inventory->GenerateNullItems();
		Inventory->SetStartInventory();

		UpdateInteractableData();
	}
}

void AContainer::UpdateInteractableData()
{
	InstanceInteractableData.InteractableType = EInteractableType::Container;
	InstanceInteractableData.Name = ContainerReference->TextData.Name;
	InstanceInteractableData.Action = ContainerReference->TextData.InteractionText;
	InstanceInteractableData.Quantity = 1;
	InteractableData = InstanceInteractableData;
}

void AContainer::BeginFocus()
{
	if (ContainerMesh)
	{
		ContainerMesh->SetRenderCustomDepth(true);
	}
}

void AContainer::EndFocus()
{
	if (ContainerMesh)
	{
		ContainerMesh->SetRenderCustomDepth(false);
	}
}

void AContainer::Interact(AMainCharacter* PlayerCharacter)
{
	if (PlayerCharacter)
	{
		PlayerCharacter->GetMainHUD()->SetOtherInventory(ContainerInventory);
		OpenContainerInventory(PlayerCharacter);
	}
}

void AContainer::OpenContainerInventory(AMainCharacter* PlayerCharacter)
{
	PlayerCharacter->ToggleOtherInventory();
}

void AContainer::DropItem(UItemBase* ItemToDrop, const int32 QuantityToDrop)
{
	if (Inventory->FindMatchingItem(ItemToDrop))
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.bNoFail = true;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		const FVector SpawnLocation{ GetActorLocation() + (GetActorForwardVector() * 50.0f) };
		const FTransform SpawnTransform(GetActorRotation(), SpawnLocation);

		const int32 RemovedQuantity = Inventory->RemoveAmountOfItem(ItemToDrop, QuantityToDrop);

		APickup* Pickup = GetWorld()->SpawnActor<APickup>(APickup::StaticClass(), SpawnTransform, SpawnParams);

		Pickup->InitializeDrop(ItemToDrop, RemovedQuantity);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Item to drop was somehow null!"));
	}
}

int32 AContainer::ChangeItemWithOtherInventory(UItemBase* ChangeFrom, UItemBase* ChangeTo, bool check, int32 amount)
{
	return Inventory->ChangeItemWithOtherInventory(ChangeFrom, ChangeTo, check, amount);
}

void AContainer::ChangeItem(UItemBase* ChangeFrom, UItemBase* ChangeTo)
{
	Inventory->ChangeItemOnSameInventory(ChangeFrom, ChangeTo);
}

void AContainer::SplitItem(UItemBase* ItemToSplit, const int32 QuantityToSplit)
{
	Inventory->SplitExistingStack(ItemToSplit, QuantityToSplit);
}

void AContainer::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	const FName ChangedPropertyName = PropertyChangedEvent.Property ? PropertyChangedEvent.Property->GetFName() : NAME_None;

	if (ChangedPropertyName == GET_MEMBER_NAME_CHECKED(FDataTableRowHandle, RowName))
	{
		if (!ContainerRowHandle.IsNull())
		{
			const FContainerData* ItemData = ContainerRowHandle.GetRow<FContainerData>(ContainerRowHandle.RowName.ToString());
			ContainerMesh->SetStaticMesh(ItemData->AssetData.Mesh);
		}
	}
}

void AContainer::BeginPlay()
{
	Super::BeginPlay();

	InitializeContainer(UContainerBase::StaticClass());
}

void AContainer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}