// 

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "AbilityComponent.generated.h"

class UAbility;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ABILITYSYSTEM_API UAbilityComponent : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY(ReplicatedUsing = OnRep_Abilities, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TArray<UAbility*> Abilities;

	TMap<FGameplayTag, TWeakObjectPtr<UAbility>> CachedAbilities;
public:	
	UAbilityComponent();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void ServerAddAbility(TSubclassOf<UAbility> Ability);

	UFUNCTION(Server, Reliable)
	void ServerRemoveAbility(const FGameplayTag& AbilityTag);

	UFUNCTION(Server, Reliable)
	void ServerAddAbilities(const TArray<TSubclassOf<UAbility>>& InAbilities);

	UFUNCTION(Server, Reliable)
	void ServerRemoveAbilities(const FGameplayTagContainer& AbilitiesToRemove);

	UFUNCTION(BlueprintCallable, Category = "Ability|Client")
	bool TryActivateAbility(const FGameplayTag& AbilityTag);

	UFUNCTION(BlueprintCallable, Category = "Ability|Client")
	bool TryDisableAbility(const FGameplayTag& AbilityTag);

protected:
	UFUNCTION(Server, Reliable)
	void ServerRequestActivateAbility(const FGameplayTag& AbilityTag);

	UFUNCTION(Server, Reliable)
	void ServerRequestDisableAbility(const FGameplayTag& AbilityTag);

	UFUNCTION() void OnRep_Abilities();

private:
	bool ActivateAbility(const FGameplayTag& AbilityTag);
	bool DisableAbility(const FGameplayTag& AbilityTag);
		
};
