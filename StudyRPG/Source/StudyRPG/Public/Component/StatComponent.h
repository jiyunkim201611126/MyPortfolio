#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StatComponent.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnStatUpdated);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class STUDYRPG_API UStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	FOnStatUpdated OnStatUpdated;

	UStatComponent();

	// CharacterMovement 관련
	FORCEINLINE void SetWalkSpeed(float walkSpd) { walkSpeed = walkSpd; };
	FORCEINLINE void SetRunSpeed(float runSpd) { runSpeed = runSpd; };
	FORCEINLINE void SetAimingWalkSpeed(float aimingWalkSpd) { aimingWalkSpeed = aimingWalkSpd; };

	FORCEINLINE float GetWalkSpeed() { return walkSpeed; };
	FORCEINLINE float GetRunSpeed() { return runSpeed; };
	FORCEINLINE float GetAimingWalkSpeed() { return aimingWalkSpeed; };

	// Stat 관련
	FORCEINLINE void SetMaxHPValue(float maxHPVal) { maxHPValue = maxHPVal; };
	FORCEINLINE void SetCurrentHPValue(float currnetHPVal) { currentHPValue = currnetHPVal; };

	FORCEINLINE float GetMaxHPValue() { return maxHPValue; };
	FORCEINLINE float GetCurrentHPValue() { return currentHPValue; };

	float TakeDamage(float damage);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
	float walkSpeed;
	float runSpeed;
	float aimingWalkSpeed;

	UPROPERTY(VisibleAnywhere, Category = "Stat")
	float maxHPValue;

	UPROPERTY(VisibleAnywhere, Category = "Stat")
	float currentHPValue;
};
