#include "Items/Fragments/Inv_ItemFragment.h"

void FInv_HealthPotionFragment::OnConsume(APlayerController* PC)
{
	// Get stats component from the PC or the PC->GetPawn()
	// or get ASC and apply a Gameplay Effect
	// or call and interface function for Healing()
	
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Healed %f HP"), HealAmount));
}

void FInv_ManaPotionFragment::OnConsume(APlayerController* PC)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("Restored %f MANA"), ManaAmount));
}
