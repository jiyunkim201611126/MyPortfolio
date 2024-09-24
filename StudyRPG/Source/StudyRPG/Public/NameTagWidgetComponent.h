#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "NameTagWidgetComponent.generated.h"

UCLASS()
class STUDYRPG_API UNameTagWidgetComponent : public UWidgetComponent
{
	GENERATED_BODY()

public:
	void LookCamera(FVector CameraLocation);
};
