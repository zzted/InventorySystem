// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Types/Inv_GridTypes.h"
#include "Inv_InventoryGrid.generated.h"

class UInv_HoverItem;
struct FGameplayTag;
struct FInv_ImageFragment;
struct FInv_GridFragment;
struct FInv_ItemManifest;
class UInv_ItemComponent;
class UInv_InventoryComponent;
class UCanvasPanel;
class UInv_GridSlot;
class UInv_SlottedItem;
/**
 * 
 */
UCLASS()
class INVENTORY_API UInv_InventoryGrid : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeOnInitialized() override;
	
	EInv_ItemCategory GetItemCategory() const {return ItemCategory;}
	FInv_SlotAvailabilityResult HasRoomForItem(const UInv_ItemComponent* ItemComponent);
	
	
	UFUNCTION()
	void AddItem(UInv_InventoryItem* InventoryItem);
	
	UFUNCTION()
	void OnSlottedItemClicked(int32 GridIndex, const FPointerEvent& MouseEvent);

private:
	
	TWeakObjectPtr<UInv_InventoryComponent> InventoryComponent;
	
	void ConstructGrid();
	FInv_SlotAvailabilityResult HasRoomForItem(const UInv_InventoryItem* InventoryItem);
	FInv_SlotAvailabilityResult HasRoomForItem(const FInv_ItemManifest& Manifest);
	
	void AddItemToIndices(const FInv_SlotAvailabilityResult& Result, UInv_InventoryItem* NewInventoryItem);
	bool MatchesCategory(const UInv_InventoryItem* InventoryItem) const;
	FVector2D GetDrawSize(const FInv_GridFragment* GridFragment) const;
	void SetSlottedItemImage(const UInv_SlottedItem* SlottedItem, const FInv_GridFragment* GridFragment, const FInv_ImageFragment* ImageFragment) const;
	void AddItemAtIndex(UInv_InventoryItem* NewInventoryItem, int32 Index, const bool bStackable, const int32 StackAmount);
	UInv_SlottedItem* CreateSlottedItem(UInv_InventoryItem* NewInventoryItem, int32 Index,
	                                                 const FInv_GridFragment* GridFragment,
	                                                 const FInv_ImageFragment* ImageFragment, 
	                                                 const bool bStackable, 
	                                                 const int32 StackAmount) const;
	
	void AddSlottedItemToCanvas(const int32 Index, const FInv_GridFragment* GridFragment, UInv_SlottedItem* SlottedItem) const;
	void UpdateGridSlots(UInv_InventoryItem* NewInventoryItem, const int32 Index, bool bStackable, const int32 StackAmount);
	bool HasRoomAtIndex(const UInv_GridSlot* GridSlot, const FIntPoint& Dimensions, const TSet<int32>& OccupiedIndices,
	                    TSet<int32>& TentativelyOccupiedIndices, const FGameplayTag& ItemType, const int32 MaxStackSize);
	bool CheckSlotConstrains(const UInv_GridSlot* GridSlot, const UInv_GridSlot* SubGridSlot, const TSet<int32>& OccupiedIndices, const FGameplayTag& ItemType, const int32 MaxStackSize);
	bool IsInGridBounds(const int32 StartIndex, const FIntPoint& ItemDimensions) const;
	int32 DetermineFillAmountForSlot(const bool bStackable, const int32 MaxStackSize, const int32 AmountToFill, const UInv_GridSlot* GridSlot);
	void CreateHoverItemWidget(int32 GridIndex, UInv_InventoryItem* ClickedInventoryItem);
	void RemoveItemFromGrid(int32 GridIndex, UInv_InventoryItem* ClickedInventoryItem);
	
	UFUNCTION()
	void AddStacks(const FInv_SlotAvailabilityResult& Result);
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category="Inventory")
	EInv_ItemCategory ItemCategory;
	
	UPROPERTY()
	TArray<TObjectPtr<UInv_GridSlot>> GridSlots;
	
	UPROPERTY(EditAnywhere, Category="Inventory")
	TSubclassOf<UInv_GridSlot> GridSlotClass;
	
	UPROPERTY(EditAnywhere, Category="Inventory")
	TSubclassOf<UInv_SlottedItem> SlottedItemClass;
	
	UPROPERTY()
	TMap<int32 /*Grid Index*/, TObjectPtr<UInv_SlottedItem>> SlottedItems;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCanvasPanel> CanvasPanel;
	
	UPROPERTY(EditAnywhere, Category="Inventory")
	int32 Rows;
	
	UPROPERTY(EditAnywhere, Category="Inventory")
	int32 Columns;
	
	UPROPERTY(EditAnywhere, Category="Inventory")
	float TileSize;
	
	UPROPERTY(EditAnywhere, Category="Inventory")
	TSubclassOf<UInv_HoverItem> HoverItemClass;
	
	UPROPERTY()
	TObjectPtr<UInv_HoverItem> HoverItem;
};
