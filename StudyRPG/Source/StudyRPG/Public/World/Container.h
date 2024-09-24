#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractionInterface.h"
#include "Component/InventoryOwnerInterface.h"
#include "Container.generated.h"

class USphereComponent;
class UContainerBase;
class UInventoryComponent;
class AMainCharacter;

UCLASS()
class STUDYRPG_API AContainer : public AActor, public IInteractionInterface, public IInventoryOwnerInterface
{
	GENERATED_BODY()
	
public:	
	AContainer();

	void InitializeContainer(const TSubclassOf<UContainerBase> BaseClass);

	void UpdateInteractableData();

	virtual void DropItem(UItemBase* ItemToDrop, const int32 QuantityToDrop) override;
	virtual int32 ChangeItemWithOtherInventory(UItemBase* ChangeFrom, UItemBase* ChangeTo, bool check, int32 amount) override;
	virtual void ChangeItem(UItemBase* ChangeFrom, UItemBase* ChangeTo) override;
	virtual void SplitItem(UItemBase* ItemToSplit, const int32 QuantityToSplit) override;

	virtual void Tick(float DeltaTime) override;

protected:

	UPROPERTY(VisibleAnywhere, Category = "Container | Components")
	UStaticMeshComponent* ContainerMesh;

	// 부모 인터페이스에 있는 InteractableData를 인스턴스 디테일 패널에 가시화
	UPROPERTY(VisibleInstanceOnly, Category = "Container | Interaction")
	FInteractableData InstanceInteractableData;

	UPROPERTY(VisibleAnywhere, Category = "Container | Inventory")
	UInventoryComponent* ContainerInventory;

	UPROPERTY(VisibleAnywhere, Category = "Container | Container Reference")
	UContainerBase* ContainerReference;

	UPROPERTY(EditInstanceOnly, Category = "Container | Container Initialization")
	FDataTableRowHandle ContainerRowHandle;

	virtual void BeginFocus() override;
	virtual void EndFocus() override;

	virtual void Interact(AMainCharacter* PlayerCharacter) override;
	void OpenContainerInventory(AMainCharacter* PlayerCharacter);

	virtual void BeginPlay() override;

	// 게임 내가 아닌 에디터에서 실행되는 함수. DesiredItemID에 변동사항이 있을 경우 에디터에서 반영해줌
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};
