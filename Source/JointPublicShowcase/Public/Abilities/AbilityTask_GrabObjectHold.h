#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_GrabObjectHold.generated.h"

class UGameplayAbility;

UCLASS()
class JOINTPUBLICSHOWCASE_API UAbilityTask_GrabObjectHold : public UAbilityTask
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category="Ability|Tasks", meta=(DisplayName="Grab Object Hold", HidePin="OwningAbility", DefaultToSelf="OwningAbility"))
    static UAbilityTask_GrabObjectHold* GrabObjectHold(
        UGameplayAbility* OwningAbility,
        UPrimitiveComponent* InGrabbedComponent,
        FName InGrabbedComponentSocketName,
        float InDistance = 450.f,
        float InForceScale = 5.f
    );

    virtual void Activate() override;
    virtual void TickTask(float DeltaTime) override;
    virtual void OnDestroy(bool bInOwnerFinished) override;
    
    
public:
    
    FBodyInstance* GetGrabbedBodyInstance() const;

protected:
    
    UPROPERTY()
    TWeakObjectPtr<UPrimitiveComponent> GrabbedComponent;
    UPROPERTY()
    FName GrabbedComponentSocketName;
    

    float Distance = 450.f;
    float ForceScale = 5.f;
};