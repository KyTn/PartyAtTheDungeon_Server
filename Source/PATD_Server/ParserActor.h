// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "ParserActor.generated.h"

class PATD_MG_StaticMap;

UCLASS()
class PATD_SERVER_API AParserActor : public AActor
{
	GENERATED_BODY()
	
private :
	PATD_MG_StaticMap* StaticMapRef;

public:	
	// Sets default values for this actor's properties
	AParserActor();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	
	
};
