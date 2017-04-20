// Fill out your copyright notice in the Description page of Project Settings.

#include "PATD_Server.h"
#include "Components/SplineComponent.h"
#include "PD_GM_SplineManager.h"


// Sets default values
APD_GM_SplineManager::APD_GM_SplineManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APD_GM_SplineManager::BeginPlay()
{
	Super::BeginPlay();
	splinesPool = TArray<APD_SplineActors*>(); //Inicializar el pool de los Splines
	for (int i = 0; i < initialSplines; i++)
	{
		APD_SplineActors* splineActor = (APD_SplineActors*)GetWorld()->SpawnActor(APD_SplineActors::StaticClass());

		splinesPool.Add(splineActor);
	}

	if (splinesPool.Num() > 0){
		UE_LOG(LogTemp, Log, TEXT("APD_GM_SplineManager::GetSpline Begin - num spline : %d"), splinesPool.Num());
	}

}

// Called every frame
void APD_GM_SplineManager::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

APD_SplineActors* APD_GM_SplineManager::GetSpline()
{

	bool allUsed = true;

	UE_LOG(LogTemp, Log, TEXT("APD_GM_SplineManager::GetSpline - num spline : %d"), splinesPool.Num());

	for (int i = 0; i < splinesPool.Num(); i++)
	{
		if (splinesPool[i]->GetIsUsing() == false)
		{
			UE_LOG(LogTemp, Log, TEXT("APD_GM_SplineManager::GetSpline - index spline : %d"), i);

			splinesPool[i]->SetIsUsing(true);
			allUsed = false;
			return splinesPool[i];
		}
	}

	if (allUsed) //Si todos estan ocupados, hay que crear uno nuevo y añadirlo al array (pool)
	{
		APD_SplineActors* splineActor = (APD_SplineActors*)GetWorld()->SpawnActor(APD_SplineActors::StaticClass());
		splinesPool.Add(splineActor);
		return splinesPool[splinesPool.Num() - 1];
	}


	return nullptr;
}