// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "INV_Highlightable.h"
#include "Components/StaticMeshComponent.h"
#include "INV_HighlightableStaticMesh.generated.h"

/**
 * 
 */
UCLASS()
class INVENTORY_API UINV_HighlightableStaticMesh : public UStaticMeshComponent, public IINV_Highlightable
{
	GENERATED_BODY()
public:
	/*Highlightable Interface*/
	virtual void Highlight_Implementation() override;
	virtual void UnHighlight_Implementation() override;
	/*End Highlightable Interface*/
	
private:
	UPROPERTY(EditAnywhere, Category="Inventory")
	TObjectPtr<UMaterialInterface> HighlightMaterial;;
	
};
