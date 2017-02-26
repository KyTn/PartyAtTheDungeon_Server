// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "MapInstantiatorActor.generated.h"


// FORWARD DECLARATIONS //

class PD_MG_StaticMap;
class PD_MG_DynamicMap;
class PD_MG_LogicPosition;
class APD_E_Archer;
class APD_E_ElementActor;

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
	TSubclassOf<class APD_E_ElementActor> TileClass;
	TSubclassOf<class APD_E_ElementActor> WallClass;
	//Enemies
	/// seria la class generica
	TSubclassOf<class APD_E_Archer> ArcherClass;
	TSubclassOf<class APD_E_Archer> ZombieClass;

#pragma endregion 


	AActor* InstantiateTile(PD_MG_LogicPosition* logpos);
	AActor* InstantiateWall(PD_MG_LogicPosition* logpos);

	ACharacter* InstantiateArcher(PD_MG_LogicPosition* logpos);
	ACharacter* InstantiateZombie(PD_MG_LogicPosition* logpos);
	
};
