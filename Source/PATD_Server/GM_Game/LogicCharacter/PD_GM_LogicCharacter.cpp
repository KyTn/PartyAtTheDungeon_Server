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

	//inicializar las variables
	isDead = false;
	
}

PD_GM_LogicCharacter::~PD_GM_LogicCharacter()
{
}

/* ==============
METODOS PRINCIPALES PARA INTERACTUAR CON EL MAPA
=============== */


TArray<FStructOrderAction>* PD_GM_LogicCharacter::FindPathToMove(PD_MG_LogicPosition desirePosition)
{
	/*
	- OBJETIVO: Sirve para conseguir el listado de ordenes de movimiento que se tienen que enviar desde
	el cliente al servidor para ir a la posici�n marcada por el jugador donde se quiere mover.
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
	- Tras el choque habr�a que cambiar esa movingLogicalPosition
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
	PD_GM_EnemyManager* enemyManager = Cast<UPD_ServerGameInstance>(GetCharacterBP()->GetGameInstance())->gameManager->enemyManager;
	PD_PlayersManager* playersManager = Cast<UPD_ServerGameInstance>(GetCharacterBP()->GetGameInstance())->gameManager->playersManager;

	switch (ActiveSkills(action.id_action))
	{
		case ActiveSkills::BasicAttack:
		{
			controller->Animation_CriticalBasicAttack();
			//como es un ataque basico, siempre sera el primero de los id_characters que este en el array de targets de los characters
			PD_GM_LogicCharacter* enemy = enemyManager->GetCharacterByID(action.id_character[0]);
			if (enemy)
			{
				Skill_BasicAttack(this, enemy);
			}
			break;
		}
		case ActiveSkills::Defense:
		{
			break;
		}
		case ActiveSkills::WhenFua:
		{
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

void PD_GM_LogicCharacter::UpdateHPCurrent(float receivedDamage)
{
	/*
	- OBJETIVO: Recibir el da�o de un ataque, actualizar la vida actual y comprobar si ha muerto o no.
	NOTA: Si es negativo sera un ataque, si es positivo ser� una curacion
	- PROCESO:
	1. Pasar el float a un int, debido a que la vida es tipo int, aunque  el da�o por las formulas llegara en float
	2. Actualizar el valor del FStructTotalStats HPCurrent (Si es curacion nunca puede pasar de HPTotal
	3. Comprobar si HPCurrent <= 0. Si es asi, actualizar la variable isDead
	- SE LLAMA DESDE: Cualquier Actor - Personaje o Enemigo que da�e a otro
	*/
	if (receivedDamage > GetTotalStats()->HPCurrent) {
		GetTotalStats()->HPCurrent = 0;
	}
	else {
		GetTotalStats()->HPCurrent -= receivedDamage;
	}
	

	UE_LOG(LogTemp, Warning, TEXT("LogicCharacter:%s  come %d de da�o y tiene ahora %d de vida"), *GetIDCharacter(), receivedDamage,GetTotalStats()->HPCurrent);
	if (GetTotalStats()->HPCurrent <= 0) {
		GetCharacterBP()->SetActorHiddenInGame(true);
		UE_LOG(LogTemp, Warning, TEXT("LogicCharacter:%s se murio"),*GetIDCharacter());
		if (isPlayer) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "HAS PERDIDO PRINGAO!");
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
	- OBJETIVO: devuelve el porcentaje de impacto bonus que se a�ade por pasivas u otras fuentes
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
	float totalBonusEvasion = 1;
	/*
	- OBJETIVO: devuelve el porcentaje de impacto bonus que se a�ade por pasivas u otras fuentes
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
	//Remove the splines point and set the character
	if (!controller) {
		UE_LOG(LogTemp, Warning, TEXT("LogicCharacter: No se encuentra el controller (null)"));
	}

	if (isPlayer)
	{
		//Limpiamos los puntos del spline
		Cast<APD_CharacterController>(controller)->GetSpline()->RemovePoints();

		//Seteamos el spline en la posicion del actor
		Cast<APD_CharacterController>(controller)->GetSpline()->SetActorLocation(mapMng->LogicToWorldPosition(GetCurrentLogicalPosition()));

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
		Cast<APD_CharacterController>(controller)->GetSpline()->SetPoints(newPositionsToMove);
		//Llamamos al metodo Mover del Controller
		Cast<APD_CharacterController>(controller)->MoveTo(0, 0);

		
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

///SKILS
/* ===============
FUNCIONES QUE DEFINEN EL COMPORTAMIENTO DE LAS HABILIDADES 
================ */
#pragma region Skills Function
void PD_GM_LogicCharacter::Skill_BasicAttack(PD_GM_LogicCharacter* CharWhoAttacks, PD_GM_LogicCharacter* CharWhoReceiveTheAttacks)
{
	/*
	 1. Comprobar si le da o no -> comprobar valores de impacto y evasion
	 2. Lanzar el ataque
	*/
	if (CharWhoAttacks->GetImpactCharacter() >= CharWhoReceiveTheAttacks->GetEvasionCharacter()) //Empacto acertado
	{
		//calcular da�o -> % Porcentaje de bounus de da�o puede cambiar aqui
		int totalDamage = (CharWhoAttacks->GetWeapon()->DMWeapon + CharWhoAttacks->GetInitBaseStats()->DMGBase) * (1 + CharWhoAttacks->GetTotalStats()->PODBonus);

		//Calcular si es critico el ataque
		if (CheckIfWasACriticalAttack(&totalDamage, CharWhoAttacks))
		{
			//lanzar animacion de ataque critico
			CharWhoAttacks->GetController()->Animation_CriticalBasicAttack();
		}
		else
		{
			//lanzar animacion de ataque normal
			CharWhoAttacks->GetController()->Animation_BasicAttack();
		}

		//quitarselo al charWhoRecieveTheAttacks
		CharWhoReceiveTheAttacks->UpdateHPCurrent(totalDamage);

		//lanzar animacion de defensa

	}
	else  //Fallo del Ataque
	{

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