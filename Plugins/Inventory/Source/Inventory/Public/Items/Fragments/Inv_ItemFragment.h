#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Inv_ItemFragment.generated.h"

/**
 * Why we use FInstancedStruct instead of UCLASS for Item Fragments:
 *
 * FInstancedStruct is designed for data-oriented, value-type polymorphism,
 * whereas UCLASS is designed for UObject-based objects with complex lifecycles.
 *
 * Key reasons for using FInstancedStruct:
 * 
 * 1. Value Semantics and Copying:
 *    - InstancedStruct supports copying by value, which is essential for
 *      items/fragments that are duplicated frequently (e.g., inventory items,
 *      undo/redo operations, serialization).
 *    - UCLASS instances are always referenced by pointer, so copying a UObject
 *      requires duplication logic and can lead to shared-state bugs.
 *
 * 2. Low Overhead and Performance:
 *    - InstancedStruct does not participate in the Garbage Collector, so
 *      allocation and destruction are lightweight and predictable.
 *    - UCLASS objects require heap allocation, GC tracking, and reference management,
 *      which adds unnecessary overhead for high-frequency small data types.
 *
 * 3. Safe and Flexible Lifecycle Management:
 *    - InstancedStruct works with UScriptStruct metadata to safely construct,
 *      copy, move, and destruct polymorphic structs without relying on UObject lifecycle.
 *    - Virtual destructors in base structs ensure proper cleanup of derived fragments.
 *
 * 4. Editor, Networking, and Serialization Friendly:
 *    - InstancedStruct integrates seamlessly with FastArray serialization,
 *      replication, and editor copy/paste operations for data blocks.
 *    - UCLASS replication and duplication semantics are more heavyweight and
 *      prone to pointer aliasing issues.
 *
 * Summary:
 * - Use FInstancedStruct for lightweight, composable, and polymorphic data fragments.
 * - Use UCLASS only for objects that need world context, behavior, or complex lifecycle management.
 */
USTRUCT(BlueprintType)
struct FInv_ItemFragment
{
	GENERATED_BODY()
	FInv_ItemFragment() {}
	FInv_ItemFragment(const FInv_ItemFragment&) = default;
	FInv_ItemFragment& operator=(const FInv_ItemFragment&) = default;
	FInv_ItemFragment(FInv_ItemFragment&&) = default;
	FInv_ItemFragment& operator=(FInv_ItemFragment&&) = default;
	virtual ~FInv_ItemFragment() {} // virtual destructor is required here
	
	FGameplayTag GetFragmentTag() const { return FragmentTag; }
	void SetFragmentTag(FGameplayTag Tag) { FragmentTag = Tag; }
	
private:
	UPROPERTY(EditAnywhere, Category="Inventory")
	FGameplayTag FragmentTag = FGameplayTag::EmptyTag;
};

USTRUCT(BlueprintType)
struct FInv_GridFragment : public FInv_ItemFragment
{
	GENERATED_BODY()
	
	FIntPoint GetGridSize() const { return GridSize; }
	void SetGridSize(FIntPoint Size) { GridSize = Size; }
	float GetGridPadding() const { return GridPadding; }
	void SetGridPadding(float Padding) { GridPadding = Padding; }
	
private:
	UPROPERTY(EditAnywhere, Category="Inventory")
	FIntPoint GridSize = {1, 1};
	
	UPROPERTY(EditAnywhere, Category="Inventory")
	float GridPadding = 0.0f;
};

USTRUCT(BlueprintType)
struct FInv_ImageFragment : public FInv_ItemFragment
{
	GENERATED_BODY()
	
	UTexture2D* GetIcon() const { return Icon; }
	
private:
	UPROPERTY(EditAnywhere, Category="Inventory")
	TObjectPtr<UTexture2D> Icon = nullptr;
	
	UPROPERTY(EditAnywhere, Category="Inventory")
	FVector2D IconDimensions = FVector2D(64.f, 64.f);
};
