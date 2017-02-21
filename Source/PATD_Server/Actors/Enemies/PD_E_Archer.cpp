// Fill out your copyright notice in the Description page of Project Settings.

#include "PATD_Server.h"
#include "PD_E_Archer.h"
//#include "PATD_Server/Actors/MyCharacterParent.h"

// Sets default values
APD_E_Archer::APD_E_Archer()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	/*static ConstructorHelpers::FObjectFinder<UClass> MyBPClass(TEXT("Class'/Game/Blueprints/Enemies/Archer'"));
	if (MyBPClass.Object != NULL)
	{
		archerClass = MyBPClass.Object;
	}*/
	static ConstructorHelpers::FObjectFinder<UBlueprint> ItemBlueprint(TEXT("Blueprint'/Game/Blueprints/Enemies/Archer.Archer'"));
	if (ItemBlueprint.Object) {
		archerClass = (UClass*)ItemBlueprint.Object->GeneratedClass;
	}
	FVector spawnLoc = FVector(0, 0, 300);
	FRotator spawnRot = FRotator(0, 0, 0);;
	UWorld* const World = GetWorld();
	if (World) {
		characterP = GetWorld()->SpawnActor<AMyCharacterParent>(archerClass, spawnLoc, spawnRot);
	}
}

// Called when the game starts or when spawned
void APD_E_Archer::BeginPlay()
{
	Super::BeginPlay();
	/*static ConstructorHelpers::FObjectFinder<UBlueprint> ItemBlueprint(TEXT("/Game/Character/MyCharacter.MyCharacter"));
	if (ItemBlueprint.Object) {
		MyItemBlueprint = (UClass*)ItemBlueprint.Object->GeneratedClass;
	}*/
	/*FVector spawnLoc = FVector(0, 0 ,300);
	FRotator spawnRot = FRotator(0, 0, 0);;
	UWorld* const World = GetWorld();
	if (World) {
		characterP = GetWorld()->SpawnActor<AMyCharacterParent>(archerClass, spawnLoc, spawnRot);
	}*/
}

// Called every frame
void APD_E_Archer::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

