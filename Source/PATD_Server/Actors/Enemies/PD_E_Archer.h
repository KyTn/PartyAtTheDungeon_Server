// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "PATD_Server/Actors/MyCharacterParent.h"
#include "PD_E_Archer.generated.h"

UCLASS()
class PATD_SERVER_API APD_E_Archer : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	APD_E_Archer();


	void Placement(FVector spawnLoc, FRotator spawnRot);
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

	/*	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "BP Classes")
	UBlueprint* archerClass;
	*/
	/*AMyCharacterParent* getCharacter(uint8 pos) { return characterPs[pos]; }

	TArray<AMyCharacterParent*> characterPs;

	AMyCharacterParent* characterP;

	TSubclassOf<class AMyCharacterParent> archerClass;

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Assets")
		USkeletalMeshComponent* AlternateMeshAsset;*/
};
