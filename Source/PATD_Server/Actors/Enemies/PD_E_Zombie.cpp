// Fill out your copyright notice in the Description page of Project Settings.

#include "PATD_Server.h"
#include "PD_E_Zombie.h"


// Sets default values
APD_E_Zombie::APD_E_Zombie()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	static ConstructorHelpers::FObjectFinder<UBlueprint> ItemBlueprint(TEXT("Blueprint'/Game/Blueprints/Enemies/Zombie.Zombie'"));
	if (ItemBlueprint.Object) {
		zombieClass = (UClass*)ItemBlueprint.Object->GeneratedClass;
	}
}

// Called when the game starts or when spawned
void APD_E_Zombie::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APD_E_Zombie::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

// Called to bind functionality to input
void APD_E_Zombie::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

