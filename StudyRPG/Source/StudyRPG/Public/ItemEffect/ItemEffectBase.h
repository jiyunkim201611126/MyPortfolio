#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemBase.h"
#include "MainCharacter.h"
#include "ItemEffectBase.generated.h"

UCLASS(Blueprintable)
class STUDYRPG_API AItemEffectBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItemEffectBase();

	virtual void ItemEffect(UItemBase* ItemToUse, AMainCharacter* character);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

};
