// 

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameplayTagContainer.h"
#include "Ability.generated.h"

class AActor;

USTRUCT(BlueprintType)
struct FAbilityRepData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	TWeakObjectPtr<AActor> Owner = nullptr;

	UPROPERTY(BlueprintReadOnly)
	bool bActivated = false;

	FAbilityRepData() : Owner(nullptr), bActivated(false) 
	{
	}

	FAbilityRepData(AActor* InOwner, const bool InActivated) : Owner(InOwner), bActivated(InActivated)
	{
	}
};


UCLASS(Abstract, Blueprintable, BlueprintType)
class ABILITYSYSTEM_API UAbility : public UObject
{
	GENERATED_BODY()
	
	UPROPERTY(ReplicatedUsing = OnRep_RepData)
	FAbilityRepData RepData;

	UPROPERTY(EditAnywhere, Category = "Ability")
	FGameplayTag AbilityTag;

public:
	virtual bool IsSupportedForNetworking() const override { return true; }
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Ability")
	FORCEINLINE bool IsActivated() const { return RepData.bActivated; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Ability")
	FORCEINLINE FGameplayTag GetAbilityTag() const { return AbilityTag; }

	UFUNCTION(BlueprintNativeEvent)
	void Activate(const FAbilityRepData& AbilityData);

	UFUNCTION(BlueprintNativeEvent)
	void Disable(const FAbilityRepData& AbilityData);

	UFUNCTION(BlueprintNativeEvent)
	bool CanStartAbility(const AActor* Instigator);

	UFUNCTION(BlueprintPure, Category = "Ability")
	FORCEINLINE bool HasCooldown() const { return CooldownTime > 0; }

	UFUNCTION(BlueprintPure, Category = "Ability")
	FORCEINLINE bool HasDuration() const { return Duration > 0; }

	UFUNCTION(BlueprintPure, Category = "Ability")
	FORCEINLINE bool HasPeriodic() const { return Period > 0; }

protected:
	UFUNCTION() void OnRep_RepData();

	virtual void StartCooldown();
	virtual void EndCooldown();

	virtual void StartDuration();
	virtual void StartPeriodic();

	UFUNCTION(BlueprintNativeEvent, Category = "Effect")
	void ExecutePeriodic(AActor* Instigator);

	void AutoDisable();
private:
	UPROPERTY(EditAnywhere)
	float CooldownTime = 0;

	//UPROPERTY()
	bool bCooldownActive;

	FTimerHandle CooldownHandle;

	UPROPERTY(EditDefaultsOnly)
	float Duration = 0.0f;

	UPROPERTY(EditDefaultsOnly)
	float Period = 0.0f;

	FTimerHandle PeriodHandle;
	FTimerHandle DurationHandle;
};
