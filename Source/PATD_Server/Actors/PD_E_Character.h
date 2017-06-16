// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Character.h"
#include "Runtime/AIModule/Classes/BehaviorTree/BehaviorTree.h"
#include "Structs/PD_ServerStructs.h"
#include "Components/WidgetComponent.h"
#include "PD_E_Character.generated.h"


class PD_GM_LogicCharacter;

UCLASS()
class PATD_SERVER_API APD_E_Character : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APD_E_Character(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
//	APD_E_Character();


	UPROPERTY(Category = PD, VisibleAnywhere, BlueprintReadWrite)
		class UWidgetComponent* Widget;
	UPROPERTY(Category = PD, VisibleAnywhere, BlueprintReadWrite)
		class USpringArmComponent* SpringArm;
	UPROPERTY(Category = PD, VisibleAnywhere, BlueprintReadWrite)
		class UCameraComponent* Camera;
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

	APD_E_Character* charWhoCollideWithMe = nullptr;

	UFUNCTION(BlueprintCallable, Category = "CollisionWithCharacters")
		void CollisionWithOtherCharacter(APD_E_Character* charWhoCrash);

	UFUNCTION(BlueprintCallable, Category = "SkillAnimation")
		bool PlayAnimationSkill(int ID_Skill);
	UFUNCTION(BlueprintCallable, Category = "SkillAnimation")
		void StopAnimationParticleSystem(); //Reset or stop the particle system of character
	UFUNCTION(BlueprintCallable, Category = "SkillAnimation")
		bool IscharacterStoppingByCollision(); //Si true, display particle of stunned by collision

	UFUNCTION(BlueprintCallable, Category = "SkillAnimation")
		bool SetCharacterCameraOnView();

	UPROPERTY(EditAnywhere, Category = "PD_IA")
		UBehaviorTree* behaviorTree;

	UPROPERTY(EditAnywhere, Category = "PD_IA")
		bool generateRandomPersonality;

	UPROPERTY(EditAnywhere, Category = "PD_IA")
		TEnumAsByte<EIAPersonality> IAPersonality;
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


	
	UFUNCTION(BlueprintCallable, Category = "PD_GM_LogicCharacter")
		void UpdateCharLife(float damage); 

	UFUNCTION(BlueprintCallable, Category = "PD_GM_LogicCharacter")
		void UpdatePoints (int inPoints);
	UFUNCTION(BlueprintCallable, Category = "PD_GM_LogicCharacter")
		int GetPoints();

	UFUNCTION(BlueprintCallable, Category = "PD_GM_LogicCharacter")
		bool IsPlayer();

	UFUNCTION(BlueprintCallable, Category = "PD_GM_LogicCharacter")
		bool IsDead();

	UFUNCTION(BlueprintCallable, Category = "PD_GM_LogicCharacter")
		void DeleteCharacter(); //Sirve para eliminar desde BP a un enemigo del game manager



	UFUNCTION()
	void OnHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit);
	
	//UFUNCTION()
	//void ReceiveHit(UPrimitiveComponent* MyComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION(BlueprintCallable, Category = "DataCharacter")
		void GetCharacterID(FString &ID_Char);

	UFUNCTION(BlueprintCallable, Category = "DataCharacter")
		void GetInfoCharcaterForWidget(FString &ID_Char, FString &TypeChar, int &numberPlayer);

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


	//Funciones para mantener la interfaz,  la barra de vida actualizada.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InGameStats")
		float percentHP = 1.0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InGameStats")
		FString stringHP = "0";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InGameStats")
		FString stateActionOnChar = " ";
	UFUNCTION(BlueprintCallable, Category = "InGameStats")
		bool  GetIsConnected();
	UFUNCTION(BlueprintCallable, Category = "InGameStats")
		bool  GetAreOrdersSended();
	UFUNCTION(BlueprintCallable, Category = "InGameStats")
		void UpdateStringHP();
	UFUNCTION(BlueprintCallable, Category = "InGameStats")
		void UpdateStateActionOnChar();

	//PARA MUSICA
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Music Sound Base")
		USoundBase* sound_Attack;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Music Sound Base")
		USoundBase* sound_AttackEsp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Music Sound Base")
		USoundBase* sound_CastSkill;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Music Sound Base")
		USoundBase* sound_GetHurt;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Music Sound Base")
		USoundBase* sound_Death;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Music Sound SKILLS")
		USoundBase* sound_Defense;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Music Sound SKILLS")
		USoundBase* sound_Heal;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Music Sound SKILLS")
		USoundBase* sound_Fua;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Music Sound SKILLS")
		USoundBase* sound_GiveFire;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Music Sound SKILLS")
		USoundBase* sound_IceBorder;




};
