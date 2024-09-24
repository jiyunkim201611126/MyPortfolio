#include "NameTagWidgetComponent.h"

void UNameTagWidgetComponent::LookCamera(FVector CameraLocation)
{
	FVector WidgetLocation = this->GetComponentLocation();

	FVector LookAtDirection = CameraLocation - WidgetLocation;

	FRotator WidgetRotation = FRotationMatrix::MakeFromX(LookAtDirection).Rotator();
	this->SetWorldRotation(WidgetRotation);
}
