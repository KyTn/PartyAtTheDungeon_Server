// Fill out your copyright notice in the Description page of Project Settings.

#include "PATD_Server.h"
#include "PD_E_Character.h"

#include "GM_Game/LogicCharacter/PD_GM_LogicCharacter.h"
#include "GM_Game/PD_GM_EnemyManager.h"
#include "WidgetComponent.h"
#include "PD_ServerGameInstance.h"
#include "Kismet/KismetMathLibrary.h"
#include "PD_GenericController.h"
#include "PD_SplineActors.h"
#include "UserWidget.h"

/*
// Sets default values
APD_E_Character::APD_E_Character()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	OnActorHit.AddDynamic(this, &APD_E_Character::OnHit);

}*/
APD_E_Character::APD_E_Character(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer) {
	Widget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PD_Char_Widget"));
	
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("PD_Char_SpringArm"));
	
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("PD_Char_Camera"));
	
	Camera->bUsePawnControlRotation = true;
	//Camera->AttachToComponent(SpringArm,);
	//FRotator(0, -50, 180);
	SpringArm->SetRelativeLocationAndRotation(FVector(0, 0, 0), FRotator( -50.0, 90 ,0));
	SpringArm->TargetArmLength = 800;
	SpringArm->bDoCollisionTest = false;
	Widget->SetRelativeLocation(FVector(0, 0, 250.0));
	Widget->bGenerateOverlapEvents = false;

	Widget->SetupAttachment(RootComponent);
	SpringArm->SetupAttachment(RootComponent);
	Camera->SetupAttachment(SpringArm);
	this->GetCharacterMovement()->GravityScale = 0;
	//this->GetCharacterMovement()->planar
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
	percentHP =  (float)logic_character->totalStats->HPCurrent / (float)logic_character->totalStats->HPTotal;

//	Update del widget para que apunte a la camara
	/*AActor* targetView= UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetViewTarget();
	ACameraActor* camera = Cast<ACameraActor>(targetView);
	APD_E_Character* PDCharacter = Cast<APD_E_Character>(targetView);
	UCameraComponent* cameraRotateTo = nullptr;
	if (camera) {
		cameraRotateTo = camera->GetCameraComponent();
	}
	else if (PDCharacter){
		cameraRotateTo = PDCharacter->Camera;
	}
	FMinimalViewInfo viewInfo;
	cameraRotateTo->GetCameraView(0, viewInfo);
	
	FRotator widgetRot = UKismetMathLibrary::FindLookAtRotation(Widget->ComponentToWorld.GetLocation(), viewInfo.Location);
	widgetRot.Roll = 0;
	Widget->SetWorldRotation(widgetRot);
	*/

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
		UE_LOG(LogTemp, Warning, TEXT("APD_E_Character::CollisionWithOtherCharacter TIENE LOGIC_CHAR: %s"), *logic_character->GetIDCharacter());

		if (logic_character->GetController())
		{
			UE_LOG(LogTemp, Warning, TEXT("APD_E_Character::CollisionWithOtherCharacter TIENE CONTROLLER: %s"), *logic_character->GetIDCharacter());

			if (!logic_character->GetController()->IsCalculatingMovePath)
			{
				UE_LOG(LogTemp, Warning, TEXT("APD_E_Character::CollisionWithOtherCharacter NO IsCalculatingMovePath: %s"), *logic_character->GetIDCharacter());
				if (logic_character->GetTotalStats())
				{
					UE_LOG(LogTemp, Warning, TEXT("APD_E_Character::CollisionWithOtherCharacter GET TOTAL STAT: %s"), *logic_character->GetIDCharacter());

					if (charWhoCrash && charWhoCrash->GetLogicCharacter() && charWhoCrash->GetLogicCharacter()->GetTotalStats()) {
						UE_LOG(LogTemp, Warning, TEXT("APD_E_Character::CollisionWithOtherCharacter charWHOCRASH CORRECT: %s"), *logic_character->GetIDCharacter());
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

						}
					}
				}
				
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
	UpdateStringHP();

	//FString actionString = " - ";
	//actionString.Append(FString::FromInt(damage));
	//stateActionOnChar = actionString;
	//UpdateStateActionOnChar();

	logic_character->UpdateHPCurrent(damage);
	if (logic_character->GetTotalStats()->HPCurrent <= 0)
	{
		logic_character->GetController()->Animation_DeathChar((int)ActiveSkills::GetHurt);
		logic_character->SetIsDead(true);
	}
	else
	{
		logic_character->GetController()->Animation_GetHurt((int)ActiveSkills::GetHurt);
	}
}

bool APD_E_Character::IsPlayer()
{
	return logic_character->GetIsPlayer();
}

bool APD_E_Character::IsDead()
{
	return logic_character->GetIsDead();

}

//LLamados desde blueprint(como bola de fuego)

void APD_E_Character::UpdatePoints(int inPoints)
{
	logic_character->UpdatePointsCurrent(inPoints);
}

void APD_E_Character::UpdateEnemiesDefeated()
{
	logic_character->GetTotalStats()->EnemiesDefeated++;
}

int APD_E_Character::GetPoints()
{
	return logic_character->GetTotalStats()->PointsCurrent;
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

void APD_E_Character::GetCharacterID(FString &ID_Char)
{

	ID_Char = logic_character->GetIDCharacter();

	/*FString positionLogic = "PoX: ";
	positionLogic.Append(FString::FromInt(logic_character->GetCurrentLogicalPosition().GetX()));
	positionLogic.Append(" PosY: ");
	positionLogic.Append(FString::FromInt(logic_character->GetCurrentLogicalPosition().GetY()));
	*/
	/*switch (logic_character->GetIAPersonality())
	{
		case EIAPersonality::Coward:{
			ID_Char.Append("::Coward");
			break;
		};
		case EIAPersonality::Neutral: {
			ID_Char.Append("::Neutral");
			break;
		};
		case EIAPersonality::Smart: {
			ID_Char.Append("::Smart");
			break;
		};
		case EIAPersonality::Warlike: {
			ID_Char.Append("::Warlike");
			break;
		};
	default:
		break;
	}
	*/
	//ID_Char.Append(positionLogic);
}

void APD_E_Character::GetInfoCharcaterForWidget(FString &ID_Char, FString &TypeChar, int &numberPlayer)
{
	ID_Char = logic_character->GetIDCharacter();

	if (logic_character)
	{
		if (logic_character->GetIsPlayer())
		{
			UE_LOG(LogTemp, Warning, TEXT("APD_E_Character::GetInfoCharcaterForWidge weapon - %d "), logic_character->GetWeapon()->TypeWeapon);

			switch (TypeWeapons(logic_character->GetWeapon()->TypeWeapon))
			{
			case TypeWeapons::SwordAndShield:
			{
				TypeChar = "Legionary";
				break;
			}
			case TypeWeapons::CrossbowAndShield:
			{
				TypeChar = "Ranger";
				break;
			}
			case TypeWeapons::Staff:
			{
				TypeChar = "Wizard";
				break;
			}
			default:
			{
				TypeChar = "NotFound";
				break;
			}
			}
			FString auxNumberplayer = logic_character->GetIDCharacter();
			FString charSplit = "_0";
			FString FStringNumberPlayerLeft = "";
			FString FStringNumberPlayerRight = "";
			auxNumberplayer.Split(charSplit, &FStringNumberPlayerLeft, &FStringNumberPlayerRight);

			int numChar = FCString::Atoi(*FStringNumberPlayerRight);
			numChar++;
			numberPlayer = numChar;
		}
		else
		{
			//Type Char
			switch (ECharacterType(logic_character->GetTypeCharacter()))
			{
			case ECharacterType::OrcBow:
			{
				TypeChar = "Archer Orc";
				break;
			}
			case ECharacterType::OrcGuns:
			{
				TypeChar = "Guns Orc";
				break;
			}
			case ECharacterType::OrcMelee:
			{
				TypeChar = "Melee Orc";
				break;
			}
			case ECharacterType::OrcBoss:
			{
				TypeChar = "Boss Orc";
				break;
			}
			default:
				break;
			}
			//NumberPlayer (For the color)
			numberPlayer = 0;
		}
	}
	

	//UE_LOG(LogTemp, Warning, TEXT("APD_E_Character::GetInfoCharcaterForWidge - name %s "), *ID_Char);
	//UE_LOG(LogTemp, Warning, TEXT("APD_E_Character::GetInfoCharcaterForWidge - type  %s "), *TypeChar);
	//UE_LOG(LogTemp, Warning, TEXT("APD_E_Character::GetInfoCharcaterForWidge number player - %d "), numberPlayer);

}

bool APD_E_Character::GetIsConnected() {
	PD_PlayersManager* playersManager = Cast<UPD_ServerGameInstance>(this->GetGameInstance())->gameManager->playersManager;
	StructPlayer *structPlayer = playersManager->GetStructPlayerByIDCharacter(logic_character->GetIDCharacter());

	//UE_LOG(LogTemp, Warning, TEXT("MyCharacter's id_player is %d"), structPlayer->ID_PLAYER);

	//UE_LOG(LogTemp, Warning, TEXT("MyCharacter's id_player is %s"),* structPlayer->ID_Client);


	return structPlayer->isConnected;


}

bool  APD_E_Character::GetAreOrdersSended() {
	PD_PlayersManager* playersManager = Cast<UPD_ServerGameInstance>(this->GetGameInstance())->gameManager->playersManager;
	StructPlayer *structPlayer = playersManager->GetStructPlayerByIDCharacter(logic_character->GetIDCharacter());
	return structPlayer->playerSendOrder;

}

void APD_E_Character::UpdateStringHP()
{
	stringHP = FString::FromInt(logic_character->totalStats->HPCurrent).Append(FString("/").Append(FString::FromInt((float)logic_character->totalStats->HPTotal)));

}

void APD_E_Character::UpdateStateActionOnChar()
{
	FOutputDeviceNull ar;

	const FString command = FString::Printf(TEXT("ShowStateActionOnChar"));

	if (Widget->GetUserWidgetObject()->CallFunctionByNameWithArguments(*command, ar, NULL, true))
	{
		UE_LOG(LogTemp, Warning, TEXT("APD_E_Character::UpdateStateActionOnChar -- EXITO EN LLAMAR A LA FUNCION"));
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("APD_E_Character::UpdateStateActionOnChar - EEROR EN LLAMATR A LA FUNCION"));
	}

}