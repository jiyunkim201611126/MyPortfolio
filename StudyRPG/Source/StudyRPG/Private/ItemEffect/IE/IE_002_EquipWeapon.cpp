#include "ItemEffect/IE/IE_002_EquipWeapon.h"

void AIE_002_EquipWeapon::ItemEffect(UItemBase* ItemToUse, AMainCharacter* character)
{
	character->bWeaponEquip = true;
	character->bUseControllerRotationYaw = true;
	character->SetWeapon(ItemToUse->AssetData.Mesh);
	character->GetMesh()->SetAnimClass(WeaponAnimClass);
}
