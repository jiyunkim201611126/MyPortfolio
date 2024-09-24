#pragma once

#include "CoreMinimal.h"
#include "ItemEffect/ItemEffectBase.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "IE_001_ThrowItem.generated.h"

UCLASS()
class STUDYRPG_API AIE_001_ThrowItem : public AItemEffectBase
{
	GENERATED_BODY()
public:
	AIE_001_ThrowItem();

	virtual void ItemEffect(UItemBase* ItemToUse, AMainCharacter* character) override;

protected:
	// 액터를 탄환처럼 날아가게 해주는 컴포넌트
	UProjectileMovementComponent* ProjectileMovement;

	UStaticMeshComponent* ThrownItemMesh;
};
