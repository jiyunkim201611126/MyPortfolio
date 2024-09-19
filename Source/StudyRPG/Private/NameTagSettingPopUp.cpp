#include "NameTagSettingPopUp.h"
#include "Kismet/GameplayStatics.h"
#include "MainCharacter.h"

void UNameTagSettingPopUp::ChangePlayerName()
{
	APlayerController* playerController = Cast<APlayerController>(UGameplayStatics::GetPlayerController(this, 0));
	AMainCharacter* mainCharacter = Cast<AMainCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));

	playerController->SetShowMouseCursor(false);
	mainCharacter->ChangePlayerNameTag(GetInputText());
	RemoveFromParent();
}

void UNameTagSettingPopUp::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	confirmButton = Cast<UButton>(GetWidgetFromName(TEXT("confirmButton")));

	confirmButton->OnClicked.AddDynamic(this, &UNameTagSettingPopUp::ChangePlayerName);
}