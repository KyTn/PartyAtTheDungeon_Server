// Fill out your copyright notice in the Description page of Project Settings.

#include "PATD_Server.h"
#include "Networking.h"
#include "PD_ServerGameInstance.h"

#include "NW_NetWorking/PD_NW_TimerActor.h"
#include "NW_NetWorking/PD_NW_NetworkManager.h"
//Includes de uso
#include "MapGeneration/PD_MG_StaticMap.h"
#include "MapGeneration/PD_MG_DynamicMap.h"

//Includes of forward declaration
#include "Structs/PD_ServerStructs.h" //Para todos los structs y enums
#include "NW_NetWorking/Socket/PD_NW_SocketManager.h"
#include "MapGeneration/ParserActor.h"
#include "PD_PlayersManager.h"
#include "MapGeneration/PD_MG_MapParser.h"
#include "GM_Game/PD_GM_MapManager.h"
#include "GM_Game/PD_GM_GameManager.h"

//Includes de prueba



//Esquema
/*
el clientMaster se conecta al servidor
Los clientes se meten ahora.
el clientMaster configura y da ok.
les sale a todos para elegir su personaje.
Aqui no se puede conectar nadie mas.
Lo crean y le dan a ready.
*/

void UPD_ServerGameInstance::HandleEvent(FStructGeneric* inDataStruct, int inPlayer, UStructType inEventType) {
	if (structServerState->enumServerState == EServerState::WaitingClientMaster) {
		//Evento NewConnection
		if (inEventType == UStructType::FStructNewConnection) {
			//Registrar en playersManager
			playersManager->AddNewPlayer((FStructNewConnection*)inDataStruct, inPlayer);
			StructPlayer* structPlayer = playersManager->getDataStructPlayer(inPlayer);
			structPlayer->clientMaster = true; //Es el ClientMaster
			//Contestar al cliente
			FStructOrderMenu clientResponse;
			clientResponse.orderType = static_cast<uint8>(MenuOrderType::Welcome);
			clientResponse.playerIndex = inPlayer;
			clientResponse.isClientMaster = true;
			networkManager->SendNow(&clientResponse, inPlayer);

			this->UpdateState();//Cambio de estado
		}
	}else if (structServerState->enumServerState == EServerState::WaitingGameConfiguration) {
		//Evento NewConnection
		if (inEventType == UStructType::FStructNewConnection) {
			//Registrar en playersManager
			playersManager->AddNewPlayer((FStructNewConnection*)inDataStruct, inPlayer);
			//Contestar al cliente
			FStructOrderMenu clientResponse;
			clientResponse.orderType = static_cast<uint8>(MenuOrderType::Welcome);
			clientResponse.playerIndex = inPlayer;
			networkManager->SendNow(&clientResponse, inPlayer);
		//Evento OrderMenu
		}else if (inEventType == UStructType::FStructOrderMenu) {
			FStructOrderMenu* menuOrder = (FStructOrderMenu*)inDataStruct;
			if (MenuOrderType(menuOrder->orderType) == MenuOrderType::GameConfigurationDone) {
				//Solo si lo envia el clientMaster
				if (inPlayer == playersManager->getIndexClientMaster()) {
					structServerState->gameConfigurationDone = true;
					this->UpdateState();//Cambio de estado
				}
			}
		}
	}else if (structServerState->enumServerState == EServerState::WaitingReady) {
		//Evento OrderMenu
		if (inEventType == UStructType::FStructOrderMenu) {
			FStructOrderMenu* menuOrder = (FStructOrderMenu*)inDataStruct;
			if (MenuOrderType(menuOrder->orderType) == MenuOrderType::ClientReady) {
				playersManager->getDataStructPlayer(inPlayer)->readyMenu = !playersManager->getDataStructPlayer(inPlayer)->readyMenu;
				this->UpdateState();//Cambio de estado
			}
		//Evento PlayerData
		}else if (inEventType == UStructType::FStructOrderMenu) {

		}
	}
}

void UPD_ServerGameInstance::UpdateState() {
	if (structServerState->enumServerState == EServerState::WaitingClientMaster){
		//Transiciones de estados
		if (playersManager->getIndexClientMaster() != -1) {
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

		mapParser = new PD_MG_MapParser();

		PD_MG_StaticMap* staticMapRef = new PD_MG_StaticMap();
		PD_MG_DynamicMap* dynamicMapRef = new PD_MG_DynamicMap();
		///Haria falta una clase Enemy manager, con los enemigos para organizar sus acciones, y llamar a las AI correspondientes
		mapParser->StartParsingFromFile(&mapPath, staticMapRef, dynamicMapRef);
		mapManager = new PD_GM_MapManager();
		mapManager->StaticMapRef = staticMapRef;
		mapManager->DynamicMapRef = dynamicMapRef;
		
		//Esto quiza no deberia guardar el string ya ahi.
		FString mapString = mapManager->StaticMapRef->GetMapString();
		//Enviar mapa al cliente
		FStructMap structMap;
		structMap.stringMap = mapString;
		networkManager->SendNow(&structMap, -1);

		this->LoadMap(levelsNameDictionary.GetMapName(4));//Mapa de juego
	}
	else //Caso indeterminado
	{
		//UE_LOG(LogTemp, Warning, TEXT("PD_GM_GameManager::OnBeginState: WARNING: estado sin inicializacion"));
	}

}


void UPD_ServerGameInstance::ChangeState(EServerState newState) {
	structServerState->enumServerState = newState;
	OnBeginState();
}
	

bool UPD_ServerGameInstance::SuscribeToEvents(int inPlayer, UStructType inType) {
	
	return true; //de momento recibe todos, siempre es cierto.
}


void UPD_ServerGameInstance::LoadMap(FString mapName)
{
	UGameplayStatics::OpenLevel((UObject*)this, FName(*mapName));


}

//Callback cuando el mapa este cargado
void UPD_ServerGameInstance::OnMapFinishLoad() {
	//Sin importar el estado hacemos:
	APD_NW_TimerActor* TimerActorSpawned = (APD_NW_TimerActor*)GetWorld()->SpawnActor(APD_NW_TimerActor::StaticClass());
	networkManager->GetSocketManager()->InitTimerActor(TimerActorSpawned);

	if (structServerState->enumServerState == EServerState::GameInProcess) {
		//Quizas esto es tarea del gameManager.
	///Esto se descomenta de aqui, y se comenta arriba para probar parte de instanciar cosas en el mapa
	/*mapParser = new PD_MG_MapParser();

	PD_MG_StaticMap* staticMapRef = new PD_MG_StaticMap();
	PD_MG_DynamicMap* dynamicMapRef = new PD_MG_DynamicMap();
	///Haria falta una clase Enemy manager, con los enemigos para organizar sus acciones, y llamar a las AI correspondientes
	mapParser->StartParsingFromFile(&mapPath, staticMapRef, dynamicMapRef);
	mapManager = new PD_GM_MapManager();
	mapManager->StaticMapRef = staticMapRef;
	mapManager->DynamicMapRef = dynamicMapRef;*/
		parserActor = (AParserActor*)GetWorld()->SpawnActor(AParserActor::StaticClass());
		mapParser->InstantiateStaticMap(parserActor,mapManager->StaticMapRef);
		mapParser->InstantiateDynamicMap(parserActor, mapManager->DynamicMapRef);
		//Aqui cedemos el control al GameManager.
		gameManager = new PD_GM_GameManager(playersManager,mapManager);
		networkManager->RegisterObserver(gameManager);

	}
}



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

void UPD_ServerGameInstance::Shutdown()
{
	Super::Shutdown();

	if (networkManager) {
	delete networkManager;
	}
	//Aqui habria que hacer muchos deletes 
	
}

void UPD_ServerGameInstance::InitializeNetworking()
{	
	
	InitializeServerAddress();

	networkManager= new PD_NW_NetworkManager();
	PD_NW_SocketManager* socketManager = networkManager->GetSocketManager();
	
	socketManager->SetIsServer(true);

	APD_NW_TimerActor* ServerActorSpawned = (APD_NW_TimerActor*)GetWorld()->SpawnActor(APD_NW_TimerActor::StaticClass());
	socketManager->SetNetworkManager(networkManager);
	
	socketManager->Init(ServerActorSpawned, serverIP, defaultServerPort);//Con esto empezaria el timer
	

	//Seteamos este gameinstance como observador de eventos en el networkmanager.
	networkManager->RegisterObserver(this);

}


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













/*************************************
******* FUNCIONES UTILIDADES / BP
*************************************/
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
	//Esto ahora se conseguiria del ServerManager

	TArray<FString> playersConnected = TArray<FString>();
	FString auxNamePlayer = "";
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

