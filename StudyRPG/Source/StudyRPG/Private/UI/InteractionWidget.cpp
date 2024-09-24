#include "UI/InteractionWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "InteractionInterface.h"

void UInteractionWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	InteractionProgressBar->PercentDelegate.BindUFunction(this, "UpdateInteractionProgress");
}

void UInteractionWidget::NativeConstruct()
{
	Super::NativeConstruct();

	currentInteractionDuration = 0.0f;
}

void UInteractionWidget::UpdateWidget(const FInteractableData* InteractableData) const
{
	// 상호작용 타입에 따라 switch 구문으로 적절한 문장 생성
	switch (InteractableData->InteractableType)
	{
	case EInteractableType::Pickup:
		KeyPressText->SetText(FText::FromString("Press"));
		InteractionProgressBar->SetVisibility(ESlateVisibility::Collapsed);

		if (InteractableData->Quantity < 2)
		{
			QuantityText->SetVisibility(ESlateVisibility::Collapsed);
		}
		else
		{
			QuantityText->SetText(FText::Format(NSLOCTEXT("InteractionWidget", "QuantityText", "x {0}"),
				InteractableData->Quantity));
			QuantityText->SetVisibility(ESlateVisibility::Visible);
		}

		break;
	case EInteractableType::NonPlayerCharacter:
		break;
	case EInteractableType::Device:
		break;
	case EInteractableType::Toggle:
		break;
	case EInteractableType::Container:
		KeyPressText->SetText(FText::FromString("Press"));
		InteractionProgressBar->SetVisibility(ESlateVisibility::Collapsed);
		QuantityText->SetVisibility(ESlateVisibility::Collapsed);
		break;
	default: ;
	}

	ActionText->SetText(InteractableData->Action);
	NameText->SetText(InteractableData->Name);
}

float UInteractionWidget::UpdateInteractionProgress()
{
	return 0.0f;
}
