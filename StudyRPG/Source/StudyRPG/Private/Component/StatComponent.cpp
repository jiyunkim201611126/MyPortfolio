#include "Component/StatComponent.h"

UStatComponent::UStatComponent()
{
}

// 매개변수가 음수로 들어오면 회복임
float UStatComponent::TakeDamage(float damage)
{
	// 회복 받음
	if (damage < 0.f)
	{
		currentHPValue = FMath::Clamp(currentHPValue - damage, 0.f, maxHPValue);

		OnStatUpdated.Broadcast();
		return damage;
	}
	// 딜 받음
	else if (damage > 0.f)
	{
		currentHPValue -= damage;
		if (currentHPValue <= 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("Player is Dead"));
		}

		OnStatUpdated.Broadcast();
		return damage;
	}

	return 0.f;
}

void UStatComponent::BeginPlay()
{
	Super::BeginPlay();

	//OnStatUpdated.Broadcast();
}
