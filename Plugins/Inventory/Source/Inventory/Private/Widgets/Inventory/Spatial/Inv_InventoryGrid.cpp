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
#include "Widgets/Utils/Inv_WidgetUtils.h"

void UInv_InventoryGrid::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	ConstructGrid();
	
	InventoryComponent = UInv_InventoryStatics::GetInventoryComponent(GetOwningPlayer());
	InventoryComponent->OnItemAdded.AddDynamic(this, &UInv_InventoryGrid::AddItem);
}

FInv_SlotAvailabilityResult UInv_InventoryGrid::HasRoomForItem(const UInv_ItemComponent* ItemComponent)
{
	return HasRoomForItem(ItemComponent->GetItemManifest());
}

FInv_SlotAvailabilityResult UInv_InventoryGrid::HasRoomForItem(const UInv_InventoryItem* InventoryItem)
{
	return HasRoomForItem(InventoryItem->GetItemManifest());
}

FInv_SlotAvailabilityResult UInv_InventoryGrid::HasRoomForItem(const FInv_ItemManifest& Manifest)
{
	FInv_SlotAvailabilityResult Result;
	Result.TotalRoomToFill = 1;
	
	FInv_SlotAvailability SlotAvailability;
	SlotAvailability.AmountToFill = 1;
	SlotAvailability.Index = 0;
	Result.SlotAvailabilities.Add(MoveTemp(SlotAvailability));
	
	return Result;
}

void UInv_InventoryGrid::AddItem(UInv_InventoryItem* InventoryItem)
{
	if (!MatchesCategory(InventoryItem)) return;
	
	FInv_SlotAvailabilityResult Result = HasRoomForItem(InventoryItem);
	
	AddItemToIndices(Result, InventoryItem);
}

void UInv_InventoryGrid::AddItemToIndices(const FInv_SlotAvailabilityResult& Result,
	UInv_InventoryItem* NewInventoryItem)
{
	// Get Grid Fragment so we know how many grid spaces the item takes
	const FInv_GridFragment* GridFragment = GetFragment<FInv_GridFragment>(NewInventoryItem, FragmentTags::GridFragment);
	// Get Image Fragment so we have the icon to display
	const FInv_ImageFragment* ImageFragment = GetFragment<FInv_ImageFragment>(NewInventoryItem, FragmentTags::IconFragment);
	if (!GridFragment || !ImageFragment) return;
	
	
	// Create widget to add to the grid
	// Store the new widget in a container
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



bool UInv_InventoryGrid::MatchesCategory(const UInv_InventoryItem* InventoryItem) const
{
	return InventoryItem->GetItemManifest().GetItemCategory() == ItemCategory;
}
