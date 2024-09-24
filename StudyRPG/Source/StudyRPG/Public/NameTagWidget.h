#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NameTagWidget.generated.h"

class UTextBlock;

UCLASS()
class STUDYRPG_API UNameTagWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	// 플레이어 네임 태그, 블루프린트상에서 같은 이름으로 추가
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* playerName;

public:
	void ChangePlayerName(FText name);
};