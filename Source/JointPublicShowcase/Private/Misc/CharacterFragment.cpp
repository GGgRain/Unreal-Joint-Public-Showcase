// Fill out your copyright notice in the Description page of Project Settings.


#include "Misc/CharacterFragment.h"

#include "Misc/FragmentableCharacterInterface.h"

UCharacterFragment::UCharacterFragment()
{
}

void UCharacterFragment::BeginPlayFragment()
{
	OnBeginPlayFragment();
}

void UCharacterFragment::EndPlayFragment()
{
	OnEndPlayFragment();
}

void UCharacterFragment::OnBeginPlayFragment_Implementation()
{
}

void UCharacterFragment::OnEndPlayFragment_Implementation()
{
}

TScriptInterface<IFragmentableCharacterInterface> UCharacterFragment::GetOwningCharacter() const
{
	// iterate outer chain and find the first fragmentable character interface
	UObject* Outer = GetOuter();
	
	while (Outer)
	{
		if (Outer->GetClass()->ImplementsInterface(UFragmentableCharacterInterface::StaticClass()))
		{
			return TScriptInterface<IFragmentableCharacterInterface>(Outer);
		}
		Outer = Outer->GetOuter();
	}
	return nullptr;
}
