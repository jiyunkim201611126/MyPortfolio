#pragma once

#include "CoreMinimal.h"
#include "ItemEffect/ItemEffectBase.h"
#include "IE_002_EquipWeapon.generated.h"

UCLASS()
class STUDYRPG_API AIE_002_EquipWeapon : public AItemEffectBase
{
	GENERATED_BODY()
public:
	AIE_002_EquipWeapon();

	virtual void ItemEffect(UItemBase* ItemToUse, AMainCharacter* character) override;

protected:
};
