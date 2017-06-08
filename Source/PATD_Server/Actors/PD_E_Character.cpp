// Fill out your copyright notice in the Description page of Project Settings.

#include "PATD_Server.h"
#include "PD_E_Character.h"

#include "GM_Game/LogicCharacter/PD_GM_LogicCharacter.h"
#include "GM_Game/PD_GM_EnemyManager.h"
#include "PD_ServerGameInstance.h"

#include "PD_GenericController.h"
#include "PD_SplineActors.h"


// Sets default values
APD_E_Character::APD_E_Character()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	OnActorHit.AddDynamic(this, &APD_E_Character::OnHit);

}

// Called when the game starts or when spawned
void APD_E_Character::BeginPlay()
{
	Super::BeginPlay();


	
}

// Called every frame
void APD_E_Character::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

// Called to bind functionality to input
void APD_E_Character::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void APD_E_Character::SetLogicCharacter(PD_GM_LogicCharacter* nlogic_character)
{
	logic_character = nlogic_character;
}
PD_GM_LogicCharacter* APD_E_Character::GetLogicCharacter()
{
	return logic_character;
}

void APD_E_Character::OnHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit)
{
	APD_E_Character* other = Cast<APD_E_Character>(Hit.GetActor());
}

//Por si acaso hay que usar el metodo de recibir hit
/*void APD_E_Character::ReceiveHit(UPrimitiveComponent* MyComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{

APD_E_Character* other = Cast<APD_E_Character>(OtherActor);
if (other != nullptr) //Los dos son characters
{
	//UE_LOG(LogTemp, Warning, TEXT("APD_E_Character::OnHit CH of own actor > %d"), logic_character->GetTotalStats()->CH);
//	UE_LOG(LogTemp, Warning, TEXT("APD_E_Character::OnHit CH of actor hited> %d"), other->GetLogicCharacter()->GetTotalStats()->CH);
	if (!logic_character->GetController()->IsCalculatingMovePath)
	{
		if (logic_character->GetAValueToDecideCollision() < other->GetLogicCharacter()->GetAValueToDecideCollision()) 
		{
			//El character que ejecuta el codigo pierde, asi que es el que se tiene que mover
			//logic_character->GetController()->IsCalculatingMovePath = true;
			//logic_character->MoveWhenCollisionLost();
		}
		else {

		}
	}
	
}
}

*/

void APD_E_Character::CollisionWithOtherCharacter(APD_E_Character* charWhoCrash)
{
	if (logic_character)
	{
		if (!logic_character->GetController()->IsCalculatingMovePath)
		{
			if (logic_character->GetTotalStats()->CH <= charWhoCrash->GetLogicCharacter()->GetTotalStats()->CH) //Si el CH es menor que el que te choca, te mueves tu
			{
				//El character que ejecuta el codigo pierde, asi que es el que se tiene que mover
				UE_LOG(LogTemp, Warning, TEXT("APD_E_Character::CollisionWithOtherCharacter PIERDE: %s"), *logic_character->GetIDCharacter());

				logic_character->GetController()->IsCalculatingMovePath = true;
				logic_character->GetController()->StopMoving();
				logic_character->GetController()->GetSpline()->RemovePoints();
				logic_character->MoveWhenCollisionLost();
			}
			else 
			{
				UE_LOG(LogTemp, Warning, TEXT("APD_E_Character::CollisionWithOtherCharacter: GANA %s"), *logic_character->GetIDCharacter());
				//charWhoCrash->logic_character->GetController()->IsCalculatingMovePath = true;
				//charWhoCrash->logic_character->GetController()->StopMoving();
				//charWhoCrash->logic_character->GetController()->GetSpline()->RemovePoints();
				//charWhoCrash->logic_character->MoveWhenCollisionLost();
			}
		}
	}

}

bool APD_E_Character::PlayAnimationSkill(int ID_Skill)
{
	FOutputDeviceNull ar;

	const FString command = FString::Printf(TEXT("PlaySkillAnimation %d"), ID_Skill);

	if (this->CallFunctionByNameWithArguments(*command, ar, NULL, true))
	{
		//UE_LOG(LogTemp, Warning, TEXT("APD_E_Character::PlayAnimationSkill -- EXITO EN LLAMAR A LA FUNCION"));
		return true;
	}
	else {
		//UE_LOG(LogTemp, Error, TEXT("APD_E_Character::PlayAnimationSkill - EEROR EN LLAMATR A LA FUNCION"));
		return false;
	}

}

bool APD_E_Character::SetCharacterCameraOnView()
{
	FOutputDeviceNull ar;

	const FString command = FString::Printf(TEXT("PutCameraPlayerOnView"));

	if (this->CallFunctionByNameWithArguments(*command, ar, NULL, true))
	{
		UE_LOG(LogTemp, Warning, TEXT("APD_E_Character::SetCharacterCameraOnView -- EXITO EN LLAMAR A LA FUNCION"));
		return true;
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("APD_E_Character::SetCharacterCameraOnView - EEROR EN LLAMATR A LA FUNCION"));
		return false;
	}

}

void APD_E_Character::UpdateCharLife(float damage)
{
	logic_character->UpdateHPCurrent(damage);
}

void APD_E_Character::DeleteCharacter() //Sirve para eliminar desde BP a un enemigo del game manager
{

	logic_character->GetTotalStats()->HPCurrent = 0;
	logic_character->SetIsDead(true);

	/*
	if (logic_character->GetIsPlayer())
	{
		logic_character->GetTotalStats()->HPCurrent = 0;
		logic_character->SetIsDead(true);
	}
	else {
		PD_GM_EnemyManager* enemyManager = Cast<UPD_ServerGameInstance>(GetGameInstance())->gameManager->enemyManager;

		enemyManager->DeleteEnemy(logic_character);
	}
	*/
}

void APD_E_Character::StopAnimationParticleSystem()
{
	FOutputDeviceNull ar;

	const FString command = FString::Printf(TEXT("StopParticleSystem"));

	if (this->CallFunctionByNameWithArguments(*command, ar, NULL, true))
	{
		UE_LOG(LogTemp, Warning, TEXT("APD_E_Character::StopAnimationParticleSystem -- EXITO EN LLAMAR A LA FUNCION"));
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("APD_E_Character::StopAnimationParticleSystem - EEROR EN LLAMATR A LA FUNCION"));
	}
}

bool APD_E_Character::IscharacterStoppingByCollision() 
{
	return logic_character->GetIsStoppingByCollision();
}