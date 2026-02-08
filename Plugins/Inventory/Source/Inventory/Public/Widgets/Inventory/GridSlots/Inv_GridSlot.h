// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Inv_GridSlot.generated.h"

class UInv_InventoryItem;
class UImage;

enum class EInv_GridSlotState : uint8
{
	Unoccupied,
	Occupied,
	Selected,
	GrayedOut
};

UCLASS()
class INVENTORY_API UInv_GridSlot : public UUserWidget
{
	GENERATED_BODY()
public:
	void SetTileIndex(const int32 Index) {TileIndex = Index;}
	int32 GetTileIndex() const { return TileIndex; }
	EInv_GridSlotState GetGridSlotState() const { return GridSlotState; }
	TWeakObjectPtr<UInv_InventoryItem> GetInventoryItem() const { return InventoryItem; }
	void SetInventoryItem(UInv_InventoryItem* Item);
	int32 GetUpperLeftIndex() const { return UpperLeftIndex; }
	void SetUpperLeftIndex(const int32 Index) { UpperLeftIndex = Index; }
	int32 GetStackCount() const { return StackCount; }
	void SetStackCount(const int32 Count) { StackCount = Count; }
	bool IsAvailable() const { return bAvailable; }
	void SetAvailable(const bool bIsAvailable) { bAvailable = bIsAvailable; }
	
	void SetOccupiedTexture();
	void SetGrayedOutTexture();
	void SetSelectedTexture();
	void SetUnoccupiedTexture();
	
	
private:
	int32 TileIndex = INDEX_NONE;
	int32 StackCount = 0;
	int32 UpperLeftIndex = INDEX_NONE;
	bool bAvailable = true;
	TWeakObjectPtr<UInv_InventoryItem> InventoryItem;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_GridSlot;
	
	UPROPERTY(EditAnywhere, Category="Inventory")
	FSlateBrush Brush_Unoccupied;
	
	UPROPERTY(EditAnywhere, Category="Inventory")
	FSlateBrush Brush_Occupied;
	
	UPROPERTY(EditAnywhere, Category="Inventory")
	FSlateBrush Brush_Selected;
	
	UPROPERTY(EditAnywhere, Category="Inventory")
	FSlateBrush Brush_GrayedOut;
	
	EInv_GridSlotState GridSlotState;
	
	
};
