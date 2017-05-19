// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Character.h"
#include "Runtime/AIModule/Classes/BehaviorTree/BehaviorTree.h"
#include "Structs/PD_ServerStructs.h"
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

	UFUNCTION(BlueprintCallable, Category = "SkillAnimation")
		bool PlayAnimationSkill(int ID_Skill);

	UPROPERTY(EditAnywhere, Category = "behavior")
		UBehaviorTree* behaviorTree;

	#pragma region Stats
	
	UPROPERTY(EditAnywhere, Category = "Base Stats")
		int baseAP;
	
	UPROPERTY(EditAnywhere, Category = "Base Stats")
		int baseDamage;
	
	UPROPERTY(EditAnywhere, Category = "Base Stats")
		int baseHP;
	
	//FStructBasicStats
	UPROPERTY(EditAnywhere, Category = "Basic Stats")
		uint8 POD;
	UPROPERTY(EditAnywhere, Category = "Basic Stats")
		uint8 AGI;
	UPROPERTY(EditAnywhere, Category = "Basic Stats")
		uint8 DES;
	UPROPERTY(EditAnywhere, Category = "Basic Stats")
		uint8 CON;
	UPROPERTY(EditAnywhere, Category = "Basic Stats")
		uint8 PER;
	UPROPERTY(EditAnywhere, Category = "Basic Stats")
		uint8 MAL;

	UPROPERTY(EditAnywhere, Category = "Basic Stats")
		TArray<TEnumAsByte<ActiveSkills>> activeSkillList;

	UPROPERTY(EditAnywhere, Category = "Basic Stats")
		TArray<TEnumAsByte<PasiveSkills>> pasiveSkillList;

	UPROPERTY(EditAnywhere, Category = "Basic Stats")
		TEnumAsByte<TypeWeapons> weapon;


	UPROPERTY(EditAnywhere, Category = "Base Stats")
		FStructTotalStats structtotal;
	#pragma endregion

	UFUNCTION()
	void OnHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit);
	
	//UFUNCTION()
	//void ReceiveHit(UPrimitiveComponent* MyComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);



	//FStructBasicStats
	uint8 GetPOD() { return POD; };

	uint8 GetAGI() { return AGI; };

	uint8 GetDES() { return DES; };

	uint8 GetCON() { return CON; };

	uint8 GetPER() { return PER; };

	uint8 GetMAL() { return MAL; };

	int GetBaseAP() { return baseAP; };

	int GetBaseDamage() { return baseDamage; };

	int  GetBaseHP() { return baseHP; };

	

};
