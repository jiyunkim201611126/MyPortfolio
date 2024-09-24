#include "InterfaceTestActor.h"

// Sets default values
AInterfaceTestActor::AInterfaceTestActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");

	SetRootComponent(Mesh);
}

void AInterfaceTestActor::BeginPlay()
{
	Super::BeginPlay();
	
	InteractableData = InstanceInteractableData;
}

// Called every frame
void AInterfaceTestActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AInterfaceTestActor::BeginFocus()
{
	if (Mesh)
	{
		Mesh->SetRenderCustomDepth(true);
	}
}

void AInterfaceTestActor::EndFocus()
{
	if (Mesh)
	{
		Mesh->SetRenderCustomDepth(false);
	}
}

void AInterfaceTestActor::BeginInteract()
{
	UE_LOG(LogTemp, Warning, TEXT("Calling BeginInteract"));
	UE_LOG(LogTemp, Warning, TEXT("%f 초 이후 Interact()를 실행합니다."), InteractableData.InteractionDuration);
}

void AInterfaceTestActor::EndInteract()
{
	UE_LOG(LogTemp, Warning, TEXT("Calling EndInteract"));
}

void AInterfaceTestActor::Interact(AMainCharacter* playerCharacter)
{
	UE_LOG(LogTemp, Warning, TEXT("Calling Interact"));
	UE_LOG(LogTemp, Warning, TEXT("%f 초 전 BeginInteract()가 실행되었습니다."), InteractableData.InteractionDuration);
}

