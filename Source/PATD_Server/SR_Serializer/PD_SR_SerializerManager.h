// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

/**
 * 
 */
class PATD_SERVER_API PD_SR_SerializerManager
{
public:
	PD_SR_SerializerManager();
	~PD_SR_SerializerManager();

	//Esta funcion la llama el socketManager cuando tiene un nuevo paquete de datos
	/*Ustruct,*/bool DeserializeData(TArray<uint8>* data );


	//Esta funcion comunica con la capa superior para serializar datos
	TArray<uint8>* SerializeData()/*Ustruct,*/ ;

	/*
	bool enviarUStruct(Ustruct) {
		SerializeData();
		socket->enviar
	}*/




};
