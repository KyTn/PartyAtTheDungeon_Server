// Fill out your copyright notice in the Description page of Project Settings.

#include "PATD_Server.h"
#include "Networking.h"
#include "PD_ServerGameInstance.h"

#include "NW_NetWorking/PD_NW_TimerActor.h"
#include "NW_NetWorking/PD_NW_NetworkManager.h"
//Includes de uso
#include "MapGeneration/Static/PD_MG_StaticMap.h"
#include "MapGeneration/Dynamic/PD_MG_DynamicMap.h"
#include "NW_Networking/Socket/PD_NW_SocketManager.h"
#include "GM_Game/LogicCharacter/PD_GM_LogicCharacter.h"
#include "GM_Game/PD_GM_SplineManager.h"
#include "Actors/PD_TimerGame.h"
#include "math.h"
#include "PD_SaveCharacterData.h"
#include "MapInfo/PD_MM_MapInfo.h"

//Includes of forward declaration
#include "Structs/PD_ServerStructs.h" //Para todos los structs y enums
#include "NW_Networking/PD_NW_NetworkManager.h"
#include "PD_PlayersManager.h"
#include "MapGeneration/PD_MG_MapGenerator.h"
#include "MapGeneration/PD_MG_MapParser.h"
#include "MapInfo/MapInstantiation/MapInstantiatorActor.h"
#include "GM_Game/PD_GM_MapManager.h"
#include "GM_Game/PD_GM_GameManager.h"
#include "GM_Game/PD_GM_EnemyManager.h"
#include "PD_MatchConfigManager.h"
#include "Actors/PD_E_ElementActor.h"
//Includes de prueba



#pragma region Get Managers
//Funciones para obtener managers
PD_GM_GameManager* UPD_ServerGameInstance::getGameManager() { return gameManager; }
PD_PlayersManager* UPD_ServerGameInstance::getPlayerManager() { return playersManager; }
#pragma endregion 

#pragma region Suscribe to events

bool UPD_ServerGameInstance::SuscribeToEvents(int inPlayer, UStructType inType) {

	return true; //de momento recibe todos, siempre es cierto.
}

#pragma endregion 


#pragma region HandleEvent Functions 

void UPD_ServerGameInstance::HandleEvent(FStructGeneric* inDataStruct, int inPlayer, UStructType inEventType) {

	if (playersManager->GetNumPlayers() >= inPlayer+1)
	{
		if (!playersManager->GetDataStructPlayer(inPlayer)->isConnected) //si el que envia esta a no conectado
			playersManager->GetDataStructPlayer(inPlayer)->isConnected = true;
	 }
	

	UE_LOG(LogTemp, Warning, TEXT("ServerGameInstance::HandleEvent:: Evento recibido:%d Estado del servidor: %d"), static_cast<uint8>(inEventType), static_cast<uint8>(structServerState->enumServerState));

	//Este IF es el Inicio de una nueva conexion. Simplemente da la bienvenida al nuevo cliente y sirve para pedir a este su ID_Cliente (Comprobar que el cliente existe o no)
	if (inEventType == UStructType::FStructNewConnection) {

		// registra un jugador entrante como jugador MASTER
		HandleEvent_NewConnection(inDataStruct, inPlayer, inEventType, true);
	}

	//Este IF es identificar si existe o no el cliente conectado y pasarle la informacion del estado del servidor y sus datos
	if (inEventType == UStructType::FStructClientID) {

		// registra un jugador entrante como jugador MASTER
		HandleEvent_IDClient(inDataStruct, inPlayer, inEventType);
	}

	if (inEventType == UStructType::FStructPing) {
		//Cuando el Servidor pide el ID Client 
		HandleEvent_PingReceive(inDataStruct, inPlayer);
	}

	if (inEventType == UStructType::FStructPong) {
		//Cuando el Servidor pide el ID Client 
		HandleEvent_PongReceive(inDataStruct, inPlayer);
	}

	///A partir de AQUI empiezan los IF de cada ESTADO
	if (structServerState->enumServerState == EServerState::StartApp) {
		// Ning�n mensaje puede llegar durante este estado
	}
	else if(structServerState->enumServerState == EServerState::WaitingMasterClient) {
		//Evento NewConnection
		if (inEventType == UStructType::FStructNewConnection) {

			// registra un jugador entrante como jugador MASTER
			//HandleEvent_NewConnection(inDataStruct, inPlayer, inEventType, true);
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("UPD_ServerGameInstance::HandleEvent:: WaitingMasterClient - NOT MATCHING "));
		}
	}	 
	else if(structServerState->enumServerState == EServerState::WaitingGameConfiguration) {

		//Evento NewConnection
		if (inEventType == UStructType::FStructNewConnection) {

			// registra un jugador entrante como jugador normal
			//HandleEvent_NewConnection(inDataStruct, inPlayer, inEventType, false);
		}
		else if (inEventType == UStructType::FStructMatchConfig) {

			// Cuando el client Master env�a la congif de la partida ... 
			HandleEvent_ConfigMatch(inDataStruct, inPlayer, inEventType);
		}
		else if (inEventType == UStructType::FStructMatchConfigDone) {

			// Cuando el client Master env�a la congif de la partida ... 
			HandleEvent_ConfigMatchDone(inDataStruct, inPlayer, inEventType);
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("UPD_ServerGameInstance::HandleEvent:: WaitingGameConfiguration - NOT MATCHING "));
		}

	}	 
	else if(structServerState->enumServerState == EServerState::Lobby_Tabern) {
		//Evento OrderMenu
		if (inEventType == UStructType::FStructOrderMenu) {

			// Cuando un jugador env�a READY, marcarlo como preparado. 
			HandleEvent_PlayerReady(inDataStruct, inPlayer, inEventType);
			//FStructOrderMenu* menuOrder = (FStructOrderMenu*)inDataStruct;
			//if (MenuOrderType(menuOrder->orderType) == MenuOrderType::ClientReady) {
			//	playersManager->getDataStructPlayer(inPlayer)->readyMenu = !playersManager->getDataStructPlayer(inPlayer)->readyMenu;
			//	this->UpdateState();//Cambio de estado
			//}
			//Evento PlayerData
		}
		else if (inEventType == UStructType::FStructCharacter) {
			//Se reciben los datos del jugador y se procesan
			HandleEvent_LoadPlayerInfo(inDataStruct, inPlayer);
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("UPD_ServerGameInstance::HandleEvent:: Lobby_Tabern - NOT MATCHING "));
		}
	}	 
	else if(structServerState->enumServerState == EServerState::Launch_Match) {

	}	 
	else if(structServerState->enumServerState == EServerState::GameInProcess) {

	}	 
	else if (structServerState->enumServerState == EServerState::Podium) {

	}
	else if(structServerState->enumServerState == EServerState::OnExit) {

	}
	else 
	{

	}
}

#pragma region Handlers

/// Registra un jugador entrante. 
/// PARAMS: 
///		FStructGeneric* inDataStruct	Estructura que recibe del NetManager
///		int inPlayer					Index del jugador que env�a
///		UStructType inEventType			Tipo de estructura
///		bool isMasterClient				registra como masterclient o no. Default: false.
///
void UPD_ServerGameInstance::HandleEvent_NewConnection(FStructGeneric* inDataStruct, int inPlayer, UStructType inEventType, bool isMasterClient = false) {
	UE_LOG(LogTemp, Warning, TEXT("ServerGameInstance::HandleEvent_NewConnection"));
	
	FStructRequestIDClient clienteWelcome;
	networkManager->SendNow(&clienteWelcome, inPlayer);

	/*
	
	*/
}

///Eventos para Ping y Pong structs -- Tema de la conexion
void UPD_ServerGameInstance::HandleEvent_PingReceive(FStructGeneric* inDataStruct, int inPlayer)
{
	FStructPong* pong = new FStructPong();
	networkManager->SendNow(pong, inPlayer);
}

void UPD_ServerGameInstance::HandleEvent_PongReceive(FStructGeneric* inDataStruct, int inPlayer) //Desde el client inPlayer le envian un pong (por envio del ping desde el servidor)
{
	playersManager->GetDataPlayers()[inPlayer]->pingPong = 2;
}




///Cuando el Cliente envia su IDCliente al Servidor
void UPD_ServerGameInstance::HandleEvent_IDClient(FStructGeneric* inDataStruct, int inPlayer, UStructType inEventType)
{
	UE_LOG(LogTemp, Warning, TEXT("UPD_ServerGameInstance::HandleEvent_IDClient"));

	FStructClientID* ID_Client = (FStructClientID*)inDataStruct;
	StructPlayer* aux_theClient = playersManager->GetStructPlayerByIDClient(ID_Client->ID_Client);
	if (aux_theClient) //Existe el Cliente ya en el Server - Reconexi�n
	{
		networkManager->GetSocketManager()->ReconnectSockets(aux_theClient->ID_PLAYER, inPlayer);

		if (playersManager->GetDataPlayers()[aux_theClient->ID_PLAYER]->isConnected)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("El Jugador con Numero: %d, se ha conectado - pero connected a true"), aux_theClient->ID_PLAYER));

		}
		else 
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("El Jugador con Numero: %d, se ha Conectado - pero connected a false"), aux_theClient->ID_PLAYER));

		}
		playersManager->GetDataPlayers()[aux_theClient->ID_PLAYER]->isConnected = true; //Seteamos la variable connected a true para el jugador reconectado
		playersManager->GetDataPlayers()[aux_theClient->ID_PLAYER]->pingPong = 2; //se pone a 2 para indicar que esta conectado (luego volvera a 1 cuando se vuelva a lanzar el broadcast de ping

		if (structServerState->enumServerState == EServerState::WaitingGameConfiguration) //Menu Principal
		{
			UE_LOG(LogTemp, Warning, TEXT("UPD_ServerGameInstance::HandleEvent_IDClient - WaitingGameConfiguration"));
			FStructWelcome clientResponse;
			clientResponse.GameState = static_cast<uint8>(GameState::ConfigureMatch);
			clientResponse.playerIndex = aux_theClient->ID_PLAYER;
			clientResponse.isClientMaster = aux_theClient->clientMaster;
			networkManager->SendNow(&clientResponse, aux_theClient->ID_PLAYER);
		}
		else if (structServerState->enumServerState == EServerState::Lobby_Tabern) {
			UE_LOG(LogTemp, Warning, TEXT("UPD_ServerGameInstance::HandleEvent_IDClient - Lobby_Tabern"));
			FStructWelcome clientResponse;
			clientResponse.GameState = static_cast<uint8>(GameState::Lobby_Tabern);
			clientResponse.playerIndex = aux_theClient->ID_PLAYER;
			clientResponse.isClientMaster = aux_theClient->clientMaster;
			networkManager->SendNow(&clientResponse, aux_theClient->ID_PLAYER);
			/*  */

			
			//Enviamos el string del map, que se envia en condiciones normales cuando el lobby se inicializa (despues de la carga del mapa de lobby en el servidor)
			//BroadcastMapString();
			BroadcastFStructMapData(mapManager->MapInfo->NETMAPDATA);
			//Con esto ya va a la taberna.
		}
		else if (structServerState->enumServerState == EServerState::GameInProcess) {
			//
			UE_LOG(LogTemp, Warning, TEXT("UPD_ServerGameInstance::HandleEvent_IDClient - GameInProcess"));
			FStructWelcome clientResponse;
			clientResponse.GameState = static_cast<uint8>(GameState::GameInProcess);
			clientResponse.playerIndex = aux_theClient->ID_PLAYER;
			clientResponse.isClientMaster = aux_theClient->clientMaster;
			networkManager->SendNow(&clientResponse, aux_theClient->ID_PLAYER);
			//Con esto iria al ReconnectingInGame state


			

			//Enviamos el string del map, que se envia en condiciones normales cuando el lobby se inicializa (despues de la carga del mapa de lobby en el servidor)
			//BroadcastMapString();
			BroadcastFStructMapData(mapManager->MapInfo->NETMAPDATA);
			//Habria que pasarle un struct con su propio personaje, ya que no lo va a poder crear porque no pasa por el lobby
			FStructCharacter characterSend = GenerateFStructCharacter(aux_theClient->ID_PLAYER);
			networkManager->SendNow(&characterSend, aux_theClient->ID_PLAYER);

		

			//Con esto hacemos que haga launchMatch como cuando todos dan ready en el lobby
			FStructClientStartMatchOnGM launchMatch = FStructClientStartMatchOnGM();
			networkManager->SendNow(&launchMatch, -1);

			FStructUpdateTurn structUpdateTurn = gameManager->GenerateStructUpdateTurn();
			if (gameManager->structGameState->enumGameState == EGameState::WaitingPlayerOrders) {
				structUpdateTurn.goToGenerateOrders = true;
				networkManager->SendNow(&structUpdateTurn, aux_theClient->ID_PLAYER);

			}
			else {
				structUpdateTurn.goToGenerateOrders = false;
			}


			BroadcastInstantiatePlayers();



			

		}else if (structServerState->enumServerState == EServerState::Launch_Match) {
			//Aqui yo pienso que podemos no dejar entrar al nuevo. Poner algo asi como "el juego esta cargando, por favor intenta reconectarte cuando termine"
			UE_LOG(LogTemp, Warning, TEXT("UPD_ServerGameInstance::HandleEvent_IDClient - Launch_Match"));

		}
	}
	else //No existe el Cliente en el Server - Nuevo Cliente (SI PROCEDE SEGUN ESTADO)
	{
		if (structServerState->enumServerState == EServerState::WaitingMasterClient) //Inicio
		{
			UE_LOG(LogTemp, Warning, TEXT("UPD_ServerGameInstance::HandleEvent_IDClient - WaitingMasterClient - NEW CLIENT"));

			//Registrar en playersManager
			playersManager->AddNewPlayer(ID_Client->ID_Client, inPlayer);

			StructPlayer* structPlayer = playersManager->GetDataStructPlayer(inPlayer);
			structPlayer->clientMaster = true; //Es el ClientMaster
			//Contestar al cliente
			//Enviamos al cliente el success 
			FStructWelcome clientResponse;
			clientResponse.GameState = static_cast<uint8>(GameState::ConfigureMatch);
			clientResponse.playerIndex = inPlayer;
			clientResponse.isClientMaster = true;
			networkManager->SendNow(&clientResponse, inPlayer);

			//Como ya tenemos un Cliente, actualizamos el estado a menu principal del Servidor
			this->UpdateState();
		}
		else if (structServerState->enumServerState == EServerState::WaitingGameConfiguration) //Menu Principal
		{
			UE_LOG(LogTemp, Warning, TEXT("UPD_ServerGameInstance::HandleEvent_IDClient - WaitingGameConfiguration - NEW CLIENT"));

			//Registrar en playersManager
			playersManager->AddNewPlayer(ID_Client->ID_Client, inPlayer);

			StructPlayer* structPlayer = playersManager->GetDataStructPlayer(inPlayer);
			structPlayer->clientMaster = false; //Es el ClientMaster
											   //Contestar al cliente
											   //Enviamos al cliente el success 
			FStructWelcome clientResponse;
			clientResponse.GameState = static_cast<uint8>(GameState::WaitingMatchConfiguration);
			clientResponse.playerIndex = inPlayer;
			clientResponse.isClientMaster = false;
			networkManager->SendNow(&clientResponse, inPlayer);
		}
		else  //EL JUEGO SE ENCUENTRA EN PARTIDA  Y NO ACEPTA NUEVOS CLIENTES
		{
			UE_LOG(LogTemp, Warning, TEXT("UPD_ServerGameInstance::HandleEvent_IDClient - CONECTION NOT ALLOWED - NEW CLIENT"));

			FStructWelcome clientResponse;
			clientResponse.GameState = static_cast<uint8>(GameState::NoConnectionAllowed);

			networkManager->SendNow(&clientResponse, inPlayer);
		}
	}
}



/// Cuando el MasterClient cambia un valor de la configuraci�n del match ...
void UPD_ServerGameInstance::HandleEvent_ConfigMatch(FStructGeneric* inDataStruct, int inPlayer, UStructType inEventType) {
	UE_LOG(LogTemp, Warning, TEXT("ServerGameInstance::HandleEvent_ConfigMatch"));
	/*
	FStructOrderMenu* menuOrder = (FStructOrderMenu*)inDataStruct;
	if (MenuOrderType(menuOrder->orderType) == MenuOrderType::GameConfigurationDone) {
		//Solo si lo envia el clientMaster
		if (inPlayer == playersManager->GetIndexClientMaster()) {
			structServerState->gameConfigurationDone = true;
			this->UpdateState();//Cambio de estado
		}
	}
	*/
	FStructMatchConfig* StructMatchConfig = (FStructMatchConfig*) inDataStruct;

	// Obtener la informaci�n y actualizar la configuraci�n del match en el servidor:
	// StructMatchConfig.id		: id del campo a cambiar
	// StructMatchConfig.value	: nuevo valor del campo id

	switch (StructMatchConfig->id) {
	case 0:
		MatchConfigManager->SetAndSend_MissionType(static_cast<MATCHCONFIG_MISSIONTYPE>(StructMatchConfig->intvalue));
		break;
	case 1:
		MatchConfigManager->SetAndSend_MapSize(static_cast<MATCHCONFIG_MAPSIZE>(StructMatchConfig->intvalue));
		break;
	case 2:
		MatchConfigManager->SetAndSend_Difficulty(static_cast<MATCHCONFIG_DIFFICULTY>(StructMatchConfig->intvalue));
		break;
	}
	// Enviar la informaci�n confirmada a los clientes ... 
}

/// Cuando el MasterClient termina de configurar el Match ... 
void UPD_ServerGameInstance::HandleEvent_ConfigMatchDone(FStructGeneric* inDataStruct, int inPlayer, UStructType inEventType) {
	UE_LOG(LogTemp, Warning, TEXT("ServerGameInstance::HandleEvent_ConfigMatchDone"));
	if (inPlayer == playersManager->GetIndexClientMaster()) {
		structServerState->gameConfigurationDone = true;

		// mandar el MatchConfigDone a todos los clientes, masterclient incluido, como confirmaci�n.
		BroadcastEndMatchConfig();

		this->UpdateState();//Cambio de estado
	}


}



void UPD_ServerGameInstance::HandleEvent_LoadPlayerInfo(FStructGeneric* inDataStruct, int inPlayer) {
	UE_LOG(LogTemp, Warning, TEXT("ServerGameInstance::HandleEvent_LoadPlayerInfo"));
	FStructCharacter* playerStats = (FStructCharacter*)inDataStruct;

	//playersManager->GetDataStructPlayer(inPlayer)->logic_Character = new PD_GM_LogicCharacter();

	playersManager->GetDataStructPlayer(inPlayer)->logic_Character->totalStats = &playerStats->totalStats;
	playersManager->GetDataStructPlayer(inPlayer)->logic_Character->basicStats = &playerStats->basicStats;
	playersManager->GetDataStructPlayer(inPlayer)->logic_Character->initBaseStats = &playerStats->initBaseStats;
	playersManager->GetDataStructPlayer(inPlayer)->logic_Character->skills = &playerStats->skills;
	playersManager->GetDataStructPlayer(inPlayer)->logic_Character->weapon = &playerStats->weapon;
	playersManager->GetDataStructPlayer(inPlayer)->logic_Character->skin = &playerStats->skin;
	playersManager->GetDataStructPlayer(inPlayer)->logic_Character->characterState = &playerStats->charState;
	FString id_character = "Player_0";
	id_character = id_character.Append(FString::FromInt(inPlayer));
	playersManager->GetDataStructPlayer(inPlayer)->logic_Character->SetIDCharacter(id_character);
}

void UPD_ServerGameInstance::HandleEvent_PlayerReady(FStructGeneric* inDataStruct, int inPlayer, UStructType inEventType) {
	UE_LOG(LogTemp, Warning, TEXT("ServerGameInstance::HandleEvent_PlayerReady"));

	FStructOrderMenu* menuOrder = (FStructOrderMenu*)inDataStruct;
	if (MenuOrderType(menuOrder->orderType) == MenuOrderType::ClientReady) {
		playersManager->GetDataStructPlayer(inPlayer)->readyMenu = menuOrder->orderType != 0;
		//playersManager->getDataStructPlayer(inPlayer)->readyMenu = !playersManager->getDataStructPlayer(inPlayer)->readyMenu;
		this->UpdateState();//Cambio de estado a GameInProcess
	}
}

#pragma endregion


void UPD_ServerGameInstance::BroadcastMapString() {
	FStructMapData * map = mapManager->MapInfo->NETMAPDATA;
	//Enviar mapa al cliente
	networkManager->SendNow(map, -1);
}

void UPD_ServerGameInstance::BroadcastFStructMapData(FStructMapData * NetMapData)
{
	networkManager->SendNow(NetMapData, -1);
}

void UPD_ServerGameInstance::BroadcastInstantiatePlayers() {

	//Enviar lista de players al cliente
	FStructInstatiatePlayers listInstantiatePlayers;
	for (int i = 0; i < playersManager->GetNumPlayers(); i++) {
		if (i != playersManager->GetDataStructPlayer(i)->ID_PLAYER) {
			UE_LOG(LogTemp, Log, TEXT("UPD_ServerGameInstance::OnBeginState(): EServerState::Launch_Match:  Launch_Match: id_player e indice de players manager no coincide"));
		}
		FStructPlayerInfoAtClient infoPlayer;
		infoPlayer.playerNum = playersManager->GetDataStructPlayer(i)->ID_PLAYER;
		infoPlayer.ID_character = playersManager->GetDataStructPlayer(i)->logic_Character->GetIDCharacter();
		infoPlayer.structSkin = *playersManager->GetDataStructPlayer(i)->logic_Character->GetSkin();
		PD_MG_LogicPosition pos = playersManager->GetDataStructPlayer(i)->logic_Character->GetCurrentLogicalPosition();

		UE_LOG(LogTemp, Log, TEXT("PD_GM_GameManager::OnBeginState: Start_Match: player %d at (%d,%d)"), i, pos.GetX(), pos.GetY());

		infoPlayer.logicPosition.positionX = pos.GetX();
		infoPlayer.logicPosition.positionY = pos.GetY();
		listInstantiatePlayers.listInfoPlayerAtClient.Add(infoPlayer);
	}

	//No hacemos broadcast porque enviamos un paquete a cada uno indicando cual es el ID de su propio personaje
	for (int i = 0; i < playersManager->GetNumPlayers(); i++) {
		listInstantiatePlayers.idClientCharacter = playersManager->GetDataStructPlayer(i)->logic_Character->GetIDCharacter();
		networkManager->SendNow(&listInstantiatePlayers, i);
	}
}
void UPD_ServerGameInstance::BroadcastMatchConfigUpdate(int id, int intvalue, FString FStringvalue)
{
	FStructMatchConfig msg = FStructMatchConfig();

	msg.id = id;
	msg.intvalue = intvalue;
	msg.FStringvalue = FStringvalue;

	networkManager->SendNow(&msg, -1);
}
void UPD_ServerGameInstance::BroadcastMatchConfigFullUpdate()
{
	FStructMatchConfig msg = FStructMatchConfig();

	// Se manda el tipo de mission
	msg.id = 0;
	msg.intvalue = (int)MatchConfigManager->Get_MissionType();
	//msg.FStringvalue = FStringvalue;

	networkManager->SendNow(&msg, -1);

	// Se manda el tama�o del mapa
	msg.id = 1;
	msg.intvalue = (int)MatchConfigManager->Get_MapSize();
	//msg.FStringvalue = FStringvalue;

	networkManager->SendNow(&msg, -1);

	// Se manda el tipo de difficulty
	msg.id = 2;
	msg.intvalue = (int)MatchConfigManager->Get_Difficulty();
	//msg.FStringvalue = FStringvalue;

	networkManager->SendNow(&msg, -1);


}
void UPD_ServerGameInstance::BroadcastEndMatchConfig()
{
	FStructMatchConfigDone msg = FStructMatchConfigDone();
	msg.from = 0;
	networkManager->SendNow(&msg, -1);

}

FStructCharacter UPD_ServerGameInstance::GenerateFStructCharacter(int indexPlayer) {
	FStructCharacter structCharacter = FStructCharacter();
	structCharacter.totalStats = *playersManager->GetCharacterByIndex(indexPlayer)->totalStats;
	structCharacter.basicStats = *playersManager->GetCharacterByIndex(indexPlayer)->basicStats;
	structCharacter.initBaseStats = *playersManager->GetCharacterByIndex(indexPlayer)->initBaseStats;
	structCharacter.skills = *playersManager->GetCharacterByIndex(indexPlayer)->skills;
	structCharacter.weapon = *playersManager->GetCharacterByIndex(indexPlayer)->weapon;
	structCharacter.skin = *playersManager->GetCharacterByIndex(indexPlayer)->skin;
	structCharacter.charState = *playersManager->GetCharacterByIndex(indexPlayer)->characterState;
	return structCharacter;
}

#pragma endregion

#pragma region ONBEGIN & UPDATE STATES

void UPD_ServerGameInstance::UpdateState() {


	if (structServerState->enumServerState == EServerState::StartApp) {

	}
	else if (structServerState->enumServerState == EServerState::WaitingMasterClient) {
		//Transiciones de estados
		if (playersManager->GetIndexClientMaster() != -1) {
			this->ChangeState(EServerState::WaitingGameConfiguration);
		}
	}
	else if (structServerState->enumServerState == EServerState::WaitingGameConfiguration) {
		//Transiciones de estados
		if (structServerState->gameConfigurationDone) {
			this->ChangeState(EServerState::Lobby_Tabern);
		}
	}
	else if (structServerState->enumServerState == EServerState::Lobby_Tabern) {
		// Si todos los jugadores est�n ready
		if (playersManager->AllPlayersReady()) {
			// enviamos a los jugadores que ya pueden ir al Launch_Match

			FStructClientStartMatchOnGM launchMatch = FStructClientStartMatchOnGM();
			networkManager->SendNow(&launchMatch, -1);
	
			this->ChangeState(EServerState::Launch_Match);
		}
	}
	else if (structServerState->enumServerState == EServerState::Launch_Match) {

	}
	else if (structServerState->enumServerState == EServerState::GameInProcess) {
		//Enviar cambio a podium
		FStructEndOfMatch endMatch = FStructEndOfMatch();
		networkManager->SendNow(&endMatch, -1);

		//cambiar a podium
		this->ChangeState(EServerState::Podium);
	}
	else if (structServerState->enumServerState == EServerState::Podium) {

	}
	else if (structServerState->enumServerState == EServerState::OnExit) {

	}
	else
	{

	}
}

void UPD_ServerGameInstance::ChangeState(EServerState newState) {
	switch (newState)
	{
	case EServerState::StartApp:
		UE_LOG(LogTemp, Warning, TEXT("ServerGameInstance::ChangeState to StartApp"));
		break;
	case EServerState::WaitingMasterClient:
		UE_LOG(LogTemp, Warning, TEXT("ServerGameInstance::ChangeState to WaitingMasterClient"));
		break;
	case EServerState::WaitingGameConfiguration:
		UE_LOG(LogTemp, Warning, TEXT("ServerGameInstance::ChangeState to WaitingGameConfiguration"));
		break;
	case EServerState::Lobby_Tabern:
		UE_LOG(LogTemp, Warning, TEXT("ServerGameInstance::ChangeState to Lobby_Tabern"));
		break;
	case EServerState::Launch_Match:
		UE_LOG(LogTemp, Warning, TEXT("ServerGameInstance::ChangeState to Launch_Match"));
		break;
	case EServerState::GameInProcess:
		UE_LOG(LogTemp, Warning, TEXT("ServerGameInstance::ChangeState to GameInProcess"));
		break;
	case EServerState::Podium:
		UE_LOG(LogTemp, Warning, TEXT("ServerGameInstance::ChangeState to Podium"));
		break;
	case EServerState::OnExit:
		UE_LOG(LogTemp, Warning, TEXT("ServerGameInstance::ChangeState to OnExit"));
		break;
	default:
		break;
	}
	
	structServerState->enumServerState = newState;
	OnBeginState();
}

void UPD_ServerGameInstance::OnBeginState() {

	if (structServerState->enumServerState == EServerState::StartApp) {

		//Aqui har�amos una espera para la pantalla de inicio, animaci�n inicial y tal ... 

		UE_LOG(LogTemp, Warning, TEXT("PD_GM_MapManager::OnBeginState - Init MapGenerator ..."));
		//TArray<RoomTemplateInfo> roomsTemplateInfoArray;
		//mapGenerationUtils->ReadAndPrepareRoomTemplateInfosFromFile("Content/DungeonTestingMaps/test1.rooms", roomsTemplateInfoArray);

		mapGenerator->Init();
		

		ChangeState(EServerState::WaitingMasterClient);
	}
	else if (structServerState->enumServerState == EServerState::WaitingMasterClient) {
		
	}
	else if (structServerState->enumServerState == EServerState::WaitingGameConfiguration) {
		this->LoadMap(levelsNameDictionary.GetMapName(2));
	}
	else if (structServerState->enumServerState == EServerState::Lobby_Tabern) {
		/* 
		FStructOrderMenu clientBroadcast;
		clientBroadcast.orderType = static_cast<uint8>(MenuOrderType::ChangeToLobby);
		networkManager->SendNow(&clientBroadcast, -1);
		/*  */

		//mapPath = mapGenerator->GenerateProceduralMap_v01(MatchConfigManager, playersManager->GetDataPlayers().Num());

		//NETMAPDATA = mapGenerator->GenerateProcedural_FStructMapData_v02(MatchConfigManager, playersManager->GetDataPlayers().Num());

		this->LoadMap(levelsNameDictionary.GetMapName(3));

	}
	else if (structServerState->enumServerState == EServerState::Launch_Match) {
		/*
		Generar los puntos de Spawn de los Personajes - Si no, tienen 0.0 --> El InstiantateMap se hace DESPUES QUE ESTO Y NO SIRVE
		ACTUALIZAR EL CURRENTLOGICPOSTION de los Personajes de los Jugadores
		*/
		TArray<PD_MG_LogicPosition> spawn = mapManager->GetSpawnPoints();
		UE_LOG(LogTemp, Warning, TEXT("PD_GM_MapManager::InstantiateDynamicMap - Spawn points Num %d"), spawn.Num());

		struct FRandomStream randomize = FRandomStream(spawn.Num());
		TArray<PD_MG_LogicPosition> spawnPoints = TArray<PD_MG_LogicPosition>();

		for (int32 j = 0; j < spawn.Num(); j++) //Shuffel del GetSpawnPoint
		{

			int index = rand() % (j + 1);
			spawn.Swap(j, index);
			//spawnPoints.Add(spawn[randomize.RandHelper(spawn.Num())]);
		}

		if (spawn.Num() > 0)
		{
			for (int i = 0; i < playersManager->GetNumPlayers(); i++)
			{
				UE_LOG(LogTemp, Warning, TEXT("tructServerState->enumServerState == EServerState::Launch_Match - Player %d is at (%d,%d)"), i, spawn[i].GetX(), spawn[i].GetY());
				playersManager->GetDataPlayers()[i]->logic_Character->SetCurrentLogicalPosition(spawn[i]);

			}
		}
		



		//Enviar lista de players al cliente
		BroadcastInstantiatePlayers();

		//Broadcast del listInstantiatePlayers
	//	networkManager->SendNow(&listInstantiatePlayers, -1);

		ChangeState(EServerState::GameInProcess);
		
	}
	else if (structServerState->enumServerState == EServerState::GameInProcess) {
		
		this->LoadMap(levelsNameDictionary.GetMapName(4));//Mapa de juego
	}
	else if (structServerState->enumServerState == EServerState::Podium) {

		this->LoadMap(levelsNameDictionary.GetMapName(5));//Mapa de Podium

	}
	else if (structServerState->enumServerState == EServerState::OnExit) {

	}
	else
	{

	}

}

#pragma endregion 


#pragma region MAPS 
void UPD_ServerGameInstance::LoadMap(FString mapName)
{
	float musicDuration = backgroundMusic->GetAudioComponent()->Sound->GetDuration();

	timeCurrentMusic = UGameplayStatics::GetUnpausedTimeSeconds(GetWorld()) - timeStartMusicBackground;

	if (timeCurrentMusic >= musicDuration)
		timeCurrentMusic = timeCurrentMusic - musicDuration;

	UGameplayStatics::OpenLevel((UObject*)this, FName(*mapName));

}

//Callback cuando el mapa este cargado
void UPD_ServerGameInstance::OnLoadedLevel() {
	//Sin importar el estado hacemos:
	APD_NW_TimerActor* TimerActorSpawned = (APD_NW_TimerActor*)GetWorld()->SpawnActor(APD_NW_TimerActor::StaticClass());
	networkManager->GetSocketManager()->InitTimerActor(TimerActorSpawned);

	if (structServerState->enumServerState == EServerState::StartApp) {

	}
	else if (structServerState->enumServerState == EServerState::WaitingMasterClient) {
		
	}
	else if (structServerState->enumServerState == EServerState::WaitingGameConfiguration) {

	}
	else if (structServerState->enumServerState == EServerState::Lobby_Tabern) {
		
		// Mientras est�n en el lobby, se genera el mapa y se env�a a los jugadores (cuando respondan que est�n en el lobby). 
		NETMAPDATA = mapGenerator->GenerateProcedural_FStructMapData_v02(MatchConfigManager, playersManager->GetDataPlayers().Num());

		mapParser = new PD_MG_MapParser();

		PD_MG_StaticMap* staticMapRef = new PD_MG_StaticMap();
		PD_MG_DynamicMap* dynamicMapRef = new PD_MG_DynamicMap();

		// Parsea el chorizo
		//mapParser->StartParsingFromFile(&mapPath, staticMapRef, dynamicMapRef);
		//mapParser->StartParsingFromChorizo(&mapPath, staticMapRef, dynamicMapRef);

		mapManager = new PD_GM_MapManager();
		mapManager->Init();

		mapParser->StartParsingFromFStructMapData(NETMAPDATA, mapManager->MapInfo, mapManager->DynamicMapRef);
		mapManager->MapInfo->NETMAPDATA = NETMAPDATA;

		// GENERAR EL USTRUC / Static&Dynamic Map



		//mapManager = new PD_GM_MapManager();
		//mapManager->Init(staticMapRef, dynamicMapRef); // inicializa las estructuras internas del mapManager (MapInfo)
		
		mapManager->MapInfo->ShowMapData();
		ShowMapInfo(mapManager->MapInfo);

		/*  */
		//BroadcastMapString();
		BroadcastFStructMapData(mapManager->MapInfo->NETMAPDATA);
		/*  */
	}
	else if (structServerState->enumServerState == EServerState::Launch_Match) {

	}
	else if (structServerState->enumServerState == EServerState::GameInProcess) {
		
		// le pasamos al mapManager un instanciador
		AMapInstantiatorActor* InstantiatorActor = (AMapInstantiatorActor*)GetWorld()->SpawnActor(AMapInstantiatorActor::StaticClass());
		mapManager->instantiator = InstantiatorActor;



		// le decimos al mapManager que instancia las cosicas en el mundo
		//mapManager->InstantiateStaticMap();
		//mapManager->InstantiateDynamicMap();

		//Aqui cedemos el control al GameManager.
		APD_GM_SplineManager* splineManager = (APD_GM_SplineManager*)GetWorld()->SpawnActor(APD_GM_SplineManager::StaticClass());
		APD_TimerGame* timer= (APD_TimerGame*)GetWorld()->SpawnActor(APD_TimerGame::StaticClass());


		gameManager = new PD_GM_GameManager(playersManager, mapManager, networkManager, splineManager, timer);


	}
	else if (structServerState->enumServerState == EServerState::Podium) {
		
		APD_TimerGame* timerOnPodium = (APD_TimerGame*)GetWorld()->SpawnActor(APD_TimerGame::StaticClass());

		UE_LOG(LogTemp, Warning, TEXT("enumServerState == EServerState::Podium -"));

		timerOnPodium->InitTimerPodium(5.0f, this);
		
	}
	else if (structServerState->enumServerState == EServerState::OnExit) {

	}
	else
	{

	}
}


void UPD_ServerGameInstance::ShowMapInfo(PD_MM_MapInfo* mapInfo) {

	UE_LOG(LogTemp, Log, TEXT("UPD_ServerGameInstance::ShowMapInfo - Num allLogicPos %d "), mapInfo->allLogicPos.Num());
	UE_LOG(LogTemp, Log, TEXT("UPD_ServerGameInstance::ShowMapInfo - Num mapAdj %d "), mapInfo->mapAdj.Num());
	UE_LOG(LogTemp, Log, TEXT("UPD_ServerGameInstance::ShowMapInfo - MAP_SIZE_IN_LOGIC_POSITIONS (%d, %d)"), mapInfo->MAP_SIZE_IN_LOGIC_POSITIONS.GetX(), mapInfo->MAP_SIZE_IN_LOGIC_POSITIONS.GetY());
	UE_LOG(LogTemp, Log, TEXT("UPD_ServerGameInstance::ShowMapInfo - Num roomByIDRoom %d"), mapInfo->roomByIDRoom.Num());
	UE_LOG(LogTemp, Log, TEXT("UPD_ServerGameInstance::ShowMapInfo - Num roomByLogPos %d"), mapInfo->roomByLogPos.Num());
	UE_LOG(LogTemp, Log, TEXT("UPD_ServerGameInstance::ShowMapInfo - Num rooms %d"), mapInfo->rooms.Num());
	UE_LOG(LogTemp, Log, TEXT("UPD_ServerGameInstance::ShowMapInfo - SpawnRoom ID %d"), mapInfo->SpawnRoom->IDRoom);


}


#pragma endregion

#pragma region INITS

// Inicializa el GameInstance
void UPD_ServerGameInstance::Init()
{
	Super::Init();
	UE_LOG(LogTemp, Warning, TEXT("Init GameInstance ~> "));
	
	//Inicializar Arrays de Skills and Weapons
	LoadSkillActiveDatafromFile();
	LoadSkillPasiveDatafromFile();
	LoadWeaponDataFromFile();
	
	levelsNameDictionary = LevelsNameDictionary(); 

	playersManager = new PD_PlayersManager();

	structServerState = new StructServerState();

	mapGenerator = new PD_MG_MapGenerator();

	//Set del MapString para mapas "aleatorios"

	/*
	switch (FMath::RandHelper(3)) {
		case 0: {
			mapPath = "Content/DungeonTestingMaps/test3.dungeon";
			mapX = 20;
			mapY = 20;
		}break;
		case 1: {
			mapPath = "Content/DungeonTestingMaps/test4.dungeon";
			mapX = 20;
			mapY = 20;
		}break;
		case 2: {
			mapPath = "Content/DungeonTestingMaps/test5.dungeon";
			mapX = 30;
			mapY = 30;
		}
	}
	*/
	ChangeState(EServerState::StartApp);
	//structServerState->enumServerState = EServerState::StartApp;
	

	//en el inicialize networking seteamos el gameinstance como observador.
	InitializeNetworking();

	MatchConfigManager = new PD_MatchConfigManager(this);

}

// Inicializa los servicios de red, el NetworkManager. 
void UPD_ServerGameInstance::InitializeNetworking()
{

	InitializeServerAddress();

	networkManager = new PD_NW_NetworkManager();
	PD_NW_SocketManager* socketManager = networkManager->GetSocketManager();

	socketManager->SetIsServer(true);

	APD_NW_TimerActor* ServerActorSpawned = (APD_NW_TimerActor*)GetWorld()->SpawnActor(APD_NW_TimerActor::StaticClass());
	socketManager->SetNetworkManager(networkManager);

	if (serverIP.Equals("NO CONNECTION"))
	{

	}
	else 
	{
		socketManager->Init(ServerActorSpawned, serverIP, defaultServerPort);//Con esto empezaria el timer

	 //Seteamos este gameinstance como observador de eventos en el networkmanager.
		networkManager->RegisterObserver(this);
	}
	

}



#pragma endregion


#pragma region SHUTDOWNS

void UPD_ServerGameInstance::Shutdown()
{
	Super::Shutdown();

	if (networkManager) {
	delete networkManager;
	}
	//Aqui habria que hacer muchos deletes 
	
}


#pragma endregion


#pragma region Comments

/*
//Format IP String as Number Parts
void UPD_ServerGameInstance::castIP(const FString& TheIP)
{
	//IP Formatting
	TheIP.Replace(TEXT(" "), TEXT(""));

	//String Parts
	TArray<FString> Parts;
	TheIP.ParseIntoArray(Parts, TEXT("."), true);

	//String to Number Parts
	for (int32 i = 0; i < 4; ++i)
	{
		ip.Add(FCString::Atoi(*Parts[i]));
	}
	UE_LOG(LogTemp, Warning, TEXT("My ip %d"), ip[0]);
	UE_LOG(LogTemp, Warning, TEXT("My ip %d"), ip[1]);
	UE_LOG(LogTemp, Warning, TEXT("My ip %d"), ip[2]);
	UE_LOG(LogTemp, Warning, TEXT("My ip %d"), ip[3]);
}*/


/*
PD_NW_SocketManager* UPD_ServerGameInstance::GetSocketManager()
{
	return socketManager;
}*/


#pragma endregion


/*************************************
******* FUNCIONES UTILIDADES / BP
*************************************/

#pragma region Blueprint_Functions 


void UPD_ServerGameInstance::InitializeServerAddress()
{
	FString myIP = "";
	FString auxmyIP = "";
	TArray < TSharedPtr<FInternetAddr>> myIPAddress;
	FString myServerName =  "";
	FString textToPrint = "";

	
	if (ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->HasNetworkDevice()) //Se puede usar funciones de red
	{
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->GetHostName(myServerName); //Get Server Name HOST
		
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->GetLocalAdapterAddresses(myIPAddress); //Get List of Address in Host
		
		if (myIPAddress.Num() > 0)
		{
			for (int i = 0; i < 1; i++)///Hay que recorrerlo m�s adelante mirando todas
			{
				auxmyIP = myIPAddress[i]->ToString(false);
				myIP.Append(auxmyIP);
			}
			//UE_LOG(LogTemp, Warning, TEXT("My ip %d"), myIP.Len());
			serverIP = myIP;
			serverName = myServerName;
		}
		else
		{
			serverIP = "NO CONNECTION";
			serverName = "NO SERVER";
		}
	}
	else  //No hay un dispositivo de red / no esta bien configurada la tarjeta de red
	{/*
		textToPrint = TEXT("Device has not a Properly Configured NETWORK device to set a SERVER");
		*/
		UE_LOG(LogTemp, Warning, TEXT("ERROR : GetServerIP:Device has not a Properly Configured NETWORK device to set a SERVER: My ip %d"), myIP.Len());
		
	}

}

FString UPD_ServerGameInstance::GetServerIP() {
	return serverIP;
}
FString UPD_ServerGameInstance::GetServerName() {
	return serverName;
}


TArray<FString> UPD_ServerGameInstance::GetPlayersConnected()
{
	//Esto ahora se conseguiria del playersManager

	TArray<FString> playersConnected = TArray<FString>();
	FString auxNamePlayer = "";


	for (int i = 0; i < playersManager->GetNumPlayers(); i++)
	{
		auxNamePlayer = "Player_0";
		auxNamePlayer.Append(FString::FromInt(i));
		playersConnected.Add(auxNamePlayer);

		
		//UE_LOG(LogTemp, Warning, TEXT(" id character is %s "),*auxNamePlayer);

	}
	

	return playersConnected;
}


TArray<bool> UPD_ServerGameInstance::GetPlayersReady()
{
	TArray<bool> playersReadyArray = TArray<bool>();

	for (int i = 0; i < playersManager->GetNumPlayers(); i++)
	{

		playersReadyArray.Add(playersManager->GetDataStructPlayer(i)->readyMenu);

	}
	//TArray<bool> playersReadyArray = networkManager->GetSocketManager()->GetReadyPlayersArray();

	// Se queda a modo de debug para probar el ADDCHild del Widget
	/*for (int i = 0; i < 4; i++) // Se queda a modo de debug para probar el ADDCHild del Widget
	{
		if (i/2 == 0)
			playersReadyArray.Add(true);
		else
			playersReadyArray.Add(false);

	}
	*/

	return playersReadyArray;
}


void UPD_ServerGameInstance::GetMapsize(float &SizemapX, float &SizemapY)
{
	SizemapX = - ( (((mapX - 1) * 100) / 2) + 1550 );
	SizemapY = ((mapY - 1) * 100) / 2;
}


int UPD_ServerGameInstance::GetServerPhase() {
	return gameManager->getServerPhase();
}

int UPD_ServerGameInstance::GetGameMngrGameState() {
	return gameManager->getGameMngrGameState();
}


/*
FUNCIONES DE CONTROL DE CAMARA
*/

void UPD_ServerGameInstance::Camera_Register(ACameraActor* inCameraServer)
{
	CameraServer = inCameraServer;
}

/*void UPD_ServerGameInstance::CameraRail_Register(ACameraRig_Rail* inCameraRigRail)
{
	CameraRail = inCameraRigRail;
}
*/
TArray<FVector> UPD_ServerGameInstance::getPlayersPositions()
{
	TArray<FVector> desieredPositions = TArray<FVector>();
	for (int i = 0; i < playersManager->GetNumPlayers(); i++)
	{
		desieredPositions.Add(mapManager->LogicToWorldPosition(playersManager->GetDataPlayers()[i]->logic_Character->GetCurrentLogicalPosition()));
	}

	return desieredPositions;
}

TArray<FVector> UPD_ServerGameInstance::getTargetPositions()
{
	return targetPositionsToCenterCamera;
}

TArray<APD_E_ElementActor*> UPD_ServerGameInstance::HiddenActorsBlockPlayers(FVector PositionPlayer)
{
	//reiniciamos todos los muros para que se vean

	for (int i = 0; i < actorsToHide.Num(); i++)
	{
		actorsToHide[i]->SetActorHiddenInGame(false);
	}

	PD_MM_Room* roomWhereWallsExits = nullptr;

	PD_MG_LogicPosition logicWallPosition = mapManager->WorldToLogicPosition(PositionPlayer);

	
	roomWhereWallsExits = mapManager->MapInfo->RoomOf(logicWallPosition);
	
		if (roomWhereWallsExits == nullptr) {
			UE_LOG(LogTemp, Warning, TEXT("fallo al adquir room"));

		}
		else {
			//UE_LOG(LogTemp, Warning, TEXT("HiddenActorsBlockPlayers::num %d"), roomWhereWallsExits->walls.Num());
			for (auto& Elem : roomWhereWallsExits->walls)
			{
				PD_MG_LogicPosition positionToCheck = Elem.Key;
				if (positionToCheck.GetY() == logicWallPosition.GetY())
				{
					//UE_LOG(LogTemp, Warning, TEXT("HiddenActorsBlockPlayers::name wall %s"), *Elem.Value->GetName());
					actorsToHide.Add(Elem.Value);
				}
			}
		}	
		

	return actorsToHide;
}

int  UPD_ServerGameInstance::GetConditionsOfGame()
{
	if (gameManager->CheckWinGameConditions())
		return 2;
	else if (gameManager->CheckLoseGameConditions())
		return 1;
	else
		return 0;

	// 0 - no pasa nada
	// 1 - Jugadores pierden
	// 2 - Jugadores Ganan
}

int UPD_ServerGameInstance::GetConfigMatchMission() {
	return (int)MatchConfigManager->Get_MissionType();
}

int UPD_ServerGameInstance::GetConfigMatchSizeMap() {
	return (int)MatchConfigManager->Get_MapSize();
}

int UPD_ServerGameInstance::GetConfigMatchDifficult() {
	return (int)MatchConfigManager->Get_Difficulty();
}


void UPD_ServerGameInstance::LoadSkillSpecificData(int TypeSkill, int id_skill, FString &nameSkill, FString &effectSkill, int &weaponRequired, int &AP, int &CD, int &target, int &range)
{
	//Create an instance of our savegame class
	UPD_SaveCharacterData* SaveGameInstance = Cast<UPD_SaveCharacterData>(UGameplayStatics::CreateSaveGameObject(UPD_SaveCharacterData::StaticClass()));

	if (UGameplayStatics::DoesSaveGameExist("SkillsData", 0))
	{
		SaveGameInstance = Cast<UPD_SaveCharacterData>(UGameplayStatics::LoadGameFromSlot("SkillsData", 0));
		if (TypeSkill == 0) //ACTIVA
		{
			if (SaveGameInstance->activeSkills.Num() > 0)
			{
				for (int i = 0; i < SaveGameInstance->activeSkills.Num(); i++)
				{
					if (id_skill == SaveGameInstance->activeSkills[i].ID_Skill)
					{
						nameSkill = SaveGameInstance->activeSkills[id_skill].name_Skill;
						effectSkill = SaveGameInstance->activeSkills[id_skill].description;
						weaponRequired = SaveGameInstance->activeSkills[id_skill].weaponRequired;
						AP = SaveGameInstance->activeSkills[id_skill].AP;
						CD = SaveGameInstance->activeSkills[id_skill].CD;
						target = SaveGameInstance->activeSkills[id_skill].target;
						range = SaveGameInstance->activeSkills[id_skill].range;
					}
				}


			}
		}
		else if (TypeSkill == 1)
		{
			if (SaveGameInstance->pasiveSkills.Num() > 0)
			{
				for (int i = 0; i < SaveGameInstance->pasiveSkills.Num(); i++)
				{
					if (id_skill == SaveGameInstance->pasiveSkills[i].ID_Skill)
					{
						nameSkill = SaveGameInstance->pasiveSkills[id_skill].name_Skill;
						effectSkill = SaveGameInstance->pasiveSkills[id_skill].description;
						weaponRequired = SaveGameInstance->pasiveSkills[id_skill].weaponRequired;
						AP = SaveGameInstance->pasiveSkills[id_skill].AP;
						CD = SaveGameInstance->pasiveSkills[id_skill].CD;
						target = SaveGameInstance->pasiveSkills[id_skill].target;
						range = SaveGameInstance->pasiveSkills[id_skill].range;
					}
				}

			}
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("File does not exist."));
	}
}

void UPD_ServerGameInstance::LoadSkillSpecificDataByType(int TypeSkill, int id_skill, FString &nameSkill, FString &effectSkill, int &weaponRequired, int &AP, int &CD, int &target, int &range)
{
	
		if (TypeSkill == 0) //ACTIVA
		{
			if (activeSkills.Num() > 0)
			{
				for (int i = 0; i < activeSkills.Num(); i++)
				{
					if (id_skill == activeSkills[i].ID_Skill)
					{
						nameSkill = activeSkills[id_skill].name_Skill;
						effectSkill = activeSkills[id_skill].description;
						weaponRequired = activeSkills[id_skill].weaponRequired;
						AP = activeSkills[id_skill].AP;
						CD = activeSkills[id_skill].CD;
						target = activeSkills[id_skill].target;
						range = activeSkills[id_skill].range;
					}
				}


			}
		}
		else if (TypeSkill == 1)
		{
			if (pasiveSkills.Num() > 0)
			{
				for (int i = 0; i < pasiveSkills.Num(); i++)
				{
					if (id_skill == pasiveSkills[i].ID_Skill)
					{
						nameSkill = pasiveSkills[id_skill].name_Skill;
						effectSkill = pasiveSkills[id_skill].description;
						weaponRequired = pasiveSkills[id_skill].weaponRequired;
						AP = pasiveSkills[id_skill].AP;
						CD = pasiveSkills[id_skill].CD;
						target = pasiveSkills[id_skill].target;
						range = pasiveSkills[id_skill].range;
					}
				}

			}
		}
		

}

void UPD_ServerGameInstance::LoadWeaponSpecificData(int indexWeapon, int &id_weapon, int &classWeapon, int &typeWeapon, int &damage, int &range)
{
	UPD_SaveCharacterData* SaveGameInstance = Cast<UPD_SaveCharacterData>(UGameplayStatics::CreateSaveGameObject(UPD_SaveCharacterData::StaticClass()));

	if (UGameplayStatics::DoesSaveGameExist("WeaponsData", 0))
	{
		SaveGameInstance = Cast<UPD_SaveCharacterData>(UGameplayStatics::LoadGameFromSlot("WeaponsData", 0));

		if (SaveGameInstance->weapons.Num() > 0)
		{
			for (int i = 0; i < SaveGameInstance->weapons.Num(); i++)
			{
				if (indexWeapon == SaveGameInstance->weapons[i].ID_Weapon)
				{
					id_weapon = indexWeapon;
					classWeapon = SaveGameInstance->weapons[i].ClassWeapon;
					typeWeapon = SaveGameInstance->weapons[i].TypeWeapon;
					damage = SaveGameInstance->weapons[i].DMWeapon;
					range = SaveGameInstance->weapons[i].RangeWeapon;
				}
			}
		}

	}
	else
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("File does not exist."));
}


void UPD_ServerGameInstance::LoadWeaponSpecificDataByType(TypeWeapons typeWeaponID, int &id_weapon, int &classWeapon, int &typeWeapon, int &damage, int &range)
{
	UE_LOG(LogTemp, Log, TEXT("UPD_ServerGameInstance::LoadWeaponSpecificDataByType: Weapon num %d"), weapons.Num());

		if (weapons.Num() > 0)
		{
			for (int i = 0; i < weapons.Num(); i++)
			{
				UE_LOG(LogTemp, Log, TEXT("UPD_ServerGameInstance::LoadWeaponSpecificDataByType: Comparando typeWeaponID %d  con : %d"), (int)typeWeaponID, weapons[i].TypeWeapon);

				if ((int)typeWeaponID == weapons[i].TypeWeapon)
				{
					UE_LOG(LogTemp, Log, TEXT("UPD_ServerGameInstance::LoadWeaponSpecificDataByType: Encontrada arma %d  Rango: %d"), (int)typeWeaponID,weapons[i].RangeWeapon);
					id_weapon = weapons[i].ID_Weapon;
					classWeapon = weapons[i].ClassWeapon;
					typeWeapon = weapons[i].TypeWeapon;
					damage = weapons[i].DMWeapon;
					range = weapons[i].RangeWeapon;
				}
			}
		}

	
}

#pragma endregion

#pragma region LoadDataFromFile

void UPD_ServerGameInstance::LoadSkillActiveDatafromFile()
{
	FString filepath = "Content/Data/ActiveSkills.csv";
	FString FilePath = FPaths::GameDir() + *filepath;
	FString FileData = "";

	if (FFileHelper::LoadFileToString(FileData, *FilePath))
	{
		TArray<FString> raws;
		FileData.ParseIntoArray(raws, TEXT("\n"), true); //Consigo cada linea del CSV (1 habilidad)

		for (int i = 0; i < raws.Num(); i++)
		{
			TArray<FString> columns;
			raws[i].ParseIntoArray(columns, TEXT(";"), true); //Consigo los campos de cada linea

															  //UE_LOG(LogTemp, Warning, TEXT("UPD_ClientGameInstance::LoadSkillActiveDatafromFile::  Fields of Skill :%d "), columns.Num());
			FStructSkill newActiveSkill = FStructSkill();
			for (int j = 0; j < columns.Num(); j++)
			{
				/*
				0 - ID
				1 - Name_ID
				2 - Skill name
				3 - Effect
				4 - Type
				5 - AP
				6 - CD
				7 - Range
				8 - Target
				9 - Weapon Required
				*/
				if (j == 0)
					newActiveSkill.ID_Skill = FCString::Atoi(*columns[j]);
				if (j == 2)
					newActiveSkill.name_Skill = columns[j];
				if (j == 3)
					newActiveSkill.description = columns[j];
				if (j == 4)
					newActiveSkill.typeSkill = FCString::Atoi(*columns[j]);
				if (j == 5)
					newActiveSkill.AP = FCString::Atoi(*columns[j]);
				if (j == 6)
					newActiveSkill.CD = FCString::Atoi(*columns[j]);
				if (j == 7)
					newActiveSkill.range = FCString::Atoi(*columns[j]);
				if (j == 8)
					newActiveSkill.target = FCString::Atoi(*columns[j]);
				if (j == 9)
					newActiveSkill.weaponRequired = FCString::Atoi(*columns[j]);
			}

			newActiveSkill.currentCD = 0;
			activeSkills.Add(newActiveSkill);
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("UPD_ClientGameInstance::LoadSkillActiveDatafromFile::  Error loading Active Skills! Failed to load file!. Path :%s"), *FilePath));
	}

}

void UPD_ServerGameInstance::LoadSkillPasiveDatafromFile()
{
	FString filepath = "Content/Data/PasiveSkills.csv";
	FString FilePath = FPaths::GameDir() + *filepath;
	FString FileData = "";

	if (FFileHelper::LoadFileToString(FileData, *FilePath))
	{
		TArray<FString> raws;
		FileData.ParseIntoArray(raws, TEXT("\n"), true); //Consigo cada linea del CSV (1 habilidad)

		for (int i = 0; i < raws.Num(); i++)
		{
			TArray<FString> columns;
			raws[i].ParseIntoArray(columns, TEXT(";"), true); //Consigo los campos de cada linea

															  //UE_LOG(LogTemp, Warning, TEXT("UPD_ClientGameInstance::LoadSkillActiveDatafromFile::  Fields of Skill :%d "), columns.Num());
			FStructSkill newPasiveSkill = FStructSkill();
			for (int j = 0; j < columns.Num(); j++)
			{
				/*
				0 - ID
				1 - Name_ID
				2 - Skill name
				3 - Effect
				4 - Type
				5 - AP
				6 - CD
				7 - Range
				8 - Target
				9 - Weapon Required
				*/
				if (j == 0)
					newPasiveSkill.ID_Skill = FCString::Atoi(*columns[j]);
				if (j == 2)
					newPasiveSkill.name_Skill = columns[j];
				if (j == 3)
					newPasiveSkill.description = columns[j];
				if (j == 4)
					newPasiveSkill.typeSkill = FCString::Atoi(*columns[j]);
				if (j == 5)
					newPasiveSkill.AP = FCString::Atoi(*columns[j]);
				if (j == 6)
					newPasiveSkill.CD = FCString::Atoi(*columns[j]);
				if (j == 7)
					newPasiveSkill.range = FCString::Atoi(*columns[j]);
				if (j == 8)
					newPasiveSkill.target = FCString::Atoi(*columns[j]);
				if (j == 9)
					newPasiveSkill.weaponRequired = FCString::Atoi(*columns[j]);
			}

			newPasiveSkill.currentCD = 0;
			pasiveSkills.Add(newPasiveSkill);
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("UPD_ClientGameInstance::LoadSkillPasiveDatafromFile::  Error loading Pasive Skills! Failed to load file!. Path :%s"), *FilePath));
	}
}

void UPD_ServerGameInstance::LoadWeaponDataFromFile()
{
	FString filepath = "Content/Data/Weapons.csv";
	FString FilePath = FPaths::GameDir() + *filepath;
	FString FileData = "";

	if (FFileHelper::LoadFileToString(FileData, *FilePath))
	{
		TArray<FString> raws;
		FileData.ParseIntoArray(raws, TEXT("\n"), true); //Consigo cada linea del CSV (1 arma)

		for (int i = 0; i < raws.Num(); i++)
		{
			TArray<FString> columns;
			raws[i].ParseIntoArray(columns, TEXT(";"), true); //Consigo los campos de cada linea

															  //UE_LOG(LogTemp, Warning, TEXT("UPD_ClientGameInstance::LoadWeaponDataFromFile::  Fields of weapon :%d "), columns.Num());
			FStructWeapon newWeapon = FStructWeapon();
			for (int j = 0; j < columns.Num(); j++)
			{
				/*
				0 - ID
				1 - Name_ID
				2 - Type
				3 - Class
				4 - DM
				5 - Rango
				*/
				if (j == 0)
					newWeapon.ID_Weapon = FCString::Atoi(*columns[j]);
				if (j == 2)
					newWeapon.TypeWeapon = FCString::Atoi(*columns[j]);
				if (j == 3)
					newWeapon.ClassWeapon = FCString::Atoi(*columns[j]);
				if (j == 4)
					newWeapon.DMWeapon = FCString::Atoi(*columns[j]);
				if (j == 5)
					newWeapon.RangeWeapon = FCString::Atoi(*columns[j]);
			}

			weapons.Add(newWeapon);
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("UPD_ClientGameInstance::LoadSkillActiveDatafromFile::  Error loading Active Skills! Failed to load file!. Path :%s"), *FilePath));
	}
}


#pragma endregion

#pragma region Functions for GameManager
void UPD_ServerGameInstance::GameManagerFunction_PlayAnimationHDHOnCharacter()
{
	gameManager->PlayAnimationOnCharacters_HurtDefenseHeal();
}

int UPD_ServerGameInstance::GetVictoryOrLostGameState()
{
	return gameManager->MatchIsWinOrLost;
}

#pragma endregion

#pragma region Functions of Podium
int UPD_ServerGameInstance::GetNumberOfPlayers()
{
	return playersManager->GetNumPlayers();
}

void UPD_ServerGameInstance::GetInfoPlayerForPodium(TArray<FString> &NamePlayer, TArray<int> &Score, TArray<int> &id_Skin)
{
	int MaxScore = 0;
	TArray<int> out_Scores = TArray<int>();
	TArray<int> out_Scores_sorted = TArray<int>();

	if (playersManager->GetNumPlayers() > 0)
	{
		for (int index_player = 0; index_player < playersManager->GetNumPlayers(); index_player++)
		{
			out_Scores.Add(playersManager->GetDataStructPlayer(index_player)->logic_Character->GetTotalStats()->PointsCurrent);
		}

		out_Scores_sorted = out_Scores;
		out_Scores_sorted.Sort(); //de menor a mayor

		//Get and ADD the WINNER
		if (playersManager->GetNumPlayers() == 1) //solo 1 jugador
		{
			NamePlayer.Add(playersManager->GetDataStructPlayer(0)->logic_Character->GetIDCharacter());
			Score.Add(out_Scores_sorted[0]);
			id_Skin.Add(playersManager->GetDataStructPlayer(0)->logic_Character->GetSkin()->ID_SkinHead);
		}
		else 
		{
			for (int i = 0; i < out_Scores.Num(); i++)
			{
				int ScoreMax_Winner = out_Scores_sorted.Pop();
				int index_winner = 0;
				if (out_Scores.Find(ScoreMax_Winner, index_winner))
				{
					NamePlayer.Add(playersManager->GetDataStructPlayer(index_winner)->logic_Character->GetIDCharacter());
					Score.Add(ScoreMax_Winner);
					id_Skin.Add(playersManager->GetDataStructPlayer(index_winner)->logic_Character->GetSkin()->ID_SkinHead);
				}
			}
		}
	}
	
}

#pragma endregion

void UPD_ServerGameInstance::ResetApplication()
{
	UE_LOG(LogTemp, Log, TEXT("UPD_ServerGameInstance::ResetApplication - Reiniciando APP ... "));
	UE_LOG(LogTemp, Log, TEXT("UPD_ServerGameInstance::ResetApplication - Borrando networking ... "));

	
	///delete network - delete Sockets
	if (networkManager) {
		delete networkManager;
	}

	UE_LOG(LogTemp, Log, TEXT("UPD_ServerGameInstance::ResetApplication - Inicializando variables SGI ... "));

	///Inizaliza the same as UPD_ServerGameInstance::Init()
	UE_LOG(LogTemp, Warning, TEXT("Init GameInstance ~> "));
	//Inicializar Arrays de Skills and Weapons
	LoadSkillActiveDatafromFile();
	LoadSkillPasiveDatafromFile();
	LoadWeaponDataFromFile();
	levelsNameDictionary = LevelsNameDictionary();
	playersManager = new PD_PlayersManager();
	structServerState = new StructServerState();
	mapGenerator = new PD_MG_MapGenerator();
	//en el inicialize networking seteamos el gameinstance como observador.
	InitializeNetworking();
	MatchConfigManager = new PD_MatchConfigManager(this);

	ChangeState(EServerState::StartApp);

	UE_LOG(LogTemp, Log, TEXT("UPD_ServerGameInstance::ResetApplication - Cambiando de mapa a INICIAL ... "));
	///Pasar al primer nivel
	LoadMap(levelsNameDictionary.GetMapName(1));
	
}

void UPD_ServerGameInstance::OnTimerPodiumEnds()
{
	ResetApplication();
}

void UPD_ServerGameInstance::RegisterBackgroundMusic(AAmbientSound* newMusic, bool newSong)
{
	backgroundMusic = newMusic;
	
	if (newSong)
		backgroundMusic->Play();
	else
		backgroundMusic->Play(timeCurrentMusic);

	timeStartMusicBackground = UGameplayStatics::GetUnpausedTimeSeconds(GetWorld());
		//backgroundMusic->Play();
}

AAmbientSound*  UPD_ServerGameInstance::GetBackgroundMusic()
{
	return backgroundMusic;
}