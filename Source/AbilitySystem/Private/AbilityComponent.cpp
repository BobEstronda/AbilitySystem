// 


#include "AbilityComponent.h"

#include "Net/UnrealNetwork.h"
#include "Ability.h"

UAbilityComponent::UAbilityComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;

	SetIsReplicatedByDefault(true);
	bReplicateUsingRegisteredSubObjectList = true;
}

void UAbilityComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UAbilityComponent, Abilities, COND_OwnerOnly)
}

void UAbilityComponent::ServerRemoveAbility_Implementation(const FGameplayTag& AbilityTag)
{
	if (!CachedAbilities.Contains(AbilityTag)) return;

	TWeakObjectPtr<UAbility> Ability = *CachedAbilities.Find(AbilityTag);
	if (!Ability.Get() || Ability->IsActivated()) return;

	CachedAbilities.Remove(AbilityTag);
	Ability->ConditionalBeginDestroy();
	Abilities.Remove(Ability.Get());
}

void UAbilityComponent::ServerAddAbility_Implementation(TSubclassOf<UAbility> Ability)
{
	const FGameplayTag& AbilityTag = Ability.GetDefaultObject()->GetAbilityTag();

	if (CachedAbilities.Contains(AbilityTag))
	{
		return;
	}

	UAbility* NewAbility = NewObject<UAbility>(this, Ability);
	AddReplicatedSubObject(NewAbility);
	Abilities.Add(NewAbility);
	CachedAbilities.Add(AbilityTag, NewAbility);
}

void UAbilityComponent::ServerRemoveAbilities_Implementation(const FGameplayTagContainer& AbilitiesToRemove)
{
	for (const FGameplayTag& AbilityTag : AbilitiesToRemove.GetGameplayTagArray())
	{
		ServerRemoveAbility(AbilityTag);
	}
}

void UAbilityComponent::ServerAddAbilities_Implementation(const TArray<TSubclassOf<UAbility>>& InAbilities)
{
	for (TSubclassOf<UAbility> Ability : InAbilities)
	{
		ServerAddAbility(Ability);
	}
}

bool UAbilityComponent::TryActivateAbility(const FGameplayTag& AbilityTag)
{
	return ActivateAbility(AbilityTag);
}

bool UAbilityComponent::TryDisableAbility(const FGameplayTag& AbilityTag)
{
	return DisableAbility(AbilityTag);
}

void UAbilityComponent::OnRep_Abilities()
{
	check(IsNetMode(NM_Client));

	CachedAbilities.Empty(Abilities.Num());

	for (UAbility* Ability : Abilities)
	{
		CachedAbilities.Add(Ability->GetAbilityTag(), Ability);
	}
}

bool UAbilityComponent::ActivateAbility(const FGameplayTag& AbilityTag)
{
	TWeakObjectPtr<UAbility> Ability = *CachedAbilities.Find(AbilityTag);
	if (!Ability.IsValid() || Ability->IsActivated()) return false;

	const bool bStartAbility = Ability->CanStartAbility(GetOwner());
	if (bStartAbility)
	{
		Ability->Activate({ GetOwner(),true });

		if (!GetOwner()->HasAuthority())
		{
			ServerRequestActivateAbility(AbilityTag);
		}
	}

	return bStartAbility;
}

bool UAbilityComponent::DisableAbility(const FGameplayTag& AbilityTag)
{
	TWeakObjectPtr<UAbility> Ability = *CachedAbilities.Find(AbilityTag);
	if (!Ability.IsValid() && !Ability->IsActivated()) return false;

	if (!GetOwner()->HasAuthority())
	{
		ServerRequestDisableAbility(AbilityTag);
	}

	Ability->Disable({ GetOwner(),false });
	return true;
}

void UAbilityComponent::ServerRequestDisableAbility_Implementation(const FGameplayTag& AbilityTag)
{
	DisableAbility(AbilityTag);
}

void UAbilityComponent::ServerRequestActivateAbility_Implementation(const FGameplayTag& AbilityTag)
{
	ActivateAbility(AbilityTag);
}
