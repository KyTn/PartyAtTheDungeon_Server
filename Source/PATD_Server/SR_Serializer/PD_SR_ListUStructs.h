// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "PD_SR_ListUStructs.generated.h"
/**
 * 
 */






USTRUCT()
struct FStructGeneric
{
	GENERATED_BODY()

		UPROPERTY()
		int structType;
	
	FStructGeneric() {

	}
};


USTRUCT()
struct FStructMap : public FStructGeneric
{
	GENERATED_BODY()

		//Always make USTRUCT variables into UPROPERTY()
		//    any non-UPROPERTY() struct vars are not replicated

		// So to simplify your life for later debugging, always use UPROPERTY()
		UPROPERTY()
		FString stringPrueba;

	UPROPERTY()
		int intPrueba;

	UPROPERTY()
		TArray<FString> arrayPruebaStrings;



	//Constructor
	FStructMap()
	{
		intPrueba = 0;
		arrayPruebaStrings = TArray<FString>();
		arrayPruebaStrings.Add("prueba 0");
		arrayPruebaStrings.Add("Prueba 1 tralalalalalla");
		arrayPruebaStrings.Add("Prueba 2 patatin");

		//Always initialize your USTRUCT variables!
		//   exception is if you know the variable type has its own default constructor

	}

	FStructMap(int p)
	{
		intPrueba = p;
		//structType = Enum.StrucMap;
		//Always initialize your USTRUCT variables!
		//   exception is if you know the variable type has its own default constructor

	}
};




USTRUCT()
struct FStructGenericList
{
	GENERATED_BODY()



		UPROPERTY()
		TArray<FStructGeneric> list;


	FStructGenericList()
	{
		FStructMap m;
		list.Add(m);
	}
};





 //If you want this to appear in BP, make sure to use this instead
 //USTRUCT(BlueprintType)

