#include "ItemEffect/IE/IE_001_ThrowItem.h"

AIE_001_ThrowItem::AIE_001_ThrowItem()
{
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->InitialSpeed = 1000.0f;
	ProjectileMovement->MaxSpeed = 1000.0f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;

	ThrownItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
}

void AIE_001_ThrowItem::ItemEffect(UItemBase* ItemToUse, AMainCharacter* character)
{
	ThrownItemMesh->SetStaticMesh(ItemToUse->AssetData.Mesh);

	if (ProjectileMovement)
	{
		ProjectileMovement->Velocity = character->GetActorForwardVector() * 1000.0f;
		ProjectileMovement->Activate();
	}
}
