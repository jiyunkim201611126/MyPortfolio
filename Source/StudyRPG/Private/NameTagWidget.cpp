#include "NameTagWidget.h"
#include "Components/TextBlock.h"

void UNameTagWidget::ChangePlayerName(FText name)
{
	playerName->SetText(name);
}