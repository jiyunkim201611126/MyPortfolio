#include "ItemEffect/IE/IE_000_DamagePlayer.h"

void AIE_000_DamagePlayer::ItemEffect(UItemBase* ItemToUse, AMainCharacter* character)
{
	character->CharacterTakeDamage(ItemToUse->ItemStatistics.DamageValue);

	Destroy();
}
