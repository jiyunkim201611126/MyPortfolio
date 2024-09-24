#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Component/StatComponent.h"
#include "Components/ProgressBar.h"
#include "StatWidget.generated.h"

UCLASS()
class STUDYRPG_API UStatWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetStatComponent(UStatComponent* OwnerStatComponent);

	void RefreshStatWidget();

protected:
	UStatComponent* StatReference;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "Stat")
	UProgressBar* HPProgressBar;
};
