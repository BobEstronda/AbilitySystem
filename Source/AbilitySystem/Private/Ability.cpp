// 


#include "Ability.h"

#include "Net/UnrealNetwork.h"

void UAbility::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UAbility, RepData, COND_None, REPNOTIFY_OnChanged)
}

void UAbility::Activate_Implementation(const FAbilityRepData& AbilityData)
{
	RepData = AbilityData;

	if (HasDuration())
	{
		StartDuration();
	}
	else
	{
		if (HasCooldown())
		{
			StartCooldown();
		}
	}

	if (HasPeriodic())
	{
		StartPeriodic();
	}
}

void UAbility::Disable_Implementation(const FAbilityRepData& AbilityData)
{
	RepData = AbilityData;

	if (HasDuration())
	{
		if (HasCooldown())
		{
			StartCooldown();
		}
	}

	GetWorld()->GetTimerManager().ClearTimer(PeriodHandle);
	GetWorld()->GetTimerManager().ClearTimer(DurationHandle);
}

bool UAbility::CanStartAbility_Implementation(const AActor* Instigator)
{
	if (HasCooldown())
	{
		return !bCooldownActive;
	}

	return true;
}

void UAbility::OnRep_RepData()
{
	if (IsActivated())
	{
		Activate(RepData);
		return;
	}
	Disable(RepData);
}

void UAbility::StartCooldown()
{
	GetWorld()->GetTimerManager().SetTimer(CooldownHandle,this, &UAbility::EndCooldown, CooldownTime,false);
	bCooldownActive = true;
}

void UAbility::EndCooldown()
{
	GetWorld()->GetTimerManager().ClearTimer(CooldownHandle);
	bCooldownActive = false;
}

void UAbility::StartDuration()
{
	GetWorld()->GetTimerManager().SetTimer(DurationHandle, this, &UAbility::AutoDisable, Duration, false);
}

void UAbility::StartPeriodic()
{
	GetWorld()->GetTimerManager().SetTimer(PeriodHandle, FTimerDelegate::CreateUObject(this, &UAbility::ExecutePeriodic, RepData.Owner.Get()), Period, true);
}

void UAbility::AutoDisable()
{
	Disable({RepData.Owner.Get(),false});
}

void UAbility::ExecutePeriodic_Implementation(AActor* Instigator)
{
}
