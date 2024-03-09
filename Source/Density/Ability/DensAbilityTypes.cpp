// Copyright 2023 Daniel Martinez Amigo. All Rights Reserved.


#include "DensAbilityTypes.h"


void FDensDamageContainerSpec::AddTargets(const TArray<FHitResult>& HitResults)
{
   for (const FHitResult& HitResult : HitResults)
   {
      FGameplayAbilityTargetData_SingleTargetHit* NewData = new FGameplayAbilityTargetData_SingleTargetHit(HitResult);
      TargetData.Add(NewData);
   }
}

void FDensDamageContainerSpec::ReplaceTargets(const TArray<FHitResult>& HitResults)
{
   TargetData.Clear();
   for (const FHitResult& HitResult : HitResults)
   {
      FGameplayAbilityTargetData_SingleTargetHit* NewData = new FGameplayAbilityTargetData_SingleTargetHit(HitResult);
      TargetData.Add(NewData);
   }
}


void FDensGameplayEffectContext::SetTargetData(const FGameplayAbilityTargetDataHandle& NewTargetData)
{
   TargetData.Clear();
   TargetData.Append(NewTargetData);
   
}


bool FDensGameplayEffectContext::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
{
   uint32 RepBits = 0;
   if (Ar.IsSaving())
   {
      if (Instigator.IsValid())
      {
         RepBits |= 1 << 0;
      }
      if (EffectCauser.IsValid())
      {
         RepBits |= 1 << 1;
      }
      if (AbilityCDO.IsValid())
      {
         RepBits |= 1 << 2;
      }
      if (bReplicateSourceObject && SourceObject.IsValid())
      {
         RepBits |= 1 << 3;
      }
      if (Actors.Num() > 0)
      {
         RepBits |= 1 << 4;
      }
      if (HitResult.IsValid())
      {
         RepBits |= 1 << 5;
      }
      if (bHasWorldOrigin)
      {
         RepBits |= 1 << 6;
      }
      if (bIsCriticalHit)
      {
         RepBits |= 1 << 7;
      }
      if (bIsWeakened)
      {
         RepBits |= 1 << 8;
      }
      if (DamageTypesTags.IsValid())
      {
         RepBits |= 1 << 9;
      }
      if (TargetData.Num())
      {
         RepBits |= 1 << 10;
      }
   }
 
   Ar.SerializeBits(&RepBits, 11);
 
   if (RepBits & (1 << 0))
   {
      Ar << Instigator;
   }
   if (RepBits & (1 << 1))
   {
      Ar << EffectCauser;
   }
   if (RepBits & (1 << 2))
   {
      Ar << AbilityCDO;
   }
   if (RepBits & (1 << 3))
   {
      Ar << SourceObject;
   }
   if (RepBits & (1 << 4))
   {
      SafeNetSerializeTArray_Default<31>(Ar, Actors);
   }
   if (RepBits & (1 << 5))
   {
      if (Ar.IsLoading())
      {
         if (!HitResult.IsValid())
         {
            HitResult = MakeShared<FHitResult>();
         }
      }
      HitResult->NetSerialize(Ar, Map, bOutSuccess);
   }
   if (RepBits & (1 << 6))
   {
      Ar << WorldOrigin;
      bHasWorldOrigin = true;
   }
   else
   {
      bHasWorldOrigin = false;
   }
 
   if (RepBits & (1 << 7))
   {
      Ar << bIsCriticalHit;
   }

   if (RepBits & (1 << 8))
   {
      Ar << bIsWeakened;
   }
   
   if (RepBits & (1 << 9))
   {
      DamageTypesTags.NetSerialize(Ar, Map, bOutSuccess);
   }

   if (RepBits & (1 << 10))
   {
      TargetData.NetSerialize(Ar, Map, bOutSuccess);
   }
 
   if (Ar.IsLoading())
   {
      AddInstigator(Instigator.Get(), EffectCauser.Get()); // Just to initialize InstigatorAbilitySystemComponent
   }  
 
   bOutSuccess = true;
   return true;
}
