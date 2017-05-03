// Fill out your copyright notice in the Description page of Project Settings.

#include "PATD_Server.h"
#include "PD_E_Character.h"

#include "GM_Game/LogicCharacter/PD_GM_LogicCharacter.h"

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
	//APD_E_Character* me = Cast<APD_E_Character>(SelfActor);
	APD_E_Character* other = Cast<APD_E_Character>(Hit.GetActor());

	//UE_LOG(LogTemp, Warning, TEXT("APD_E_Character::OnHit %s name of actor hited> %s"), *GetName() , *other->GetName());
	/*
	if (other != nullptr) //Los dos son characters
	{
		UE_LOG(LogTemp, Warning, TEXT("APD_E_Character::OnHit CH of own actor > %d"), logic_character->GetTotalStats()->CH);
		UE_LOG(LogTemp, Warning, TEXT("APD_E_Character::OnHit CH of actor hited> %d"), other->GetLogicCharacter()->GetTotalStats()->CH);

		if (logic_character->GetTotalStats()->CH < other->GetLogicCharacter()->GetTotalStats()->CH)
		{
			//El character que ejecuta el codigo pierde, asi que es el que se tiene que mover
			logic_character->MoveWhenCollisionLost();
		}
	}
	*/
}

//Por si acaso hay que usar el metodo de recibir hit
/*void APD_E_Character::ReceiveHit(UPrimitiveComponent* MyComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
FColor DisplayColor = FColor::Green;
const FString DebugMessage(OtherActor->GetName());
GEngine->AddOnScreenDebugMessage(-1, 5.0f, DisplayColor, DebugMessage);

APD_E_Character* other = Cast<APD_E_Character>(OtherActor);
if (other != nullptr) //Los dos son characters
{
	UE_LOG(LogTemp, Warning, TEXT("APD_E_Character::OnHit CH of own actor > %d"), logic_character->GetTotalStats()->CH);
	UE_LOG(LogTemp, Warning, TEXT("APD_E_Character::OnHit CH of actor hited> %d"), other->GetLogicCharacter()->GetTotalStats()->CH);

	if (logic_character->GetTotalStats()->CH < other->GetLogicCharacter()->GetTotalStats()->CH)
	{
		//El character que ejecuta el codigo pierde, asi que es el que se tiene que mover
		logic_character->MoveWhenCollisionLost();
	}
}
}
*/
