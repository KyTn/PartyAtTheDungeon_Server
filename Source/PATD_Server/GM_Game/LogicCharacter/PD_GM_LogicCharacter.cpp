// Fill out your copyright notice in the Description page of Project Settings.

#include "PATD_Server.h"
#include "PD_GM_LogicCharacter.h"
#include "Actors/Enemies/PD_AIController.h"
#include "Actors/Players/PD_CharacterController.h"

#include <math.h>       /* ceil */
#include "math.h"
#include "GM_Game/PD_GM_GameManager.h"
#include "../../PD_ServerGameInstance.h"
#include "GM_Game/PD_GM_SplineManager.h"
#include "GM_Game/PD_GM_EnemyManager.h"
#include "../../PD_PlayersManager.h"
#include"Actors/PD_E_Character.h"
#include "GM_Game/PD_GM_MapManager.h"
#include "MapInfo/PD_MM_MapInfo.h"

//Includes of forward declaration
#include "Structs/PD_ServerStructs.h" //Para todos los structs y enums
#include "MapGeneration/PD_MG_LogicPosition.h"
#include "Actors/PD_GenericController.h"
#include "PATD_Server/Structs/PD_ServerEnums.h"
#include "GM_Game/PD_GM_MapManager.h"

PD_GM_LogicCharacter::PD_GM_LogicCharacter()
{
	//Inicializar los Structs
	basicStats = new FStructBasicStats();
	initBaseStats = new FStructInitBaseStats();
	skills = new FStructCharacterSkills();
	weapon = new FStructWeapon();
	skin = new FStructSkin();
	totalStats = new FStructTotalStats();
	characterState = new FStructCharacterState();

	//inicializar las variables
	isDead = false;
	
}

PD_GM_LogicCharacter::~PD_GM_LogicCharacter()
{
	character_Player_BP->Destroy();
}

/* ==============
METODOS PRINCIPALES PARA INTERACTUAR CON EL MAPA
=============== */


TArray<FStructOrderAction>* PD_GM_LogicCharacter::FindPathToMove(PD_MG_LogicPosition desirePosition)
{
	/*
	- OBJETIVO: Sirve para conseguir el listado de ordenes de movimiento que se tienen que enviar desde
	el cliente al servidor para ir a la posición marcada por el jugador donde se quiere mover.
	- PROCESO:
	1. recibe como parametro LogicPosition (X e Y) que identifica a la casilla
	2. llama a la funcion del Algoritmo A* con esta posicion y la referencia al MapManager
	3. El algoritmo devuelve un array completo con el camino a seguir en orden.
	- SE LLAMA DESDE: El Cliente (atraves de interfaz) , o la IA para conseguir el camino hasta la posicion elegida en la fase de movimiento
	*/

	TArray<FStructOrderAction>* listTileToMove = new TArray<FStructOrderAction>();



	return listTileToMove;
}


//PD_MG_LogicPosition* MoveToLogicPosition(uint8 tick, TArray<FStructOrderAction> listMove);
PD_MG_LogicPosition* PD_GM_LogicCharacter::MoveToLogicPosition(FStructOrderAction* order)
{
	/*
	- OBJETIVO: Simula el movimiento del personaje siguiendo una lista de movimientos dada (el camino calculado hasta una
	posicion casilla a casilla)
	- PROCESO:
	1. Recibe como parametro un array con la lista de movimientos casilla a casilla y el indice del array (tick) de
	la casilla a comprobar la simulacion
	2. Simula donde va casilla por casilla comprobando que es posible el movimiento
	3. Actualizar la variable movingLogicPosition
	3. Al acabar de simular devuelve un LogicPosition con la posicion final tras la simulacion
	- SE LLAMA DESDE: Desde el GameManager cuando sea el turno de simular el movimiento. Se pasa el parametro tick, para asegurar
	de que todos los actors que se mueven lo hacen a la vez.
	**Se devuelve un LogicPosition para que el GamenManager pueda ver si hay conflictos dados las posiciones logicas de ese tick de todos los personajes
	que se mueven y actue en consecuencia.
	- Tras el choque habría que cambiar esa movingLogicalPosition
	*/
	/*
	PD_MG_LogicPosition* targetPosition;
	targetPosition = movingLogicalPosition;
	*/
	
	if (isPlayer)
		UE_LOG(LogTemp, Log, TEXT("PD_GM_LogicCharacter::MoveToLogicPosition --> Nueva Posicion Jugador: %f %f"), order->targetLogicPosition.positionX, order->targetLogicPosition.positionY);

	currentLogicalPosition = PD_MG_LogicPosition(order->targetLogicPosition.positionX, order->targetLogicPosition.positionY);
	
	return &currentLogicalPosition;
}


bool PD_GM_LogicCharacter::MoveToPhysicalPosition(TArray<FVector> listPositionsToMove)
{

	
	if (!controller) {
		UE_LOG(LogTemp, Warning, TEXT("LogicCharacter: No se encuentra el controller (null)"));
	}

	if (isPlayer){
			//Cast<APD_CharacterController>(controller)->MoveTo(realPosition.X, realPosition.Y);
			
			//Ponemos como ocupado el spline elegido
			Cast<APD_CharacterController>(controller)->GetSpline()->SetIsUsing(true);
			//limpiamos los puntos del spline anteriores
			Cast<APD_CharacterController>(controller)->GetSpline()->RemovePoints();
			//Seteamos su posicion con la posicion ACTUAL en el MUNDO del Character a Mover
			Cast<APD_CharacterController>(controller)->GetSpline()->SetToActorLocation(Cast<APD_CharacterController>(controller)->GetPawn()->GetActorLocation());
			//Seteamos el Spline Component con los puntos a los que queremos movernos
			Cast<APD_CharacterController>(controller)->GetSpline()->SetPoints(listPositionsToMove);
			//Llamamos al metodo Mover del Controller
			Cast<APD_CharacterController>(controller)->MoveTo(0,0); ///Hablar con CARLOS y MARCOS para ver como tocar esto sin afectar a su EnemyController
	}
	else {
		
			//Cast<APD_EnemyController>(controller)->MoveTo(realPosition.X, realPosition.Y);

			//Ponemos como ocupado el spline elegido
			Cast<APD_AIController>(controller)->GetSpline()->SetIsUsing(true);
			//Seteamos su posicion con la posicion ACTUAL en el MUNDO del Character a Mover
			Cast<APD_AIController>(controller)->GetSpline()->SetToActorLocation(Cast<APD_AIController>(controller)->GetPawn()->GetActorLocation());
			//Seteamos el Spline Component con los puntos a los que queremos movernos
			Cast<APD_AIController>(controller)->GetSpline()->SetPoints(listPositionsToMove);
			//Llamamos al metodo Mover del Controller
			Cast<APD_AIController>(controller)->MoveTo(0,0);

	}
	return true;

}


bool PD_GM_LogicCharacter::ActionTo(FStructTargetToAction action)
{
	UE_LOG(LogTemp, Log, TEXT("PD_GM_LogicCharacter::ActionTo: Iniciando ActionTo: "));
	PD_GM_EnemyManager* enemyManager = Cast<UPD_ServerGameInstance>(GetCharacterBP()->GetGameInstance())->gameManager->enemyManager;
	PD_PlayersManager* playersManager = Cast<UPD_ServerGameInstance>(GetCharacterBP()->GetGameInstance())->gameManager->playersManager;

	//controller->Animation_CriticalBasicAttack((int)ActiveSkills::GiveMeTheFireBlast);

	switch (ActiveSkills(action.id_action))
	{
		case ActiveSkills::BasicAttack:
		{
			UE_LOG(LogTemp, Log, TEXT("PD_GM_LogicCharacter::ActionTo: BasicAttack "));
			
			//controller->Animation_CriticalBasicAttack((int)ActiveSkills::BasicAttack);
			//como es un ataque basico, siempre sera el primero de los id_characters que este en el array de targets de los characters
			PD_GM_LogicCharacter* characterToAttack = nullptr;
			if (isPlayer)
				characterToAttack = enemyManager->GetCharacterByID(action.id_character[0]);
			else
				characterToAttack = playersManager->GetCharacterByID(action.id_character[0]);
			if (characterToAttack)
			{
				Skill_BasicAttack(this, characterToAttack);
			}
			break;
		}
		case ActiveSkills::Defense:
		{
			UE_LOG(LogTemp, Log, TEXT("PD_GM_LogicCharacter::ActionTo: Defense "));

			Skill_Defense(this);
			break;
		}
		case ActiveSkills::WhenFua:
		{
			UE_LOG(LogTemp, Log, TEXT("PD_GM_LogicCharacter::ActionTo: WhenFua "));

			Skill_Melee_Daggers_WhenFua(this);
			break;
		}
		case ActiveSkills::JumpFatTigger:
		{
			UE_LOG(LogTemp, Log, TEXT("PD_GM_LogicCharacter::ActionTo: JumpFatTigger "));

			PD_MG_LogicPosition positionToJump = PD_MG_LogicPosition(action.positions[0].positionX, action.positions[0].positionY);
			Skill_Melee_LargeSword_JumpFatTigger(this, positionToJump);
			break;
		}
		case ActiveSkills::Hostion:
		{
			UE_LOG(LogTemp, Log, TEXT("PD_GM_LogicCharacter::ActionTo: Hostion "));

			PD_GM_LogicCharacter* characterToAttack = nullptr;
			if (isPlayer)
				characterToAttack = enemyManager->GetCharacterByID(action.id_character[0]);
			else
				characterToAttack = playersManager->GetCharacterByID(action.id_character[0]);

			if (characterToAttack)
			{
				Skill_Melee_Hostion(this, characterToAttack);
			}
			break;
		}
		case ActiveSkills::SomeHit:
		{
			UE_LOG(LogTemp, Log, TEXT("PD_GM_LogicCharacter::ActionTo: SomeHit "));

			PD_GM_LogicCharacter* characterToAttack = nullptr;
			if (isPlayer)
				characterToAttack = enemyManager->GetCharacterByID(action.id_character[0]);
			else
				characterToAttack = playersManager->GetCharacterByID(action.id_character[0]);

			if (characterToAttack)
			{
				Skill_Range_Guns_SomeHit(this, characterToAttack);
			}
			break;
		}
		case ActiveSkills::RightInTheAsshole:
		{
			UE_LOG(LogTemp, Log, TEXT("PD_GM_LogicCharacter::ActionTo: RightInTheAsshole "));

			PD_GM_LogicCharacter* characterToAttack = nullptr;
			if (isPlayer)
				characterToAttack = enemyManager->GetCharacterByID(action.id_character[0]);
			else
				characterToAttack = playersManager->GetCharacterByID(action.id_character[0]);

			if (characterToAttack)
			{
				Skill_Range_RightInTheAsshole(this, characterToAttack);
			}
			break;
		}
		case ActiveSkills::GiveMeTheFireBlast:
		{
			UE_LOG(LogTemp, Log, TEXT("PD_GM_LogicCharacter::ActionTo: GiveMeTheFireBlast "));

			PD_GM_LogicCharacter* characterToAttack = nullptr;
			if (isPlayer)
				characterToAttack = enemyManager->GetCharacterByID(action.id_character[0]);
			else
				characterToAttack = playersManager->GetCharacterByID(action.id_character[0]);

			if (characterToAttack)
			{
				Skill_Magic_GiveMeTheFireBlas(this, characterToAttack);
			}
			break;
		}
		case ActiveSkills::Exclaimchas:
		{
			UE_LOG(LogTemp, Log, TEXT("PD_GM_LogicCharacter::ActionTo: Exclaimchas "));

			PD_MG_LogicPosition positionToTeleport = PD_MG_LogicPosition(action.positions[0].positionX, action.positions[0].positionY);
			Skill_Magic_ExclaimChas(this, positionToTeleport);
			break;
		}
		case ActiveSkills::BeInCrossroads:
		{
			UE_LOG(LogTemp, Log, TEXT("PD_GM_LogicCharacter::ActionTo: BeInCrossroads "));

			Skill_Magic_BeInCrossroads(this);
			break;
		}
		case ActiveSkills::WhoHeal:
		{
			UE_LOG(LogTemp, Log, TEXT("PD_GM_LogicCharacter::ActionTo: WhoHeal "));

			PD_GM_LogicCharacter* characterToHeal = nullptr;
			if (isPlayer)
				characterToHeal = playersManager->GetCharacterByID(action.id_character[0]);
			else
				characterToHeal = enemyManager->GetCharacterByID(action.id_character[0]);

			if (characterToHeal)
			{
				Skill_Magic_WhoHeal(this, characterToHeal);
			}
			break;
		}
		case ActiveSkills::ShotMana:
		{
			UE_LOG(LogTemp, Log, TEXT("PD_GM_LogicCharacter::ActionTo: ShotMana "));

			Skill_Magic_ShotMana(this);
			break;
		}
		default:
		{
			// ID SKILL --------- NO COINCIDE CON NINGUN ID SKILL
			UE_LOG(LogTemp, Log, TEXT("PD_GM_LogicCharacter::ActionTo ERROR TO ID SKIL -  %d "), action.id_action);

		}
	}

	return true;
}


void PD_GM_LogicCharacter::ConsumeItem(uint32 idItem)
{
	/*
	- OBJETIVO: Consume un item durante la fase de Consumibles
	- PROCESO:
	1. Recibe un ID que identifica el tipo de Item a consumir
	2. Con el MapManager convierte la posicion logica en fisica
	3.  Actualiza  las variables currentLogicalPosition a la recibida por parametro
	4.Con el PlayerManager coge su personaje y llama a la funcion MoveTo() de su controlador
	- SE LLAMA DESDE: Desde el GameManager cuando sea el turno de visualizar el movimiento.
	Devuelve true si todo ha ido bien, si algo ha fallado, devuelve false (normalmente a raiz de que la funcion move del controlador
	devuelva tambien false
	*/
}

void PD_GM_LogicCharacter::UpdateHPCurrent(float updateLifeIn)
{
	/*
	- OBJETIVO: Recibir el daño de un ataque, actualizar la vida actual y comprobar si ha muerto o no.
	NOTA: Si es negativo sera un ataque, si es positivo será una curacion
	- PROCESO:
	1. Pasar el float a un int, debido a que la vida es tipo int, aunque  el daño por las formulas llegara en float
	2. Actualizar el valor del FStructTotalStats HPCurrent (Si es curacion nunca puede pasar de HPTotal
	3. Comprobar si HPCurrent <= 0. Si es asi, actualizar la variable isDead
	- SE LLAMA DESDE: Cualquier Actor - Personaje o Enemigo que dañe a otro
	*/
	
	int updateLife = FMath::TruncToInt(updateLifeIn);
	//int aa = (GetTotalStats()->HPCurrent + updateLife); //FMath::Clamp(0, (int)GetTotalStats()->HPTotal, (GetTotalStats()->HPCurrent + updateLife));
	//UE_LOG(LogTemp, Warning, TEXT("LogicCharacter:UpdateHPCurrent Vida actual: %d Vida futura: %d"),  GetTotalStats()->HPCurrent , aa);
	
	GetTotalStats()->HPCurrent=FMath::Clamp((GetTotalStats()->HPCurrent + updateLife),0, (int)GetTotalStats()->HPTotal );

	
	UE_LOG(LogTemp, Warning, TEXT("LogicCharacter:%s  come %d de daño y tiene ahora %d de vida"), *GetIDCharacter(), updateLife,GetTotalStats()->HPCurrent);
	if (GetTotalStats()->HPCurrent <= 0) {
		//GetCharacterBP()->SetActorHiddenInGame(true);
		UE_LOG(LogTemp, Warning, TEXT("LogicCharacter:%s se murio"),*GetIDCharacter());
		if (isPlayer) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "HAS PERDIDO PRINGAO!");
	}

	if (updateLife < 0.0f) {
		//lanzar animacion de pain
		this->GetController()->Animation_GetHurt((int)ActiveSkills::GetHurt);
	}
	else {

		//FX de la curacion sobre el afectado
		Cast<APD_E_Character>(this->GetCharacterBP())->PlayAnimationSkill((int)ActiveSkills::WhoHeal);
	}
}

//Devuelve la tirada que le pidan para resolver los conflictos
int8 PD_GM_LogicCharacter::GetARoll(int8 numOfDice, int32 numOfFaces)
{
	int8 totalSum = 0;
	for (int i = 0; i < numOfDice; i++)
	{
		totalSum = totalSum + FMath::RandRange(1, numOfFaces);
	}
	return totalSum;
}

//Metodo para saber el porcentaje de bonus de la probabilidad de IMPACTAR
float PD_GM_LogicCharacter::ImpactPercentage()
{
	float totalBonusImpact = 1;
	/*
	- OBJETIVO: devuelve el porcentaje de impacto bonus que se añade por pasivas u otras fuentes
	- PROCESO:
	1. Recorrer tu lista de pasivas para encontrar si alguna coincide con una habilidad que
	afecte a este porcentaje
	2. Si tiene alguna, suma al contador global de porcentaje el valor dado
	3. devolver dicho valor
	- SE LLAMA DESDE: la funcion GetImpactCharacter()
	*/
	return totalBonusImpact;
}


//Metodo para saber el porcentaje de bonus de la probabilidad de EVADIR
float PD_GM_LogicCharacter::EvasionPercentage()
{
	float totalBonusEvasion = 0;
	/*
	- OBJETIVO: devuelve el porcentaje de impacto bonus que se añade por pasivas u otras fuentes
	- PROCESO:
	1. Recorrer tu lista de pasivas para encontrar si alguna coincide con una habilidad que
	afecte a este porcentaje
	2. Si tiene alguna, suma al contador global de porcentaje el valor dado
	3. devolver dicho valor
	- SE LLAMA DESDE: la funcion GetEvasionCharacter()
	*/

	return totalBonusEvasion;
}


//Metodo para saber el valor que determina si ha impactado sobre un enemigo o no
int8 PD_GM_LogicCharacter::GetImpactCharacter()
{
	int8 ImpactTotal;
	/*
	- OBJETIVO: devuelve el valor  de impacto total para saber si da o no da
	- PROCESO:
	1. Conseguir el porcentaje de bonus de Impacto -- llamar a ImpactPercentage()
	2. Conseguir el Bonus de Destreza
	3. Calcular el Impacto total
	4. devolver el valor
	- SE LLAMA DESDE: cualuqier actor que quiera saber su valor de Impacto para saber si da o no da sobre otro actor
	*/

	ImpactTotal = roundf((GetARoll(3, 6) + totalStats->DESBonus) * (1 + ImpactPercentage()));

	return ImpactTotal;
}


//Metodo para saber el valor que determina si ha evadido un ataque o no
int8 PD_GM_LogicCharacter::GetEvasionCharacter()
{
	int8 EvasionTotal;
	/*
	- OBJETIVO: devuelve el valor  de evasion total de un personaje
	- PROCESO:
	1. Conseguir el porcentaje de bonus de Evasion -- llamar a EvasionPercentage()
	2. Conseguir el Bonus de Agilidad
	3. Calcular la evasion total
	4. devolver el valor
	- SE LLAMA DESDE: cualuqier actor que quiera saber su valor de Impacto para saber si da o no da sobre otro actor
	*/

	EvasionTotal = roundf((GetARoll(3, 6) + totalStats->AGIBonus) * (1 + EvasionPercentage()));

	return EvasionTotal;

}

void PD_GM_LogicCharacter::MoveWhenCollisionLost()
{
	UE_LOG(LogTemp, Warning, TEXT("LogicCharacter: MoveWhenCollisionLost"));

	//Remove the splines point and set the character
	if (!controller) {
		UE_LOG(LogTemp, Warning, TEXT("LogicCharacter: No se encuentra el controller (null)"));
	}

	if (isPlayer)
	{
		//Limpiamos los puntos del spline
		if (Cast<APD_CharacterController>(controller)->GetSpline())
		{
			UE_LOG(LogTemp, Warning, TEXT("LogicCharacter: MoveWhenCollisionLost - Si hay Spline"));

			Cast<APD_CharacterController>(controller)->GetSpline()->RemovePoints();
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("LogicCharacter: MoveWhenCollisionLost - No hay Spline"));

			UPD_ServerGameInstance* SGI = Cast<UPD_ServerGameInstance>(GetCharacterBP()->GetGameInstance());
			if (SGI)
			{
				Cast<APD_CharacterController>(controller)->SetSpline(SGI->gameManager->splineManager->GetSpline());
			}
		}

		
		//Seteamos el spline en la posicion del actor
		Cast<APD_CharacterController>(controller)->GetSpline()->SetActorLocation(mapMng->LogicToWorldPosition(GetCurrentLogicalPosition()));

		//Seteamos los nuevos puntos del spline para moverse
		TArray<FVector> newPositionsToMove = TArray<FVector>();
		newPositionsToMove.Add(mapMng->LogicToWorldPosition(GetCurrentLogicalPosition()));

		
		TArray<PD_MG_LogicPosition> possibleNewPositionToMove = mapMng->Get_LogicPosition_Adyacents_To(GetCurrentLogicalPosition());
		UE_LOG(LogTemp, Log, TEXT("PD_GM_LogicCharacter::MoveWhenCollisionLost %d"), newPositionsToMove.Num());
		for (int j = 0; j < possibleNewPositionToMove.Num(); j++)
		{

			UPD_ServerGameInstance* SGI = Cast<UPD_ServerGameInstance>(GetCharacterBP()->GetGameInstance());
			if (SGI)
			{
				if (SGI->gameManager->CheckIsLogicCharacterInPosition(possibleNewPositionToMove[j]))
				{
					newPositionsToMove.Add(mapMng->LogicToWorldPosition(possibleNewPositionToMove[j]));
					SetCurrentLogicalPosition(possibleNewPositionToMove[j]);
					break;
				}
			}

		}

		
		//Seteamos el Spline Component con los puntos a los que queremos movernos
		if (newPositionsToMove.Num() > 0)
		{
			UE_LOG(LogTemp, Log, TEXT("PD_GM_LogicCharacter::MoveWhenCollisionLost %d"), newPositionsToMove.Num());

			if (Cast<APD_CharacterController>(controller)->GetSpline())
			{
				UE_LOG(LogTemp, Log, TEXT("PD_GM_LogicCharacter::MoveWhenCollisionLost - SetPoints"));

				controller->GetSpline()->SetPoints(newPositionsToMove);
				//Llamamos al metodo Mover del Controller
				//Cast<APD_CharacterController>(controller)->MoveTo(0, 0);
			}
			else{}
			
		}
		
	}
	else
	{
		//Limpiamos los puntos del spline
		if (!Cast<APD_AIController>(controller)->GetSpline())
		{
			UPD_ServerGameInstance* SGI = Cast<UPD_ServerGameInstance>(GetCharacterBP()->GetGameInstance());
			if (SGI)
			{
				Cast<APD_AIController>(controller)->SetSpline(SGI->gameManager->splineManager->GetSpline());
			}
		}
		else {
			Cast<APD_AIController>(controller)->GetSpline()->RemovePoints();
		}

		//Seteamos el spline en la posicion del actor
		Cast<APD_AIController>(controller)->GetSpline()->SetActorLocation(mapMng->LogicToWorldPosition(GetCurrentLogicalPosition()));

		//Seteamos los nuevos puntos del spline para moverse
		TArray<FVector> newPositionsToMove = TArray<FVector>();
		newPositionsToMove.Add(mapMng->LogicToWorldPosition(GetCurrentLogicalPosition()));

		TArray<PD_MG_LogicPosition> possibleNewPositionToMove = mapMng->Get_LogicPosition_Adyacents_To(GetCurrentLogicalPosition());
		for (int j = 0; j < possibleNewPositionToMove.Num(); j++)
		{

			UPD_ServerGameInstance* SGI = Cast<UPD_ServerGameInstance>(GetCharacterBP()->GetGameInstance());
			if (SGI)
			{
				if (SGI->gameManager->CheckIsLogicCharacterInPosition(possibleNewPositionToMove[j]))
				{
					newPositionsToMove.Add(mapMng->LogicToWorldPosition(possibleNewPositionToMove[j]));
					SetCurrentLogicalPosition(possibleNewPositionToMove[j]);
					break;
				}
			}

		}

		//Seteamos el Spline Component con los puntos a los que queremos movernos
		Cast<APD_AIController>(controller)->GetSpline()->SetPoints(newPositionsToMove);
		//Llamamos al metodo Mover del Controller
		Cast<APD_AIController>(controller)->MoveTo(0, 0);

	}

	
	
}

/*
======================================================
*/
///FUNCIONES PARA AYUDA DE HABILIDADES
bool PD_GM_LogicCharacter::CheckIfWasACriticalAttack(int* initialDamage, PD_GM_LogicCharacter* character)
{
	int damageAfterCheck = *initialDamage;

	//Aqui comprobar el grado de % de criticio, dependiendo de pasias de los personajes
	if ( (FMath::RandRange(0.0f, 100.0f)) >= (100 - character->GetTotalStats()->MALBonus ) ) //Critical Attack
	{
		damageAfterCheck = damageAfterCheck + (damageAfterCheck * character->GetTotalStats()->MALBonus );
	}

	if (damageAfterCheck > *initialDamage)
		return true;
	else
		return false;
	
}

int PD_GM_LogicCharacter::CalculateAPleftInPlayerActions(PD_GM_LogicCharacter* CharWhoAttacks)
{
	PD_GM_EnemyManager* enemyManager = Cast<UPD_ServerGameInstance>(GetCharacterBP()->GetGameInstance())->gameManager->enemyManager;
	PD_PlayersManager* playersManager = Cast<UPD_ServerGameInstance>(GetCharacterBP()->GetGameInstance())->gameManager->playersManager;

	int APleft = CharWhoAttacks->GetTotalStats()->AP; //AP totales del personaje

	if (CharWhoAttacks->isPlayer)
	{
		//Calculamos el AP necesario para su movimiento
		APleft = APleft - playersManager->GetStructPlayerByIDClient(CharWhoAttacks->ID_character)->turnOrders->positionsToMove.Num();
		//Calculamos el AP gastado para el resto de las acciones , que no sean defensa u otras (ya elegidas)
		for (int i = 0; i < playersManager->GetStructPlayerByIDClient(CharWhoAttacks->ID_character)->turnOrders->actions.Num(); i++)
		{
			if ((playersManager->GetStructPlayerByIDClient(CharWhoAttacks->ID_character)->turnOrders->actions[i].id_action != (int)ActiveSkills::Defense) &&
				((playersManager->GetStructPlayerByIDClient(CharWhoAttacks->ID_character)->turnOrders->actions[i].id_action != (int)ActiveSkills::WhenFua)) &&
				((playersManager->GetStructPlayerByIDClient(CharWhoAttacks->ID_character)->turnOrders->actions[i].id_action != (int)ActiveSkills::Hostion)))
			{
				for (int j = 0; j < Cast<UPD_ServerGameInstance>(GetCharacterBP()->GetGameInstance())->activeSkills.Num(); j++)
				{
					if (Cast<UPD_ServerGameInstance>(GetCharacterBP()->GetGameInstance())->activeSkills[j].ID_Skill ==
						playersManager->GetStructPlayerByIDClient(CharWhoAttacks->ID_character)->turnOrders->actions[i].id_action) //buscar la habilidad en el listado totales de habilidades -> saber cuanto AP consume
					{
						APleft = APleft - Cast<UPD_ServerGameInstance>(GetCharacterBP()->GetGameInstance())->activeSkills[j].AP;
					}
				}
			}
		}
	}
	else 
	{
		//Calculamos el AP necesario para su movimiento
		APleft = APleft - enemyManager->GetTurnOrders(enemyManager->GetIndexByID(CharWhoAttacks->ID_character))->positionsToMove.Num();
		//Calculamos el AP gastado para el resto de las acciones , que no sean defensa u otras (ya elegidas)
		for (int i = 0; i < enemyManager->GetTurnOrders(enemyManager->GetIndexByID(CharWhoAttacks->ID_character))->actions.Num(); i++)
		{
			if ((playersManager->GetStructPlayerByIDClient(CharWhoAttacks->ID_character)->turnOrders->actions[i].id_action != (int)ActiveSkills::Defense) &&
				((playersManager->GetStructPlayerByIDClient(CharWhoAttacks->ID_character)->turnOrders->actions[i].id_action != (int)ActiveSkills::WhenFua)) &&
				((playersManager->GetStructPlayerByIDClient(CharWhoAttacks->ID_character)->turnOrders->actions[i].id_action != (int)ActiveSkills::Hostion)))
			{
				for (int j = 0; j < Cast<UPD_ServerGameInstance>(GetCharacterBP()->GetGameInstance())->activeSkills.Num(); j++)
				{
					if (Cast<UPD_ServerGameInstance>(GetCharacterBP()->GetGameInstance())->activeSkills[j].ID_Skill ==
						enemyManager->GetTurnOrders(enemyManager->GetIndexByID(CharWhoAttacks->ID_character))->actions[i].id_action) //buscar la habilidad en el listado totales de habilidades -> saber cuanto AP consume
					{
						APleft = APleft - Cast<UPD_ServerGameInstance>(GetCharacterBP()->GetGameInstance())->activeSkills[j].AP;
					}
				}
			}
		}
	}
	
	return APleft;
}

int PD_GM_LogicCharacter::CalculateReductionOfDamage(PD_GM_LogicCharacter* CharWhoDeffense)
{
	int reductionOfDamage = 0; 
	

	if (CharWhoDeffense->GetCharacterState()->activeEffectsOnCharacter.Contains((int)ActiveSkills::Defense)) //Si continene la defensa - Tambien se puede comprobar aqui otros reductores
	{
		int APleft = CalculateAPleftInPlayerActions(CharWhoDeffense);
		if (APleft >= 5)
			reductionOfDamage = 25;
		else
			reductionOfDamage = 5 * APleft;
	}

	return reductionOfDamage;
}

int PD_GM_LogicCharacter::CalculateIncreaseOfDamage(PD_GM_LogicCharacter* CharWhoDeffense)
{
	int increaseDMG = 0;


	if (CharWhoDeffense->GetCharacterState()->activeEffectsOnCharacter.Contains((int)ActiveSkills::WhenFua)) //Si continene la defensa - Tambien se puede comprobar aqui otros reductores
	{
		increaseDMG = 10;

		//Si se da este power up, se lo tiene que quitar (solo dura 1 hostia) -> aunque se va hacer de modo generico - primero restando 1 al valor y luego viendo si es 0 para quitarlo.
		int TimeEffectOnTurn = CharWhoDeffense->GetCharacterState()->activeEffectsOnCharacter[(int)ActiveSkills::WhenFua];
		TimeEffectOnTurn--;
		if (TimeEffectOnTurn == 0)
		{
			CharWhoDeffense->GetCharacterState()->activeEffectsOnCharacter.Remove((int)ActiveSkills::WhenFua);
		}

	}

	return increaseDMG;
}




///SKILS
/* ===============
FUNCIONES QUE DEFINEN EL COMPORTAMIENTO DE LAS HABILIDADES 
================ */
#pragma region Skills Function
void PD_GM_LogicCharacter::Skill_BasicAttack(PD_GM_LogicCharacter* CharWhoAttacks, PD_GM_LogicCharacter* CharWhoReceiveTheAttacks)
{
	/*
	 1. Comprobar si le da o no -> comprobar valores de impacto y evasion
	 1.5 - Comprobar reductores de daño 
	 2. Lanzar el ataque
	*/

	//Rotar al character hacia la direccion donde esta su enemigo
	controller->UpdateRotationCharacterToEnemy(CharWhoReceiveTheAttacks->GetCharacterBP()->GetActorLocation()); //Pasarle la direccion del enemigo al que va a atacar

	//Cast<APD_E_Character>(CharWhoAttacks->GetCharacterBP())->SetCharacterCameraOnView();


	if (CharWhoAttacks->GetImpactCharacter() >= CharWhoReceiveTheAttacks->GetEvasionCharacter()) //Empacto acertado
	{
		//calcular daño -> % Porcentaje de bounus de daño puede cambiar aqui
		int totalDamage = (CharWhoAttacks->GetWeapon()->DMWeapon + CharWhoAttacks->GetInitBaseStats()->DMGBase) * (1 + CharWhoAttacks->GetTotalStats()->PODBonus);
		int increaseOfDMG = CalculateIncreaseOfDamage(CharWhoAttacks);
		totalDamage = totalDamage + ((increaseOfDMG / 100)  * totalDamage);

		//Calcular si es critico el ataque
		if (CheckIfWasACriticalAttack(&totalDamage, CharWhoAttacks))
		{
			//lanzar animacion de ataque critico
			CharWhoAttacks->GetController()->Animation_CriticalBasicAttack((int)ActiveSkills::BasicAttack);
		}
		else
		{
			//lanzar animacion de ataque normal
			CharWhoAttacks->GetController()->Animation_BasicAttack((int)ActiveSkills::BasicAttack);
		}

		//quitarselo al charWhoRecieveTheAttacks
		int reductionOfDamage = CalculateReductionOfDamage(CharWhoReceiveTheAttacks);
		totalDamage = totalDamage - ((reductionOfDamage / 100)  * totalDamage);
		CharWhoReceiveTheAttacks->UpdateHPCurrent(-totalDamage);

		
	}
	else  //Fallo del Ataque
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, "FALLO DE ATAQUE DEL CHARACTER !");

		//lanzar animacion de defensa por parte del atacado
		CharWhoReceiveTheAttacks->GetController()->Animation_DefenseChar((int)ActiveSkills::Defense);
	}
}

void PD_GM_LogicCharacter::Skill_Defense(PD_GM_LogicCharacter* CharWhoAttacks)
{
	/*
	- Gasta tus AP restantes y aumenta tu reduccion de daño en el proximo turno (turno de los enemigos) - luego se va
	1. Poner en array de ActiveEffects la de defense con 1 turno
	2. Actualizar eso al final del turno de los enemigos
	3. Tener en cuenta esto de la defensa en cualquier punto de ataque
	*/
	//Cast<APD_E_Character>(CharWhoAttacks->GetCharacterBP())->SetCharacterCameraOnView();

	CharWhoAttacks->GetCharacterState()->activeEffectsOnCharacter.Add((int)ActiveSkills::Defense, 1);

	CharWhoAttacks->GetController()->Animation_DefenseChar((int)ActiveSkills::Defense);
}

/*MELE*/
///Dagas
void PD_GM_LogicCharacter::Skill_Melee_Daggers_WhenFua(PD_GM_LogicCharacter* CharWhoAttacks)
{
	/*
	 - Incrementa el siguiente ataque (basico) en 4 
	 1. Poner en el TMAP de estados power up esto y ponerlo a 1. 
	 2. Cuando se realiza un ataque basico comprobar los estados
	 3. Si esta este, aumentar el ataque en 4 y poner a 0 el CD en el power up
	*/

	//Cast<APD_E_Character>(CharWhoAttacks->GetCharacterBP())->SetCharacterCameraOnView();

	CharWhoAttacks->GetController()->Animation_CastSkill((int)ActiveSkills::WhenFua);

	CharWhoAttacks->GetCharacterState()->activeEffectsOnCharacter.Add((int)ActiveSkills::WhenFua, 1);

}
///Mandoble
void PD_GM_LogicCharacter::Skill_Melee_LargeSword_JumpFatTigger(PD_GM_LogicCharacter* CharWhoAttacks, PD_MG_LogicPosition PositionToJump)
{
	PD_GM_EnemyManager* enemyManager = Cast<UPD_ServerGameInstance>(GetCharacterBP()->GetGameInstance())->gameManager->enemyManager;

	/*
	- Salta 2 tiles y cae haciendo daño a todo lo que hay alrededor
	1. Iniciar animacion de salto - o ataque critico
	2. Desplazar a la posicion pasada por parametro al personaje
	3. Coger los enemigos adyacentes a esa posicion y quitarles el daño calculado
	*/
	//Cast<APD_E_Character>(CharWhoAttacks->GetCharacterBP())->SetCharacterCameraOnView();

	///NOTA: Comprobar que no se esperen en las animaciones para lanzar las animaciones - si no habria que hacer una animacion de salta y con el SetActorLocation
	CharWhoAttacks->GetController()->GetSpline()->RemovePoints();
	FVector posToMove = mapMng->LogicToWorldPosition(PositionToJump);
	TArray<FVector> positionsToMove = TArray<FVector>();
	positionsToMove.Add(posToMove);
	CharWhoAttacks->GetController()->GetSpline()->SetPoints(positionsToMove);

	CharWhoAttacks->GetController()->MoveTo(0, 0);
	CharWhoAttacks->SetCurrentLogicalPosition(PositionToJump);

	CharWhoAttacks->GetController()->Animation_CriticalBasicAttack((int)ActiveSkills::BasicAttack);

	//Coger las adyacentes y quitarles el daño calculado
	//calcular daño -> % Porcentaje de bounus de daño puede cambiar aqui
	TArray<PD_MG_LogicPosition> nearTiles = mapMng->Get_LogicPosition_Diagonals_And_Adyacents_To(PositionToJump);
	for (int i = 0; i < nearTiles.Num(); i++)
	{
		for (int j = 0; j < enemyManager->GetEnemies().Num(); j++)
		{
			if (nearTiles[i] == enemyManager->GetEnemies()[j]->GetCurrentLogicalPosition()) //Si coincide el baldosa a comprobar con la posicion del enemigo a comprobar, se calcula el daño
			{
				int totalDamage = (CharWhoAttacks->GetWeapon()->DMWeapon + CharWhoAttacks->GetInitBaseStats()->DMGBase) * (1 + CharWhoAttacks->GetTotalStats()->PODBonus);
				int increaseOfDMG = CalculateIncreaseOfDamage(CharWhoAttacks);
				totalDamage = totalDamage + ((increaseOfDMG / 100)  * totalDamage);

				//Calcular si es critico el ataque
				if (CheckIfWasACriticalAttack(&totalDamage, CharWhoAttacks))
				{
					CharWhoAttacks->GetController()->Animation_CriticalBasicAttack((int)ActiveSkills::BasicAttack);
				}
				else
				{
					CharWhoAttacks->GetController()->Animation_BasicAttack((int)ActiveSkills::BasicAttack);
				}

				int reductionOfDamage = CalculateReductionOfDamage(enemyManager->GetEnemies()[j]);
				totalDamage = totalDamage - ((reductionOfDamage / 100)  * totalDamage);
				enemyManager->GetEnemies()[j]->UpdateHPCurrent(-totalDamage);
			}
		}
	}
}

///Melee
void PD_GM_LogicCharacter::Skill_Melee_Hostion(PD_GM_LogicCharacter* CharWhoAttacks, PD_GM_LogicCharacter* CharWhoReceiveTheAttacks)
{
	/*
	- Pegas un hostion muy fuerte para intentar acabar con tu enemigo de 1 golpe - Gastas todos tus AP restantes y aumentas tu ataque en ese %
	1. Calcular el daño
	2 . Calcular el AP restante
	3. Volver a calcular el daño correcto
	4. Quitar la vida a l enemigo
	5. Animaciones
	*/

	controller->UpdateRotationCharacterToEnemy(CharWhoReceiveTheAttacks->GetCharacterBP()->GetActorLocation()); //Pasarle la direccion del enemigo al que va a atacar

	//Cast<APD_E_Character>(CharWhoAttacks->GetCharacterBP())->SetCharacterCameraOnView();


	int totalDamage = (CharWhoAttacks->GetWeapon()->DMWeapon + CharWhoAttacks->GetInitBaseStats()->DMGBase) * (1 + CharWhoAttacks->GetTotalStats()->PODBonus);
	int increaseOfDMG = CalculateIncreaseOfDamage(CharWhoAttacks);
	totalDamage = totalDamage + ((increaseOfDMG / 100)  * totalDamage);

	int APLeftBonus = CalculateAPleftInPlayerActions(CharWhoAttacks);

	totalDamage = totalDamage + ((APLeftBonus / 100) * totalDamage);

	int reductionOfDamage = CalculateReductionOfDamage(CharWhoReceiveTheAttacks);
	totalDamage = totalDamage - ((reductionOfDamage / 100)  * totalDamage);
	CharWhoReceiveTheAttacks->UpdateHPCurrent(-totalDamage);

	CharWhoAttacks->GetController()->Animation_CriticalBasicAttack((int)ActiveSkills::Hostion);

}

/*RANGO*/
///Guns
void PD_GM_LogicCharacter::Skill_Range_Guns_SomeHit(PD_GM_LogicCharacter* CharWhoAttacks, PD_GM_LogicCharacter* CharWhoReceiveTheAttacks)
{
	/*
	- Hace una ragafa de disparos. El ataque acierta SIEMPRE - Gasta todo el AP
	1. Bucle for para se hagan las animaciones y se repitan los ataques (basicos)
	2. Animacionde pain de parte del atacado
	*/
	///NOTA: si vemos que va mal lo de la repeticion de las animaciones (que no se repite o da problemas) se quitara y se dejaria un efecto FX
	//Se pone 2, para que se haga una rafaga de disparos -> pero se puede poner la cifra que se quiera. 


	controller->UpdateRotationCharacterToEnemy(CharWhoReceiveTheAttacks->GetCharacterBP()->GetActorLocation()); //Pasarle la direccion del enemigo al que va a atacar

	//Cast<APD_E_Character>(CharWhoAttacks->GetCharacterBP())->SetCharacterCameraOnView();

	for (int i = 0; i < 2; i++)
	{
		int totalDamage = (CharWhoAttacks->GetWeapon()->DMWeapon + CharWhoAttacks->GetInitBaseStats()->DMGBase) * (1 + CharWhoAttacks->GetTotalStats()->PODBonus);
		int increaseOfDMG = CalculateIncreaseOfDamage(CharWhoAttacks);
		totalDamage = totalDamage + ((increaseOfDMG / 100)  * totalDamage);

		//Calcular si es critico el ataque
		if (CheckIfWasACriticalAttack(&totalDamage, CharWhoAttacks))
		{
			CharWhoAttacks->GetController()->Animation_CriticalBasicAttack((int)ActiveSkills::BasicAttack);
		}
		else
		{
			CharWhoAttacks->GetController()->Animation_BasicAttack((int)ActiveSkills::BasicAttack);
		}
		//quitarselo al charWhoRecieveTheAttacks
		int reductionOfDamage = CalculateReductionOfDamage(CharWhoReceiveTheAttacks);
		totalDamage = totalDamage - ((reductionOfDamage / 100)  * totalDamage);
		CharWhoReceiveTheAttacks->UpdateHPCurrent(-totalDamage);
	}
}

///Range
void PD_GM_LogicCharacter::Skill_Range_RightInTheAsshole(PD_GM_LogicCharacter* CharWhoAttacks, PD_GM_LogicCharacter* CharWhoReceiveTheAttacks)
{
	/*
	- Hace un ataque critico
	1. Calcular el daño
	2. hacerlo critico
	3. lanzar las respectivas animaciones
	*/
	//Cast<APD_E_Character>(CharWhoAttacks->GetCharacterBP())->SetCharacterCameraOnView();

	controller->UpdateRotationCharacterToEnemy(CharWhoReceiveTheAttacks->GetCharacterBP()->GetActorLocation()); //Pasarle la direccion del enemigo al que va a atacar

		int totalDamage = (CharWhoAttacks->GetWeapon()->DMWeapon + CharWhoAttacks->GetInitBaseStats()->DMGBase) * (1 + CharWhoAttacks->GetTotalStats()->PODBonus);
		int increaseOfDMG = CalculateIncreaseOfDamage(CharWhoAttacks);
		totalDamage = totalDamage + ((increaseOfDMG / 100)  * totalDamage);
		totalDamage = totalDamage + (totalDamage * CharWhoAttacks->GetTotalStats()->MALBonus);

		CharWhoAttacks->GetController()->Animation_CriticalBasicAttack((int)ActiveSkills::BasicAttack);

		int reductionOfDamage = CalculateReductionOfDamage(CharWhoReceiveTheAttacks);
		totalDamage = totalDamage - ((reductionOfDamage / 100)  * totalDamage);
		CharWhoReceiveTheAttacks->UpdateHPCurrent(-totalDamage);
}

/*MAGIA*/
///Magic
void PD_GM_LogicCharacter::Skill_Magic_GiveMeTheFireBlas(PD_GM_LogicCharacter* CharWhoAttacks, PD_GM_LogicCharacter* CharWhoReceiveTheAttacks)
{
	/*
	- Lanza una bola de fuego con 6 de daño ( + poder ? )
	1. Actualizar vida del atacado ?¿?
	2. Lanzar animacion de casteo de habilidad del personaje que ataca
	3. Lanzar bola de fuego
	*/
	///NOTA: Hay que comprobar el metodo ReceiveHit de los characteres, lo suyo seria que al chocar la bola de fuego es cuando se quita la vida y se ejecuta la animacion de pain y tal
	
	//Cast<APD_E_Character>(CharWhoAttacks->GetCharacterBP())->SetCharacterCameraOnView();

	//Lanzar animacion de casteo de habilidad

	controller->UpdateRotationCharacterToEnemy(CharWhoReceiveTheAttacks->GetCharacterBP()->GetActorLocation()); //Pasarle la direccion del enemigo al que va a atacar

	CharWhoAttacks->GetController()->Animation_CriticalBasicAttack((int)ActiveSkills::GiveMeTheFireBlast);
	

}
void PD_GM_LogicCharacter::Skill_Magic_ExclaimChas(PD_GM_LogicCharacter* CharWhoAttacks, PD_MG_LogicPosition PositionToTeleport)
{
	/*
	- Teleporta una casilla
	1. Animacion de Casteo de Habildad
	2. Lanzar FX de humo? o teletransporte
	3. SetActorLocation con la nueva posicion
	4. Actualizar CurrentPosition
	*/

	//Cast<APD_E_Character>(CharWhoAttacks->GetCharacterBP())->SetCharacterCameraOnView();

	FVector vectorToTeleport = FVector(mapMng->LogicToWorldPosition(PositionToTeleport));
	
	//Animacionde Casteo de Habilidad
	CharWhoAttacks->GetController()->Animation_CastSkill((int)ActiveSkills::Exclaimchas);

	//Lanzar humo ?¿?
	//Cast<APD_E_Character>(CharWhoAttacks->GetCharacterBP())->PlayAnimationSkill((int)ActiveSkills::Exclaimchas);

	CharWhoAttacks->SetCurrentLogicalPosition(PositionToTeleport);
	CharWhoAttacks->GetCharacterBP()->SetActorLocation(vectorToTeleport);

}
void PD_GM_LogicCharacter::Skill_Magic_BeInCrossroads(PD_GM_LogicCharacter* CharWhoAttacks)
{

	/*
	- Explosion de hielo que daña 3 puntos de vida a los enemigos adyacentes
	1. Conseguir a los enemigos adyacentes
	2. Animacion de casteo de habilidad
	3. FX de explosion de hielo
	4. Updatear la vida de los enemigos afectados
	5. Animacion de pain de los enemigos afectados
	*/

	//Cast<APD_E_Character>(CharWhoAttacks->GetCharacterBP())->SetCharacterCameraOnView();

	PD_GM_EnemyManager* enemyManager = Cast<UPD_ServerGameInstance>(GetCharacterBP()->GetGameInstance())->gameManager->enemyManager;

	//FX de explosion de hielo
	//Cast<APD_E_Character>(CharWhoAttacks->GetCharacterBP())->PlayAnimationSkill((int)ActiveSkills::BeInCrossroads);

	//Animacion de casteo de habilidad
	//CharWhoAttacks->GetController()->Animation_CastSkill();
	CharWhoAttacks->GetController()->Animation_BasicAttack((int)ActiveSkills::BeInCrossroads);

	//Conseguir a los enemigos adyacentes
	
	//Conseguir las casillas adyacentes y comprobar que son enemigos para cogerlos
	//updatear la vida con aquellos que te encuentres
	//lanzar animacion de pain de estos
	TArray<PD_MG_LogicPosition> nearTiles = TArray<PD_MG_LogicPosition>();

	UE_LOG(LogTemp, Warning, TEXT("LogicCharacter: Skill_Magic_BeInCrossroads"));
	if (mapMng) {
		UE_LOG(LogTemp, Warning, TEXT("LogicCharacter: hay map manager ref"));

		nearTiles = mapMng->Get_LogicPosition_Diagonals_And_Adyacents_To(GetCurrentLogicalPosition());
	}
	for (int i = 0; i < nearTiles.Num(); i++)
	{
		for (int j = 0; j < enemyManager->GetEnemies().Num(); j++)
		{
			if (nearTiles[i] == enemyManager->GetEnemies()[j]->GetCurrentLogicalPosition()) //Si coincide el baldosa a comprobar con la posicion del enemigo a comprobar, se calcula el daño
			{
				int totalDamage = (CharWhoAttacks->GetWeapon()->DMWeapon + CharWhoAttacks->GetInitBaseStats()->DMGBase) * (1 + CharWhoAttacks->GetTotalStats()->PODBonus);
				
				int reductionOfDamage = CalculateReductionOfDamage(enemyManager->GetEnemies()[j]);
				totalDamage = totalDamage - ((reductionOfDamage / 100)  * totalDamage);
				enemyManager->GetEnemies()[j]->UpdateHPCurrent(-totalDamage);
			}
		}
	}

}
void PD_GM_LogicCharacter::Skill_Magic_WhoHeal(PD_GM_LogicCharacter* CharWhoAttacks, PD_GM_LogicCharacter* CharWhoReceiveTheAttacks)
{
	/*
	- Cura 20 puntos de vida a un aliado + Los estados alterados del mismo
	1. Animacion de Casteo de HBabilidad
	2. FX sobre el afectado de curacion
	3. updatear su vida
	4. quitar los estados del afectado
	*/

	controller->UpdateRotationCharacterToEnemy(CharWhoReceiveTheAttacks->GetCharacterBP()->GetActorLocation()); //Pasarle la direccion del enemigo al que va a atacar

	//Cast<APD_E_Character>(CharWhoAttacks->GetCharacterBP())->SetCharacterCameraOnView();

	//animacion de casteo de habilidad
	CharWhoAttacks->GetController()->Animation_CastSkill((int)ActiveSkills::WhoHeal);

	CharWhoReceiveTheAttacks->UpdateHPCurrent(20);

	CharWhoReceiveTheAttacks->GetCharacterState()->alteredCharacterState.Empty();


}
void PD_GM_LogicCharacter::Skill_Magic_ShotMana(PD_GM_LogicCharacter* CharWhoAttacks)
{
	/*
	- Resetaa el CD de todas las habilidades
	1. Animacion de Casteo
	2. FX de curacion ?¿?
	3. Resetar todos los CDCurrent a 0, para que se puedan volver a usar
	*/

	///NOTA: ! Hay que pasar esta informacion a los clientes para que se actualicen
	//animacion de cast de habilidad

	//Cast<APD_E_Character>(CharWhoAttacks->GetCharacterBP())->SetCharacterCameraOnView();

	//FX de curacion
	Cast<APD_E_Character>(CharWhoAttacks->GetCharacterBP())->PlayAnimationSkill((int)ActiveSkills::WhoHeal);

	for (int i = 0; i < CharWhoAttacks->GetSkills()->listActiveSkills.Num(); i++)
	{
		CharWhoAttacks->GetSkills()->listActiveSkills[i].currentCD = 0; //seteando currentCD a 0, lo pones de disponible para que vuelva a ser utilizado
	}
}


#pragma endregion




///GET Y SET
/* ===============
METODOS GET Y SET PARA STRUCTS DE STATS y DATOS
================ */
//GET
FStructBasicStats* PD_GM_LogicCharacter::GetBasicStats() { return basicStats; }
FStructInitBaseStats* PD_GM_LogicCharacter::GetInitBaseStats() { return initBaseStats; }
FStructCharacterSkills* PD_GM_LogicCharacter::GetSkills() { return skills; }
FStructWeapon* PD_GM_LogicCharacter::GetWeapon() { return weapon; }
FStructSkin* PD_GM_LogicCharacter::GetSkin() { return skin; }
FStructTotalStats* PD_GM_LogicCharacter::GetTotalStats() { return totalStats; }
FStructCharacterState* PD_GM_LogicCharacter::GetCharacterState() { return characterState; }

bool PD_GM_LogicCharacter::GetIsPlayer() { return isPlayer; }
bool PD_GM_LogicCharacter::GetIsDead() { return isDead; }
int PD_GM_LogicCharacter::GetPoints() { return points; }
FString PD_GM_LogicCharacter::GetIDCharacter() { return ID_character; }
ECharacterType PD_GM_LogicCharacter::GetTypeCharacter() { return type_character; }
APD_GenericController* PD_GM_LogicCharacter::GetController() { return controller; }
ACharacter* PD_GM_LogicCharacter::GetCharacterBP() { return character_Player_BP; }
PD_MG_LogicPosition PD_GM_LogicCharacter::GetCurrentLogicalPosition() { return currentLogicalPosition; }
TArray<PD_MG_LogicPosition> PD_GM_LogicCharacter::GetMovingLogicalPosition() { return movingLogicalPosition; }
bool PD_GM_LogicCharacter::GetIsStoppingByCollision() { return isStoppingByCollision; }
EIAPersonality PD_GM_LogicCharacter::GetIAPersonality() { return IAPersonality; }

//SET
void PD_GM_LogicCharacter::SetBasicStats(int nPOD, int nAGI, int nDES, int nCON, int nPER, int nMAL)
{
	basicStats->POD = nPOD;
	basicStats->AGI = nAGI;
	basicStats->DES = nDES;
	basicStats->CON = nCON;
	basicStats->PER = nPER;
	basicStats->MAL = nMAL;

	for (int i = 0; i < this->skills->listPasiveSkills.Num(); i++)
	{
		switch (PasiveSkills(this->skills->listPasiveSkills[i].ID_Skill))
		{
		case PasiveSkills::IamTank:
		{
			FMath::Clamp(basicStats->CON + 3, 1, 10);
			FMath::Clamp(basicStats->AGI - 1, 1, 10);
			FMath::Clamp(basicStats->DES - 1, 1, 10);
			break;
		}
		case PasiveSkills::KingMistery:
		{
			FMath::Clamp(basicStats->POD - 1, 1, 10);
			FMath::Clamp(basicStats->AGI + 3, 1, 10);
			break;
		}
		case PasiveSkills::IamTankInDistance:
		{
			FMath::Clamp(basicStats->CON + 2, 1, 10);
			FMath::Clamp(basicStats->POD + 1, 1, 10);
			FMath::Clamp(basicStats->AGI - 2, 1, 10);
			break;
		}
		case PasiveSkills::RedBullets:
		{
			FMath::Clamp(basicStats->MAL + 4, 1, 10);
			FMath::Clamp(basicStats->DES + 1, 1, 10);
			break;
		}
		case PasiveSkills::SquabEye:
		{
			FMath::Clamp(basicStats->DES + 3, 1, 10);
			FMath::Clamp(basicStats->PER + 1, 1, 10);
			FMath::Clamp(basicStats->CON - 1, 1, 10);
			FMath::Clamp(basicStats->MAL - 1, 1, 10);
			break;
		}
		case PasiveSkills::TwoblackCandles:
		{
			FMath::Clamp(basicStats->MAL + 2, 1, 10);
			break;
		}
		case PasiveSkills::AsBull:
		{
			FMath::Clamp(basicStats->CON + 2, 1, 10);
			FMath::Clamp(basicStats->POD + 2, 1, 10);
			FMath::Clamp(basicStats->AGI - 3, 1, 10);
			break;
		}
		case PasiveSkills::TheSweeper:
		{
			FMath::Clamp(basicStats->DES - 1, 1, 10);
			FMath::Clamp(basicStats->PER + 3, 1, 10);
			break;
		}
		default:
			break;
		}
	}
}
void PD_GM_LogicCharacter::SetInitBaseStats(int nHP, int nDMG, int nAP)
{
	initBaseStats->HPBase = nHP;
	initBaseStats->DMGBase = nDMG;
	initBaseStats->APBase = nAP;
}
void PD_GM_LogicCharacter::SetSkills(TArray<FStructSkill> nActSkills, TArray<FStructSkill> nPasSkills)
{
	skills->listActiveSkills = nActSkills;
	skills->listPasiveSkills = nPasSkills;
}
void PD_GM_LogicCharacter::SetWeapon()
{
	weapon->DMWeapon = 20;
	weapon->ID_Weapon = 0;
	weapon->TypeWeapon = 0;// 0-Melee, 1-Distancia, 2-Magia
	weapon->RangeWeapon = 1;
}
void PD_GM_LogicCharacter::SetSkin()
{

}
void PD_GM_LogicCharacter::SetTotalStats()
{
	//Bonus de STATS Basicos
	totalStats->PODBonus = (0.05 * (basicStats->POD - 5));
	totalStats->AGIBonus = basicStats->AGI - 5;
	totalStats->DESBonus = basicStats->DES - 5;
	totalStats->CONBonus = (0.05 * (basicStats->CON - 5));
	//mele
	if (weapon->TypeWeapon == 0)
	{
		totalStats->PERBonus = 0.05 * basicStats->PER;
	}
	//rango
	else if (weapon->TypeWeapon == 1)
	{
		totalStats->PERBonus = (ceil(basicStats->PER / 2)) - 3;
	}
	//distancia
	else if (weapon->TypeWeapon == 2)
	{
		totalStats->PERBonus = (ceil(basicStats->PER / 2)) - 3;
	}
	totalStats->MALBonus = basicStats->MAL * 0.02;

	//Stats Secundarios
	//AP
	if ((basicStats->AGI + basicStats->DES) < 10)
	{
		totalStats->AP = floor((basicStats->AGI + basicStats->DES) / 2) - 5;
	}
	else if ((basicStats->AGI + basicStats->DES) >= 10)
	{
		totalStats->AP = ceil((basicStats->AGI + basicStats->DES) / 2) - 5;
	}
	totalStats->APTotal = initBaseStats->APBase + totalStats->AP;
	totalStats->APCurrent = totalStats->APTotal;

	//Choque
	if ((basicStats->POD + basicStats->MAL) < 10)
	{
		totalStats->CH = floor((basicStats->POD + basicStats->MAL) / 2) - 5;
	}
	else if ((basicStats->POD + basicStats->MAL) >= 10)
	{
		totalStats->CH = ceil((basicStats->POD + basicStats->MAL) / 2) - 5;
	}
	//Salvacion
	if ((basicStats->CON + basicStats->PER) < 10)
	{
		totalStats->SA = floor((basicStats->CON + basicStats->PER) / 2) - 5;
	}
	else if ((basicStats->CON + basicStats->PER) >= 10)
	{
		totalStats->SA = ceil((basicStats->POD + basicStats->PER) / 2) - 5;
	}

	//Vida total
	//	totalStats->HPTotal = (initBaseStats->HPBase + HPBonus) * (1 + totalStats->CONBonus); - HPBonus es por pasivas u otras fuentes
	totalStats->HPTotal = (initBaseStats->HPBase) * (1 + totalStats->CONBonus);
	totalStats->HPCurrent = totalStats->HPTotal;

}

void PD_GM_LogicCharacter::SetIsPlayer(bool nIsPlayer) { isPlayer = nIsPlayer; }
void PD_GM_LogicCharacter::SetIsDead(bool nIsDead) { isDead = nIsDead; }
void PD_GM_LogicCharacter::SetPoints(int inPoints) { points = inPoints; }
void PD_GM_LogicCharacter::SetIDCharacter(FString nID_character) { ID_character = nID_character; }
void PD_GM_LogicCharacter::SetTypeCharacter(ECharacterType nID_character) { type_character = nID_character; }
void PD_GM_LogicCharacter::SetController(APD_GenericController* ncontroller) { controller = ncontroller; }
void PD_GM_LogicCharacter::SetCharacterBP(ACharacter* ncharacter_Player_BP) { character_Player_BP = ncharacter_Player_BP; }
void PD_GM_LogicCharacter::SetCurrentLogicalPosition(PD_MG_LogicPosition ncurrentLogicalPosition) { currentLogicalPosition = ncurrentLogicalPosition; }
void PD_GM_LogicCharacter::AddMovementLogicalPosition(PD_MG_LogicPosition nnewLogicalPosition){	movingLogicalPosition.Add(nnewLogicalPosition);}
void PD_GM_LogicCharacter::ClearMovingLogicalPosition() { movingLogicalPosition.Empty(); }
void PD_GM_LogicCharacter::SetMapManager(PD_GM_MapManager* nmapManager) { mapMng = nmapManager; }
void PD_GM_LogicCharacter::SetIsStoppingByCollision(bool nIsStoppingByCollision) { isStoppingByCollision = nIsStoppingByCollision; }
void PD_GM_LogicCharacter::SetIAPersonality(EIAPersonality inPersonality) { IAPersonality = inPersonality; }