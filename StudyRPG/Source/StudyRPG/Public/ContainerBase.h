// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ContainerBase.generated.h"

/**
 * 
 */
USTRUCT()
struct FContainerTextData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere)
	FText Name;

	UPROPERTY(EditAnywhere)
	FText InteractionText;
};

USTRUCT()
struct FContainerNumericData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere)
	int32 SlotsCapacity;

	UPROPERTY(EditAnywhere)
	int32 WeightCapacity;
};

USTRUCT()
struct FContainerAssetData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere)
	UStaticMesh* Mesh;
};

USTRUCT()
struct FContainerData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, Category = "Item Data")
	FName ID;

	UPROPERTY(EditAnywhere, Category = "Container Data")
	FContainerTextData TextData;

	UPROPERTY(EditAnywhere, Category = "Container Data")
	FContainerNumericData NumericData;

	UPROPERTY(EditAnywhere, Category = "Container Data")
	FContainerAssetData AssetData;
};

UCLASS()
class STUDYRPG_API UContainerBase : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, Category = "Container Data")
	FName ID;

	UPROPERTY(EditAnywhere, Category = "Container Data")
	FContainerTextData TextData;

	UPROPERTY(EditAnywhere, Category = "Container Data")
	FContainerNumericData NumericData;

	UPROPERTY(EditAnywhere, Category = "Container Data")
	FContainerAssetData AssetData;
};
