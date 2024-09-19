#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractionInterface.h"
#include "Pickup.generated.h"

class UItemBase;

UCLASS()
class STUDYRPG_API APickup : public AActor, public IInteractionInterface
{
	GENERATED_BODY()
	
public:	
	APickup();

	void InitializePickup(const TSubclassOf<UItemBase> BaseClass, const int32 InQuantity);

	void InitializeDrop(UItemBase* ItemToDrop, const int32 InQuantity);

	FORCEINLINE UItemBase* GetItemData() { return ItemReference; }

	virtual void BeginFocus() override;
	virtual void EndFocus() override;

protected:
	// 액터 디테일 패널에서 DesiredItemID을 기입 (미리 월드에 배치할 필요 있음)
	// 그 위 ItemDataTable도 할당해주면 일치하는 컬럼의 정보들을 가져와
	// 모든 구조체 속 변수를 초기화함

	UPROPERTY(VisibleAnywhere, Category = "Pickup | Components")
	UStaticMeshComponent* PickupMesh;

	UPROPERTY(VisibleAnywhere, Category = "Pickup | Item Reference")
	UItemBase* ItemReference;

	// 부모 인터페이스에 있는 InteractableData를 인스턴스 디테일 패널에 가시화
	UPROPERTY(VisibleInstanceOnly, Category = "Pickup | Interaction")
	FInteractableData InstanceInteractableData;

	UPROPERTY(EditInstanceOnly, Category = "Pickup | Item Initialization")
	int32 ItemQuantity;

	UPROPERTY(EditInstanceOnly, Category = "Pickup | Item Initialization")
	FDataTableRowHandle ItemRowHandle;


	virtual void BeginPlay() override;

	virtual void Interact(AMainCharacter* PlayerCharacter) override;
	void UpdateInteractableData();

	void TakePickup(const AMainCharacter* Taker);

	// 게임 내가 아닌 에디터에서 실행되는 함수. DesiredItemID에 변동사항이 있을 경우 에디터에서 반영해줌
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};
