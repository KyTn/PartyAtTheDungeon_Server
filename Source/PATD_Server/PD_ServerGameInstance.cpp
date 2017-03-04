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

//Includes of forward declaration
#include "Structs/PD_ServerStructs.h" //Para todos los structs y enums
#include "NW_Networking/PD_NW_NetworkManager.h"
#include "PD_PlayersManager.h"
#include "MapGeneration/PD_MG_MapParser.h"
#include "MapInfo/MapInstantiation/MapInstantiatorActor.h"
#include "GM_Game/PD_GM_MapManager.h"
#include "GM_Game/PD_GM_GameManager.h"
#include "GM_Game/PD_GM_EnemyManager.h"
//Includes de prueba



#pragma region Get Managers
//Funciones para obtener managers
PD_GM_GameManager* UPD_ServerGameInstance::getGameManager() { return gameManager; }
#pragma endregion 

#pragma region Suscribe to events

bool UPD_ServerGameInstance::SuscribeToEvents(int inPlayer, UStructType inType) {

	return true; //de momento recibe todos, siempre es cierto.
}

#pragma endregion 


#pragma region HandleEvent Functions 

void UPD_ServerGameInstance::HandleEvent(FStructGeneric* inDataStruct, int inPlayer, UStructType inEventType) {
	UE_LOG(LogTemp, Warning, TEXT("ServerGameInstance::HandleEvent:: Evento recibido:%d Estado del servidor: %d"), static_cast<uint8>(inEventType), static_cast<uint8>(structServerState->enumServerState));


	if (structServerState->enumServerState == EServerState::StartApp) {
		// Ningún mensaje puede llegar durante este estado
	}
	else if(structServerState->enumServerState == EServerState::WaitingMasterClient) {
		//Evento NewConnection
		if (inEventType == UStructType::FStructNewConnection) {

			// registra un jugador entrante como jugador MASTER
			HandleEvent_NewConnection(inDataStruct, inPlayer, inEventType, true);
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("UPD_ServerGameInstance::HandleEvent:: WaitingMasterClient - NOT MATCHING "));
		}
	}	 
	else if(structServerState->enumServerState == EServerState::WaitingGameConfiguration) {

		//Evento NewConnection
		if (inEventType == UStructType::FStructNewConnection) {

			// registra un jugador entrante como jugador normal
			HandleEvent_NewConnection(inDataStruct, inPlayer, inEventType, false);
		}
		else if (inEventType == UStructType::FStructMatchConfig) {

			// Cuando el client Master envía la congif de la partida ... 
			HandleEvent_ConfigMatch(inDataStruct, inPlayer, inEventType);
		}
		else if (inEventType == UStructType::FStructMatchConfigDone) {

			// Cuando el client Master envía la congif de la partida ... 
			HandleEvent_ConfigMatchDone(inDataStruct, inPlayer, inEventType);
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("UPD_ServerGameInstance::HandleEvent:: WaitingGameConfiguration - NOT MATCHING "));
		}

	}	 
	else if(structServerState->enumServerState == EServerState::Lobby_Tabern) {
		//Evento OrderMenu
		if (inEventType == UStructType::FStructOrderMenu) {

			// Cuando un jugador envía READY, marcarlo como preparado. 
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
///		int inPlayer					Index del jugador que envía
///		UStructType inEventType			Tipo de estructura
///		bool isMasterClient				registra como masterclient o no. Default: false.
///
void UPD_ServerGameInstance::HandleEvent_NewConnection(FStructGeneric* inDataStruct, int inPlayer, UStructType inEventType, bool isMasterClient = false) {
	UE_LOG(LogTemp, Warning, TEXT("ServerGameInstance::HandleEvent_NewConnection"));
	//Registrar en playersManager
	playersManager->AddNewPlayer((FStructNewConnection*)inDataStruct, inPlayer);

	if (isMasterClient) {
		StructPlayer* structPlayer = playersManager->GetDataStructPlayer(inPlayer);
		structPlayer->clientMaster = true; //Es el ClientMaster
										   //Contestar al cliente
	}

	//Enviamos al cliente el success 
	FStructOrderMenu clientResponse;
	clientResponse.orderType = static_cast<uint8>(MenuOrderType::Welcome);
	clientResponse.playerIndex = inPlayer;
	clientResponse.isClientMaster = isMasterClient;
	networkManager->SendNow(&clientResponse, inPlayer);

	//Si es el MasterClient, es que estamos en WaitingMasterClient y cambiamos de estado al WaitingGameConfiguration
	if(isMasterClient) this->UpdateState(); 

}

/// Cuando el MasterClient cambia un valor de la configuración del match ...
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

	// Obtener la información y actualizar la configuración del match en el servidor:
	// StructMatchConfig.id		: id del campo a cambiar
	// StructMatchConfig.value	: nuevo valor del campo id

	// Enviar la información confirmada a los clientes ... 
}

/// Cuando el MasterClient termina de configurar el Match ... 
void UPD_ServerGameInstance::HandleEvent_ConfigMatchDone(FStructGeneric* inDataStruct, int inPlayer, UStructType inEventType) {
	UE_LOG(LogTemp, Warning, TEXT("ServerGameInstance::HandleEvent_ConfigMatchDone"));
	if (inPlayer == playersManager->GetIndexClientMaster()) {
		structServerState->gameConfigurationDone = true;

		// mandar el MatchConfigDone a todos los clientes, masterclient incluido, como confirmación.
		FStructMatchConfigDone configDone = FStructMatchConfigDone();
		configDone.from = 0;
		networkManager->SendNow(&configDone, -1);

		this->UpdateState();//Cambio de estado
	}


}



void UPD_ServerGameInstance::HandleEvent_LoadPlayerInfo(FStructGeneric* inDataStruct, int inPlayer) {
	UE_LOG(LogTemp, Warning, TEXT("ServerGameInstance::HandleEvent_LoadPlayerInfo"));
	FStructCharacter* playerStats = (FStructCharacter*)inDataStruct;

	playersManager->GetDataStructPlayer(inPlayer)->logic_Character = new PD_GM_LogicCharacter();

	playersManager->GetDataStructPlayer(inPlayer)->logic_Character->totalStats = &playerStats->totalStats;
	playersManager->GetDataStructPlayer(inPlayer)->logic_Character->basicStats = &playerStats->basicStats;
	playersManager->GetDataStructPlayer(inPlayer)->logic_Character->initBaseStats = &playerStats->initBaseStats;
	playersManager->GetDataStructPlayer(inPlayer)->logic_Character->skills = &playerStats->skills;
	playersManager->GetDataStructPlayer(inPlayer)->logic_Character->weapon = &playerStats->weapon;
	playersManager->GetDataStructPlayer(inPlayer)->logic_Character->skin = &playerStats->skin;
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
		// Si todos los jugadores están ready
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

		//Aqui haríamos una espera para la pantalla de inicio, animación inicial y tal ... 

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
		this->LoadMap(levelsNameDictionary.GetMapName(3));

	}
	else if (structServerState->enumServerState == EServerState::Launch_Match) {
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

		//CODIGO PROVISIONAL PARA ESPERAR. HAY QUE ARREGLAR EL NETMANAGER
		int i = 0;
		float f = 1;
		while (i < 10000) {
			UE_LOG(LogTemp, Warning, TEXT("wait cutrisimo %d"), i);
			i++;
			f = f*f;
		}
		//FIN CODIGO PROVISIONAL

		//Broadcast del listInstantiatePlayers
		networkManager->SendNow(&listInstantiatePlayers, -1);
		ChangeState(EServerState::GameInProcess);
		
	}
	else if (structServerState->enumServerState == EServerState::GameInProcess) {
		
		this->LoadMap(levelsNameDictionary.GetMapName(4));//Mapa de juego
	}
	else if (structServerState->enumServerState == EServerState::Podium) {

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
		
		// Mientras están en el lobby, se genera el mapa y se envía a los jugadores (cuando respondan que están en el lobby). 

		mapParser = new PD_MG_MapParser();

		PD_MG_StaticMap* staticMapRef = new PD_MG_StaticMap();
		PD_MG_DynamicMap* dynamicMapRef = new PD_MG_DynamicMap();

		// Parsea el chorizo
		mapParser->StartParsingFromFile(&mapPath, staticMapRef, dynamicMapRef);





		// GENERAR EL USTRUC / Static&Dynamic Map




		mapManager = new PD_GM_MapManager();
		mapManager->Init(staticMapRef, dynamicMapRef); // inicializa las estructuras internas del mapManager (MapInfo)
	
													   

		/*  */
		FString mapString = staticMapRef->GetMapString();
		//Enviar mapa al cliente
		FStructMap structMap;
		structMap.stringMap = mapString;
		networkManager->SendNow(&structMap, -1);
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
		gameManager = new PD_GM_GameManager(playersManager, mapManager, networkManager);


	}
	else if (structServerState->enumServerState == EServerState::Podium) {

	}
	else if (structServerState->enumServerState == EServerState::OnExit) {

	}
	else
	{

	}
}

#pragma endregion

#pragma region INITS

// Inicializa el GameInstance
void UPD_ServerGameInstance::Init()
{
	Super::Init();
	UE_LOG(LogTemp, Warning, TEXT("Init GameInstance ~> "));
	levelsNameDictionary = LevelsNameDictionary(); 

	playersManager = new PD_PlayersManager();

	structServerState = new StructServerState();

	ChangeState(EServerState::StartApp);
	//structServerState->enumServerState = EServerState::StartApp;
	

	//en el inicialize networking seteamos el gameinstance como observador.
	InitializeNetworking();

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

	socketManager->Init(ServerActorSpawned, serverIP, defaultServerPort);//Con esto empezaria el timer


																		 //Seteamos este gameinstance como observador de eventos en el networkmanager.
	networkManager->RegisterObserver(this);

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
		
		for (int i = 0; i < 1; i++)///Hay que recorrerlo más adelante mirando todas
		{
			auxmyIP = myIPAddress[i]->ToString(false);
			myIP.Append(auxmyIP);
		}
		//UE_LOG(LogTemp, Warning, TEXT("My ip %d"), myIP.Len());
		serverIP = myIP;
		serverName = myServerName;
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
		auxNamePlayer = "Player ";
		auxNamePlayer.Append(FString::FromInt(i));
		playersConnected.Add(auxNamePlayer);
	}
	/*
	TArray<PD_NW_Socket*> socketArray = networkManager->GetSocketManager()->GetSocketArray();

	for (int i = 0; i < socketArray.Num(); i++)
	{
		auxNamePlayer = "Player ";
		auxNamePlayer.Append(FString::FromInt(i));
		playersConnected.Add(auxNamePlayer);
	}
	*/

	// Se queda a modo de debug para probar el ADDCHild del Widget
	/*for (int i = 0; i < 12; i++) 
	{
		auxNamePlayer = "Player ";
		auxNamePlayer.Append(FString::FromInt(i));
		playersConnected.Add(auxNamePlayer);
	}
	*/

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



#pragma endregion







