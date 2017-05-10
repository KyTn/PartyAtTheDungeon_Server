// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Character.h"
#include "Runtime/AIModule/Classes/BehaviorTree/BehaviorTree.h"
#include "PD_E_Character.generated.h"

class PD_GM_LogicCharacter;

UCLASS()
class PATD_SERVER_API APD_E_Character : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APD_E_Character();

	//Variable Logic Character para tener doble referencia y poder llamar a los metodos logicos con metodos del Character
	PD_GM_LogicCharacter* logic_character;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void SetLogicCharacter(PD_GM_LogicCharacter* nlogic_character);
	PD_GM_LogicCharacter* GetLogicCharacter();

	UPROPERTY(EditAnywhere, Category = "behavior")
		UBehaviorTree* behaviorTree;

	#pragma region Stats
	
	UPROPERTY(EditAnywhere, Category = "Base Stats")
		int baseAP;
	
	UPROPERTY(EditAnywhere, Category = "Base Stats")
		int baseDamage;
	
	UPROPERTY(EditAnywhere, Category = "Base Stats")
		int baseHP;
	
	#pragma endregion

	UFUNCTION()
	void OnHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit);
	
	//UFUNCTION()
	//void ReceiveHit(UPrimitiveComponent* MyComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);


};
