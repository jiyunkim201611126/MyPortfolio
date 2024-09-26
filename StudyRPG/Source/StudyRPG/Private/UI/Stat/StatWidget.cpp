#include "UI/Stat/StatWidget.h"

void UStatWidget::SetStatComponent(UStatComponent* OwnerStatComponent)
{
	StatReference = OwnerStatComponent;

	StatReference->OnStatUpdated.AddUObject(this, &UStatWidget::RefreshStatWidget);
	
	RefreshStatWidget();
}

void UStatWidget::RefreshStatWidget()
{
	UE_LOG(LogTemp, Warning, TEXT("Player HP Bar Refreshed"));
	HPProgressBar->SetPercent(StatReference->GetCurrentHPValue() / StatReference->GetMaxHPValue());
}
