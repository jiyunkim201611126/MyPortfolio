#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/EditableText.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "InputTextPopupWidget.generated.h"

UCLASS()
class STUDYRPG_API UInputTextPopupWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	// 설명 텍스트 블럭, 블루프린트상에서 같은 이름으로 추가
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* descriptionText;

	// 확인 버튼
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (BindWidget))
	UButton* confirmButton;

	// 확인 버튼 내 텍스트
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* buttonText;

public:
	// 입력란
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (BindWidget))
	UEditableText* inputText;

	FORCEINLINE FText GetInputText() { return inputText->GetText(); };
};
