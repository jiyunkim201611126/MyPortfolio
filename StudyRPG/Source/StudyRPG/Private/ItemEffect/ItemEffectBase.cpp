#include "ItemEffect/ItemEffectBase.h"

// Sets default values
AItemEffectBase::AItemEffectBase()
{

}

// Called when the game starts or when spawned
void AItemEffectBase::BeginPlay()
{
	Super::BeginPlay();
}

void AItemEffectBase::ItemEffect(UItemBase* ItemToUse, AMainCharacter* character)
{
	UE_LOG(LogTemp, Warning, TEXT("Item Used!"));

	Destroy();
}
