// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Structs/PD_ServerEnums.h"
#include "MapInstantiatorActor.generated.h"


// FORWARD DECLARATIONS //

class PD_MG_StaticMap;
class PD_MG_DynamicMap;
class PD_MG_LogicPosition;
class APD_E_Character;
class APD_E_ElementActor;
class APD_E_Door;
class PD_MM_Room;
//////////////////////////

UCLASS()
class PATD_SERVER_API AMapInstantiatorActor : public AActor
{
	GENERATED_BODY()
	
public:
																			 
#pragma region BUILT IN FUNCTIONS											 
																			 
	AMapInstantiatorActor();												 
																			 
	// Called when the game starts or when spawned							 
	virtual void BeginPlay() override;										 
																			 
	// Called every frame													 
	virtual void Tick(float DeltaSeconds) override;							 
																			 
#pragma endregion															 	

																			 
#pragma region TSUBCLASSOF Static & Dynamic									 
																			 
	//Static																 
	
#pragma region DEFAULT - COMMON

	TSubclassOf<class APD_E_ElementActor> TileClass;
	TSubclassOf<class APD_E_ElementActor> WallClass;

	TSubclassOf<class APD_E_Door> DoorClass;

#pragma endregion


#pragma region MAPSKIN DUNGEON

	TSubclassOf<class APD_E_ElementActor> Dungeon_SpecialTile_Class;
	
	
	//Props
	TSubclassOf<class APD_E_ElementActor> Dungeon_Prop_Treasure_01_Class;



#pragma endregion 


#pragma region ENEMIES

	//Enemies
	/// seria la class generica
	TSubclassOf<class APD_E_Character> ArcherClass;
	TSubclassOf<class APD_E_Character> ZombieClass;
#pragma endregion 


#pragma region PLAYERS
	TSubclassOf<class APD_E_Character> PlayerClass;
#pragma endregion 
#pragma endregion 

	APD_E_ElementActor* InstantiateTile(PD_MG_LogicPosition logpos);
	APD_E_ElementActor* InstantiateWall(PD_MG_LogicPosition logpos);

	APD_E_Door* InstantiateDoor(PD_MG_LogicPosition logpos);

#pragma region Instantiate Dungeon MapElements 

	APD_E_ElementActor* Instantiate_Dungeon_Prop_Treasure_01(PD_MG_LogicPosition logpos);
	APD_E_ElementActor* Instantiate_Dungeon_SpecialTile(PD_MG_LogicPosition logpos);

#pragma endregion 

	APD_E_Character* InstantiateArcher(PD_MG_LogicPosition logpos);
	APD_E_Character* InstantiateZombie(PD_MG_LogicPosition logpos);
	APD_E_Character* InstantiatePlayer(PD_MG_LogicPosition logpos);
};
