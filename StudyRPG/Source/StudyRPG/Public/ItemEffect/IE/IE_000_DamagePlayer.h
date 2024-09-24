#pragma once

#include "CoreMinimal.h"
#include "ItemEffect/ItemEffectBase.h"
#include "IE_000_DamagePlayer.generated.h"

UCLASS()
class STUDYRPG_API AIE_000_DamagePlayer : public AItemEffectBase
{
	GENERATED_BODY()

	virtual void ItemEffect(UItemBase* ItemToUse, AMainCharacter* character) override;
};
