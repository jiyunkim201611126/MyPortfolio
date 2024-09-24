#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Components/SphereComponent.h"
#include "InteractionInterface.generated.h"

class AMainCharacter;

UENUM()
enum class EInteractableType : uint8
{
	Pickup UMETA(DisplayName = "Pickup"),
	NonPlayerCharacter UMETA(DisplayName = "NonPlayerCharacter"),
	Device UMETA(DisplayName = "Device"),
	Toggle UMETA(DisplayName = "Toggle"),
	Container UMETA(DisplayName = "Container")
};

USTRUCT(Blueprintable)
struct FInteractableData
{
	GENERATED_USTRUCT_BODY()

	// 구조체 생성자. 별다른 초기화 없이 생성 시 아래대로 초기화됨
	FInteractableData() :
		InteractableType(EInteractableType::Pickup),
		Name(FText::GetEmpty()),
		Action(FText::GetEmpty()),
		Quantity(0),
		InteractionDuration(0.0f)
	{};

	UPROPERTY(EditInstanceOnly)
	EInteractableType InteractableType;

	UPROPERTY(EditInstanceOnly)
	FText Name;

	UPROPERTY(EditInstanceOnly)
	FText Action;

	// Pickup에만 쓰임
	UPROPERTY(EditInstanceOnly)
	int32 Quantity;

	// 상호작용할 때 걸리는 시간
	// 몇 초간 꾹 눌러야 하는 거 구현할 때 쓰임, 기본값 0초
	UPROPERTY(EditInstanceOnly)
	float InteractionDuration;
};

UINTERFACE(MinimalAPI)
class UInteractionInterface : public UInterface
{
	GENERATED_BODY()
};

class STUDYRPG_API IInteractionInterface
{
	GENERATED_BODY()
	
public:
	virtual void BeginFocus();
	virtual void EndFocus();
	virtual void BeginInteract();
	virtual void EndInteract();
	virtual void Interact(AMainCharacter* playerCharacter);

	/*
	인터페이스는 속성이 없는 클래스기 때문에 UPROPERTY를 사용할 수 없음
	상속받을 클래스가 InstanceInteractableData라는 이름으로 한 번 더 선언해서
	데이터를 받은 뒤 아래 구조체에 할당해줄 것
	상속받은 클래스는 UPROPERTY를 쓸 수 있음
	*/
	FInteractableData InteractableData;

	// 구조체와 관계 없는 상호작용 검출용 콜리전, Trace Channel 'Interaction'
	USphereComponent* InteractionCollision;
};
