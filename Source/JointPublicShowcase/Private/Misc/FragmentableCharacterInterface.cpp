// Fill out your copyright notice in the Description page of Project Settings.

#include "Misc/FragmentableCharacterInterface.h"
#include "Misc/CharacterFragment.h"

void IFragmentableCharacterInterface::GetFragments_Implementation(TArray<UCharacterFragment*>& OutFragments)
{
	return;
}

void IFragmentableCharacterInterface::BeginPlayFragments_Implementation()
{
	TArray<UCharacterFragment*> Fragments;
	
	GetFragments(Fragments);
	
	for (UCharacterFragment*& Fragment : Fragments)
	{
		if (!Fragment) continue;
		
		Fragment->BeginPlayFragment();
	}
}

void IFragmentableCharacterInterface::EndPlayFragments_Implementation()
{
	TArray<UCharacterFragment*> Fragments;
	
	GetFragments(Fragments);
	
	for (UCharacterFragment*& Fragment : Fragments)
	{
		if (!Fragment) continue;
		
		Fragment->EndPlayFragment();
	}
}
