#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Types/Inv_GridTypes.h"
#include "StructUtils/InstancedStruct.h"
#include "Inv_ItemManifest.generated.h"

/*
 * The Item Manifest contains all the necessary data for creating a new Inventory Item
 */

struct FInv_ItemFragment;

USTRUCT(BlueprintType)
struct INVENTORY_API FInv_ItemManifest
{
	GENERATED_BODY()
	
	UInv_InventoryItem* Manifest(UObject* NewOuter);
	EInv_ItemCategory GetItemCategory() const { return ItemCategory; }
	FGameplayTag GetItemType() const { return ItemType; }
	
	template <typename  FragmentType>
	requires std::derived_from<FragmentType, FInv_ItemFragment>
	const FragmentType* GetFragmentOfTypeWithTag(const FGameplayTag& FragmentTag) const;
	
	template <typename  FragmentType>
	requires std::derived_from<FragmentType, FInv_ItemFragment>
	const FragmentType* GetFragmentOfType() const;
	
	template <typename  FragmentType>
	requires std::derived_from<FragmentType, FInv_ItemFragment>
	FragmentType* GetFragmentOfTypeMutable();
	
private:
	UPROPERTY(EditAnywhere, Category="Inventory", meta = (ExcludeBaseStruct))
	TArray<TInstancedStruct<FInv_ItemFragment>> Fragments;
	
	UPROPERTY(EditAnywhere, Category="Inventory")
	EInv_ItemCategory ItemCategory = EInv_ItemCategory::None;
	
	UPROPERTY(EditAnywhere, Category="Inventory")
	FGameplayTag ItemType;
};

template <typename FragmentType>
requires std::derived_from<FragmentType, FInv_ItemFragment>
const FragmentType* FInv_ItemManifest::GetFragmentOfTypeWithTag(const FGameplayTag& FragmentTag) const
{
	for (const TInstancedStruct<FInv_ItemFragment>& Fragment : Fragments)
	{
		if (const FragmentType* FragmentPtr = Fragment.GetPtr<FragmentType>())
		{
			if (!FragmentPtr->GetFragmentTag().MatchesTagExact(FragmentTag)) continue; 
			return FragmentPtr;
		}
	}
	return nullptr;
}

template <typename FragmentType> requires std::derived_from<FragmentType, FInv_ItemFragment>
const FragmentType* FInv_ItemManifest::GetFragmentOfType() const
{
	for (const TInstancedStruct<FInv_ItemFragment>& Fragment : Fragments)
	{
		if (const FragmentType* FragmentPtr = Fragment.GetPtr<FragmentType>())
		{
			return FragmentPtr;
		}
	}
	return nullptr;
}

template <typename FragmentType> requires std::derived_from<FragmentType, FInv_ItemFragment>
FragmentType* FInv_ItemManifest::GetFragmentOfTypeMutable()
{
	for (TInstancedStruct<FInv_ItemFragment>& Fragment : Fragments)
	{
		if (FragmentType* FragmentPtr = Fragment.GetMutablePtr<FragmentType>())
		{
			return FragmentPtr;
		}
	}
	return nullptr;
}


