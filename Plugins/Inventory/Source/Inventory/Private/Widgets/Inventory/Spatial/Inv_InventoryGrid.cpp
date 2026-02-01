// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Inventory/Spatial/Inv_InventoryGrid.h"

#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "InventoryManagement/Components/Inv_InventoryComponent.h"
#include "InventoryManagement/Utils/Inv_InventoryStatics.h"
#include "Items/Inv_InventoryItem.h"
#include "Items/Components/Inv_ItemComponent.h"
#include "Items/Fragments/Inv_FragmentTags.h"
#include "Items/Fragments/Inv_ItemFragment.h"
#include "Widgets/Inventory/GridSlots/Inv_GridSlot.h"
#include "Widgets/Inventory/SlottedItems/Inv_SlottedItem.h"
#include "Widgets/Utils/Inv_WidgetUtils.h"

void UInv_InventoryGrid::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	ConstructGrid();
	
	InventoryComponent = UInv_InventoryStatics::GetInventoryComponent(GetOwningPlayer());
	InventoryComponent->OnItemAdded.AddDynamic(this, &UInv_InventoryGrid::AddItem);
}

void UInv_InventoryGrid::ConstructGrid()
{
	GridSlots.Reserve(Rows * Columns);
	
	for (int32 i = 0; i < Rows; ++i)
	{
		for (int32 j = 0; j < Columns; ++j)
		{
			UInv_GridSlot* GridSlot = CreateWidget<UInv_GridSlot>(this, GridSlotClass);
			CanvasPanel->AddChild(GridSlot);
			const FIntPoint TilePosition(j, i);
			GridSlot->SetTileIndex(UInv_WidgetUtils::GetIndexFromPosition(TilePosition, Columns));
			
			UCanvasPanelSlot* GridCPS = UWidgetLayoutLibrary::SlotAsCanvasSlot(GridSlot);
			GridCPS->SetSize(FVector2D(TileSize));
			GridCPS->SetPosition(TilePosition * TileSize);
			
			GridSlots.Add(GridSlot);
		}
	}
}

void UInv_InventoryGrid::AddItem(UInv_InventoryItem* InventoryItem)
{
	if (!MatchesCategory(InventoryItem)) return;
	
	FInv_SlotAvailabilityResult Result = HasRoomForItem(InventoryItem);
	
	AddItemToIndices(Result, InventoryItem);
}

FInv_SlotAvailabilityResult UInv_InventoryGrid::HasRoomForItem(const UInv_ItemComponent* ItemComponent)
{
	return HasRoomForItem(ItemComponent->GetItemManifest());
}

FInv_SlotAvailabilityResult UInv_InventoryGrid::HasRoomForItem(const UInv_InventoryItem* InventoryItem)
{
	return HasRoomForItem(InventoryItem->GetItemManifest());
}

bool UInv_InventoryGrid::HasRoomAtIndex(const UInv_GridSlot* GridSlot, const FIntPoint& Dimensions, const TSet<int32>& OccupiedIndices, TSet<int32>& TentativelyOccupiedIndices)
{
	bool bHasRoomAtIndex = true;
	// Is there room at this index? (i.e., are there other items in the way?)
	UInv_InventoryStatics::ForEach2D(GridSlots, GridSlot->GetTileIndex(), Dimensions, Columns, 
		[&](const UInv_GridSlot* SubGridSlot)
		{
			if (CheckSlotConstrains(GridSlot, SubGridSlot, OccupiedIndices))
			{
				TentativelyOccupiedIndices.Add(SubGridSlot->GetTileIndex());
			}
			else
			{
				bHasRoomAtIndex = false;
			}
		});
	
	return bHasRoomAtIndex;
}

bool UInv_InventoryGrid::CheckSlotConstrains(const UInv_GridSlot* GridSlot, const UInv_GridSlot* SubGridSlot, const TSet<int32>& OccupiedIndices)
{
	// Index claimed? 
	if (OccupiedIndices.Contains(SubGridSlot->GetTileIndex())) return false;
	// Has valid item?
	if (!SubGridSlot->GetInventoryItem().IsValid()) return true;
	// Is this Grid slot an upper left slot?
	if (SubGridSlot->GetUpperLeftIndex() != GridSlot->GetTileIndex()) return false;
	// Is this item the same type as the item we're trying to add?
	// If so, is this a stackable item?
	// If stackable, is this slot at max stack size already?
	return false;
}

FInv_SlotAvailabilityResult UInv_InventoryGrid::HasRoomForItem(const FInv_ItemManifest& Manifest)
{
	FInv_SlotAvailabilityResult Result;
	// Determine if the item is stackable
	const FInv_StackableFragment* StackableFragment = Manifest.GetFragmentOfType<FInv_StackableFragment>();
	Result.bStackable = StackableFragment != nullptr;
	
	// Determine how many stacks to add
	const int32 MaxStackSize = Result.bStackable ? StackableFragment->GetMaxStackSize() : 1;
	int32 AmountToFill = Result.bStackable ? StackableFragment->GetStackCount() : 1;
	
	TSet<int32> OccupiedIndices;
	// For each Grid slot:
	for (const TObjectPtr<UInv_GridSlot>& GridSlot : GridSlots)
	{
		// If we don't have anymore to fill, break out loop early
		if (AmountToFill == 0) break;
		
		// Is this index claimed yet?
		if (OccupiedIndices.Contains(GridSlot->GetTileIndex())) continue;
		
		// Can the item fit here? (i.e., is it out of grid bounds?)
		const FInv_GridFragment* GridFragment = Manifest.GetFragmentOfType<FInv_GridFragment>();
		const FIntPoint Dimensions = GridFragment ? GridFragment->GetGridSize() : FIntPoint(1, 1);
		TSet<int32> TentativeOccupiedIndices;
		if (!HasRoomAtIndex(GridSlot, Dimensions, OccupiedIndices, TentativeOccupiedIndices)) continue;
		OccupiedIndices.Append(TentativeOccupiedIndices);
		
		
		// How much to fill?
		// Update the amount left to fill
	}
		
	// How much is the Remainder?
	
	return Result;
}

void UInv_InventoryGrid::AddItemToIndices(const FInv_SlotAvailabilityResult& Result,
                                          UInv_InventoryItem* NewInventoryItem)
{
	for (const FInv_SlotAvailability& SlotAvailability : Result.SlotAvailabilities)
	{
		AddItemAtIndex(NewInventoryItem, SlotAvailability.Index, Result.bStackable, SlotAvailability.AmountToFill);
		UpdateGridSlots(NewInventoryItem, SlotAvailability.Index, Result.bStackable, SlotAvailability.AmountToFill);
	}
}

bool UInv_InventoryGrid::MatchesCategory(const UInv_InventoryItem* InventoryItem) const
{
	return InventoryItem->GetItemManifest().GetItemCategory() == ItemCategory;
}


FVector2D UInv_InventoryGrid::GetDrawSize(const FInv_GridFragment* GridFragment) const
{
	const float IconTileWidth = TileSize - GridFragment->GetGridPadding() * 2;
	const FVector2D IconSize = GridFragment->GetGridSize() * IconTileWidth;
	return IconSize;
}

void UInv_InventoryGrid::SetSlottedItemImage(const UInv_SlottedItem* SlottedItem, const FInv_GridFragment* GridFragment,
                                             const FInv_ImageFragment* ImageFragment) const
{
	FSlateBrush Brush;
	Brush.SetResourceObject(ImageFragment->GetIcon());
	Brush.DrawAs = ESlateBrushDrawType::Image;
	Brush.ImageSize = GetDrawSize(GridFragment);
	SlottedItem->SetImageBrush(Brush);
}

void UInv_InventoryGrid::AddItemAtIndex(UInv_InventoryItem* NewInventoryItem, int32 Index, const bool bStackable,
                                        const int32 StackAmount)
{
	// Get Grid Fragment so we know how many grid spaces the item takes
	const FInv_GridFragment* GridFragment = GetFragment<FInv_GridFragment>(NewInventoryItem, FragmentTags::GridFragment);
	// Get Image Fragment so we have the icon to display
	const FInv_ImageFragment* ImageFragment = GetFragment<FInv_ImageFragment>(NewInventoryItem, FragmentTags::IconFragment);
	if (!GridFragment || !ImageFragment) return;
	// Create a widget to add to the grid
	UInv_SlottedItem* SlottedItem = CreateSlottedItem(NewInventoryItem, Index, GridFragment, ImageFragment, bStackable, StackAmount);
	// Add slotted item to the canvas panel
	AddSlottedItemToCanvas(Index, GridFragment, SlottedItem);
	// Store the new widget in a container
	SlottedItems.Add(Index, SlottedItem);
}

UInv_SlottedItem* UInv_InventoryGrid::CreateSlottedItem(UInv_InventoryItem* NewInventoryItem,
                                                        const int32 Index, const FInv_GridFragment* GridFragment, 
                                                        const FInv_ImageFragment* ImageFragment, const bool bStackable, 
                                                        const int32 StackAmount) const
{
	UInv_SlottedItem* SlottedItem = CreateWidget<UInv_SlottedItem>(GetOwningPlayer(), SlottedItemClass);
	SlottedItem->SetInventoryItem(NewInventoryItem);
	SetSlottedItemImage(SlottedItem, GridFragment, ImageFragment);
	SlottedItem->SetGridIndex(Index);
	SlottedItem->SetIsStackable(bStackable);
	const int32 StackCount = bStackable ? StackAmount : 0;
	SlottedItem->SetStackCount(StackCount);
	return SlottedItem;
}

void UInv_InventoryGrid::AddSlottedItemToCanvas(const int32 Index, const FInv_GridFragment* GridFragment,
                                                UInv_SlottedItem* SlottedItem) const
{
	CanvasPanel->AddChild(SlottedItem);
	UCanvasPanelSlot* CanvasSlot = UWidgetLayoutLibrary::SlotAsCanvasSlot(SlottedItem);
	CanvasSlot->SetSize(GetDrawSize(GridFragment));
	const FVector2D DrawPosition = UInv_WidgetUtils::GetPositionFromIndex(Index, Columns) * TileSize;
	const FVector2D DrawPositionWithPadding = DrawPosition + FVector2D(GridFragment->GetGridPadding());
	CanvasSlot->SetPosition(DrawPositionWithPadding);
}



void UInv_InventoryGrid::UpdateGridSlots(UInv_InventoryItem* NewInventoryItem, const int32 Index, const bool bStackable, const int32 StackAmount)
{
	check(GridSlots.IsValidIndex(Index));
	
	if (bStackable)
	{
		GridSlots[Index]->SetStackCount(StackAmount);
	}
	
	
	const FInv_GridFragment* GridFragment = GetFragment<FInv_GridFragment>(NewInventoryItem, FragmentTags::GridFragment);
	const FIntPoint Dimensions = GridFragment ? GridFragment->GetGridSize() : FIntPoint(1, 1);
	
	UInv_InventoryStatics::ForEach2D(GridSlots, Index, Dimensions, Columns, [&](UInv_GridSlot* GridSlot)
	{
		GridSlot->SetInventoryItem(NewInventoryItem);
		GridSlot->SetUpperLeftIndex(Index);
		GridSlot->SetOccupiedTexture();
		GridSlot->SetAvailable(false);
	});
}
