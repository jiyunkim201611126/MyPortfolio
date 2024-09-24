#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/InputTextPopupWidget.h"
#include "NameTagSettingPopUp.generated.h"

class UTextBlock;
class UButton;
class AMainCharacter;

UCLASS()
class STUDYRPG_API UNameTagSettingPopUp : public UInputTextPopupWidget
{
	GENERATED_BODY()

protected:

	virtual void NativeOnInitialized();

public:
	UFUNCTION()
	void ChangePlayerName();
};