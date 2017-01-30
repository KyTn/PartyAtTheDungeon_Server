// Fill out your copyright notice in the Description page of Project Settings.

#include "PATD_Server.h"
#include "PD_SR_SerializerManager.h"

PD_SR_SerializerManager::PD_SR_SerializerManager() {

}
PD_SR_SerializerManager::~PD_SR_SerializerManager() {

}

FStructGenericList PD_SR_SerializerManager::DeserializeData(TArray<uint8>* data, bool & correct)
{
	FStructGenericList generyStructs;
	UStruct* FMyStruct = FStructGenericList::StaticStruct();
	FMemoryReader ArReader(*data);
	FMyStruct->SerializeBin(ArReader, &generyStructs);//No estoy seguro si añade las estructuras al array
	if (generyStructs.list.Num()==0)// si esta vacio no le llegara nada, ¿no?
		correct = false;
	else
		correct = true;
	return generyStructs;
}

TArray<uint8>* PD_SR_SerializerManager::SerializeData(FStructGenericList generycStructs, bool & correct)
{
	TArray<uint8>* data = new TArray<uint8>();
	UStruct* FMyStruct = FStructGenericList::StaticStruct();

	FMemoryWriter ArWriter(*data);
	FMyStruct->SerializeBin(ArWriter, &generycStructs);
	if (data->Num() == 0)// si esta vacio no le llegara nada, ¿no?
		correct = false;
	else
		correct = true;
	return data;
}
