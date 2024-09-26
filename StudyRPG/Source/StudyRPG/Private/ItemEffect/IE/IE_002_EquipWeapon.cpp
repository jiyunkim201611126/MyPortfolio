#include "ItemEffect/IE/IE_002_EquipWeapon.h"

AIE_002_EquipWeapon::AIE_002_EquipWeapon()
{

}

void AIE_002_EquipWeapon::ItemEffect(UItemBase* ItemToUse, AMainCharacter* character)
{
	character->SetWeapon(ItemToUse->AssetData.Mesh);
}
