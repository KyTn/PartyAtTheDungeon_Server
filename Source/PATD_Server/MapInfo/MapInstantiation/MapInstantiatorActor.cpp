// Fill out your copyright notice in the Description page of Project Settings.

#include "PATD_Server.h"
#include "MapInstantiatorActor.h"
#include "PATD_Server/MapGeneration/Static/PD_MG_StaticMap.h"
#include "PATD_Server/MapGeneration/PD_MG_LogicPosition.h"
#include "PATD_Server/MapGeneration/PD_MG_MapParser.h"
#include "PATD_Server/MapGeneration/Dynamic/PD_MG_DynamicMap.h"
#include "PATD_Server/Actors/Enemies/PD_E_EnemyCharacter.h"
#include "PATD_Server/Actors/PD_E_ElementActor.h"


#pragma region BUILT IN FUNTIONS 


AMapInstantiatorActor::AMapInstantiatorActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UBlueprint> TileBlueprint(TEXT("Blueprint'/Game/BluePrints/MapElements/BP_ME_TileActor.BP_ME_TileActor'"));
	if (TileBlueprint.Object) {
		TileClass = (UClass*)TileBlueprint.Object->GeneratedClass;
	}
	static ConstructorHelpers::FObjectFinder<UBlueprint> WallBlueprint(TEXT("Blueprint'/Game/BluePrints/MapElements/BP_ME_WallActor.BP_ME_WallActor'"));
	if (WallBlueprint.Object) {
		WallClass = (UClass*)WallBlueprint.Object->GeneratedClass;
	}
	static ConstructorHelpers::FObjectFinder<UBlueprint> ArcherBlueprint(TEXT("Blueprint'/Game/Blueprints/Enemies/Archer.Archer'"));
	if (ArcherBlueprint.Object) {
		ArcherClass = (UClass*)ArcherBlueprint.Object->GeneratedClass;
	}

	static ConstructorHelpers::FObjectFinder<UBlueprint> ZombieBlueprint(TEXT("Blueprint'/Game/Blueprints/Enemies/Zombie.Zombie'"));
	if (ZombieBlueprint.Object) {
		ZombieClass = (UClass*)ZombieBlueprint.Object->GeneratedClass;
	}
}

// Called when the game starts or when spawned
void AMapInstantiatorActor::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AMapInstantiatorActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


#pragma endregion



#pragma region INSTANTIATORS

AActor* AMapInstantiatorActor::InstantiateTile(PD_MG_LogicPosition* logpos)
{
	//return GetWorld()->SpawnActor<APD_E_TileActor>(tileActor,FVector(logpos->GetX()*100.0f, logpos->GetY() * 100.0f, 0.f), FRotator(0.0f, 0.f, 0.f), FActorSpawnParameters());

	//return GetWorld()->SpawnActor<APD_E_TileActor>(FVector(-1.0f * logpos->GetX()*100.0f, logpos->GetY() * 100.0f, 0.f), FRotator(0.0f, 0.f, 0.f), FActorSpawnParameters());
	/*UWorld* const World = GetWorld();
	if (World) {
	characterT = GetWorld()->SpawnActor<APD_E_TileActor>(TileClass, FVector(-1.0f * logpos->GetX()*100.0f, logpos->GetY() * 100.0f, 0.f), FRotator(0.0f, 0.f, 0.f));
	}
	characterPs.Add(characterT);*/
	return GetWorld()->SpawnActor<APD_E_ElementActor>(TileClass, FVector(-1.0f * logpos->GetX()*100.0f, logpos->GetY() * 100.0f, 0.f), FRotator(0.0f, 0.f, 0.f));
	//	return new AActor();
}


AActor* AMapInstantiatorActor::InstantiateWall(PD_MG_LogicPosition* logpos)
{
	//return GetWorld()->SpawnActor<APD_E_TileActor>(tileActor,FVector(logpos->GetX()*100.0f, logpos->GetY() * 100.0f, 0.f), FRotator(0.0f, 0.f, 0.f), FActorSpawnParameters());
	//return GetWorld()->SpawnActor<APD_E_ElementActor>(WallClass, FVector(-1.0f * logpos->GetX()*100.0f, logpos->GetY() * 100.0f, 0.f), FRotator(0.0f, 0.f, 0.f));
	return GetWorld()->SpawnActor<APD_E_ElementActor>(WallClass, FVector(-1.0f * logpos->GetX()*100.0f, logpos->GetY() * 100.0f, 0.f), FRotator(0.0f, 0.f, 0.f));

	//return GetWorld()->SpawnActor<APD_E_WallActor>(FVector(-1.0f * logpos->GetX()*100.0f, logpos->GetY() * 100.0f, 0.f), FRotator(0.0f, 0.f, 0.f), FActorSpawnParameters());
}


ACharacter* AMapInstantiatorActor::InstantiateArcher(PD_MG_LogicPosition* logpos) {
	/*TSubclassOf<class APD_E_Zombie> zombieClass;
	AActor*	characterP = NewObject<APD_E_Archer>();
	static ConstructorHelpers::FObjectFinder<UBlueprint> ItemBlueprint(TEXT("Blueprint'/Game/Blueprints/Enemies/Zombie.Zombie'"));
	if (ItemBlueprint.Object) {
	zombieClass = (UClass*)ItemBlueprint.Object->GeneratedClass;
	}
	UWorld* const World = GetWorld();
	if (World) {
	characterP = GetWorld()->SpawnActor<AMyCharacterParent>(zombieClass, FVector(-1.0f * logpos->GetX()*100.0f, logpos->GetY() * 100.0f, 100.f), FRotator(0.0f, 0.f, 0.f));
	}
	return characterP;*/
	//characterP = GetWorld()->SpawnActor<APD_E_Archer>(FVector(-1.0f * logpos->GetX()*100.0f, logpos->GetY() * 100.0f, 100.f), FRotator(0.0f, 0.f, 0.f), FActorSpawnParameters());
	///characterP->Placement(FVector(-1.0f * logpos->GetX()*100.0f, logpos->GetY() * 100.0f, 100.f), FRotator(0.0f, 0.f, 0.f));
	//characterP->getCharacter(0)->SetActorLocationAndRotation(FVector(-1.0f *1600.0f, 700.0f, 100.f), FRotator(0.0f, 0.f, 0.f));
	//return characterP;

	//return GetWorld()->SpawnActor<APD_E_Zombie>(FVector(-1.0f * logpos->GetX()*100.0f, logpos->GetY() * 100.0f, 100.f), FRotator(0.0f, 0.f, 0.f), FActorSpawnParameters());
	return GetWorld()->SpawnActor<APD_E_EnemyCharacter>(ArcherClass, FVector(-1.0f * logpos->GetX()*100.0f, logpos->GetY() * 100.0f, 100.f), FRotator(0.0f, 0.f, 0.f));
}

ACharacter* AMapInstantiatorActor::InstantiateZombie(PD_MG_LogicPosition* logpos) {
	return GetWorld()->SpawnActor<APD_E_EnemyCharacter>(ZombieClass, FVector(-1.0f * logpos->GetX()*100.0f, logpos->GetY() * 100.0f, 100.f), FRotator(0.0f, 0.f, 0.f));
}

#pragma endregion



