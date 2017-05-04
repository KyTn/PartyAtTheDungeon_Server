// Fill out your copyright notice in the Description page of Project Settings.

#include "PATD_Server.h"
#include "PD_E_Door.h"



// Sets default values
APD_E_Door::APD_E_Door()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APD_E_Door::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void APD_E_Door::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APD_E_Door::InteractToActivate(AActor * interactor, bool overwriteState)
{
}

void APD_E_Door::InteractToDeactivate(AActor * interactor, bool overwriteState)
{
}

