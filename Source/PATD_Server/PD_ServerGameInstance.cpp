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

	if (structServerState->enumServerState == EServerState::WaitingClientMaster) {
		//Evento NewConnection
		if (inEventType == UStructType::FStructNewConnection) {

			// registra un jugador entrante como jugador MASTER
			HandleEvent_NewConnection(inDataStruct, inPlayer, inEventType, true);
		}
	}else if (structServerState->enumServerState == EServerState::WaitingGameConfiguration) {

		//Evento NewConnection
		if (inEventType == UStructType::FStructNewConnection) {

			// registra un jugador entrante como jugador normal
			HandleEvent_NewConnection(inDataStruct, inPlayer, inEventType, false);

		//Evento OrderMenu
		}else if (inEventType == UStructType::FStructOrderMenu) {

			// Cuando el client Master envía la congif de la partida ... 
			HandleEvent_ConfigMatch(inDataStruct, inPlayer, inEventType);


			//FStructOrderMenu* menuOrder = (FStructOrderMenu*)inDataStruct;
			//if (MenuOrderType(menuOrder->orderType) == MenuOrderType::GameConfigurationDone) {
			//	//Solo si lo envia el clientMaster
			//	if (inPlayer == playersManager->getIndexClientMaster()) {
			//		structServerState->gameConfigurationDone = true;
			//		this->UpdateState();//Cambio de estado
			//	}
			//}



		}
	}else if (structServerState->enumServerState == EServerState::WaitingReady) {
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
	}
}



// Registra un jugador entrante. 
// PARAMS: 
//		FStructGeneric* inDataStruct	Estructura que recibe del NetManager
//		int inPlayer					Index del jugador que envía
//		UStructType inEventType			Tipo de estructura
//		bool isMasterClient				registra como masterclient o no. Default: false.
//
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

	if(isMasterClient) this->UpdateState(); //Cambio de estado al WaitingGameConfiguration

}

void UPD_ServerGameInstance::HandleEvent_ConfigMatch(FStructGeneric* inDataStruct, int inPlayer, UStructType inEventType) {
	UE_LOG(LogTemp, Warning, TEXT("ServerGameInstance::HandleEvent_ConfigMatch"));
	FStructOrderMenu* menuOrder = (FStructOrderMenu*)inDataStruct;
	if (MenuOrderType(menuOrder->orderType) == MenuOrderType::GameConfigurationDone) {
		//Solo si lo envia el clientMaster
		if (inPlayer == playersManager->GetIndexClientMaster()) {
			structServerState->gameConfigurationDone = true;
			this->UpdateState();//Cambio de estado
		}
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

#pragma region ONBEGIN & UPDATE STATES

void UPD_ServerGameInstance::UpdateState() {
	if (structServerState->enumServerState == EServerState::WaitingClientMaster){
		//Transiciones de estados
		if (playersManager->GetIndexClientMaster() != -1) {
			this->ChangeState(EServerState::WaitingGameConfiguration);
		}
	}else  if (structServerState->enumServerState == EServerState::WaitingGameConfiguration) {
		//Transiciones de estados
		if (structServerState->gameConfigurationDone) {
			this->ChangeState(EServerState::WaitingReady);
		}
	}else if (structServerState->enumServerState == EServerState::WaitingReady) {
		if (playersManager->AllPlayersReady()) {
			this->ChangeState(EServerState::GameInProcess);
		}
	}

}

void UPD_ServerGameInstance::ChangeState(EServerState newState) {
	structServerState->enumServerState = newState;
	OnBeginState();
}

void UPD_ServerGameInstance::OnBeginState() {
	if (structServerState->enumServerState == EServerState::WaitingClientMaster) {
		
	}else  if (structServerState->enumServerState == EServerState::WaitingGameConfiguration) {
				
		this->LoadMap(levelsNameDictionary.GetMapName(2));
		
	}else if (structServerState->enumServerState == EServerState::WaitingReady) {
		FStructOrderMenu clientBroadcast;
		clientBroadcast.orderType = static_cast<uint8>(MenuOrderType::ChangeToLobby);
		networkManager->SendNow(&clientBroadcast, -1);

		this->LoadMap(levelsNameDictionary.GetMapName(3));

	}
	else if (structServerState->enumServerState == EServerState::GameInProcess){

		this->LoadMap(levelsNameDictionary.GetMapName(4));//Mapa de juego
	}
	else //Caso indeterminado
	{
		//UE_LOG(LogTemp, Warning, TEXT("PD_GM_GameManager::OnBeginState: WARNING: estado sin inicializacion"));
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

	if (structServerState->enumServerState == EServerState::WaitingReady) 
	{

	}

	if (structServerState->enumServerState == EServerState::GameInProcess) {
			//Quizas esto es tarea del gameManager.
		///Esto se descomenta de aqui, y se comenta arriba para probar parte de instanciar cosas en el mapa
		mapParser = new PD_MG_MapParser();

		PD_MG_StaticMap* staticMapRef = new PD_MG_StaticMap();
		PD_MG_DynamicMap* dynamicMapRef = new PD_MG_DynamicMap();
	
		// Parsea el chorizo
		mapParser->StartParsingFromFile(&mapPath, staticMapRef, dynamicMapRef);
		mapManager = new PD_GM_MapManager();
		mapManager->StaticMapRef = staticMapRef;
		mapManager->DynamicMapRef = dynamicMapRef;


		FString mapString = staticMapRef->GetMapString();
		//Enviar mapa al cliente
		FStructMap structMap;
		structMap.stringMap = mapString;
		networkManager->SendNow(&structMap, -1);

		// le pasamos al mapManager un instanciador
		AMapInstantiatorActor* InstantiatorActor = (AMapInstantiatorActor*)GetWorld()->SpawnActor(AMapInstantiatorActor::StaticClass());
		mapManager->instantiator = InstantiatorActor;

		// le decimos al mapManager que instancia las cosicas en el mundo
		//mapManager->InstantiateStaticMap();
		//mapManager->InstantiateDynamicMap();

		//Aqui cedemos el control al GameManager.
		gameManager = new PD_GM_GameManager(playersManager,mapManager);
		networkManager->RegisterObserver(gameManager);

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
	structServerState->enumServerState = EServerState::WaitingClientMaster;

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







