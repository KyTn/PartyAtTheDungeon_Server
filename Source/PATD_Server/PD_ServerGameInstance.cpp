// Fill out your copyright notice in the Description page of Project Settings.

#include "PATD_Server.h"
#include "Networking.h"
#include "PD_ServerGameInstance.h"

#include "NW_NetWorking/PD_NW_TimerActor.h"
#include "NW_NetWorking/PD_NW_NetworkManager.h"

//Includes of forward declaration
#include "NW_NetWorking/Socket/PD_NW_SocketManager.h"
#include "MapGeneration/PD_MG_Map.h"

//Includes de prueba
#include "NW_Networking/EventLayer/PD_NW_iEventObserver.h"
#include "MapGeneration/PD_MG_StaticMap.h"


void UPD_ServerGameInstance::Init()
{
	Super::Init();
	UE_LOG(LogTemp, Warning, TEXT("Init GameInstance ~> "));
	InitializeNetworking();

	class ObservadorPrueba : public PD_NW_iEventObserver
	{
	public:
		UPD_ServerGameInstance *gi;
		ObservadorPrueba(UPD_ServerGameInstance* i) {
			gi = i;
		}
		void handleEvent(FStructGenericoHito2* dataStruct, int inPlayer, UStructType inEventType) {
			UE_LOG(LogTemp, Warning, TEXT("Recibido una MenuOrder "));

			if (dataStruct->orderType != 255) { //NullOrder
				FStructGenericoHito2 respuesta =  FStructGenericoHito2();
				switch (dataStruct->orderType) {
				case 0: //New connection
					if (gi->GetWorld()->GetMapName() != "UEDPIE_0_LVL_4_GameMap") {
						if (gi->clientMasterIndex == -1) {//No hay clientMaster
							gi->clientMasterIndex = inPlayer;
							respuesta.orderType = 5;//SetClientMaster
							respuesta.stringMap.AppendInt(inPlayer);
						}
						else {//Hay clientMaster
							respuesta.orderType = 6;//Welcome
							respuesta.stringMap.AppendInt(inPlayer);
						}
					}
					else {
						respuesta.orderType = 10;//InvalidConnection
						//Deberiamos quitar la conexion del manager o ver como gestionar esto mas adelante.
					}
					gi->networkManager->SendNow(&respuesta, inPlayer);
					if (gi->GetWorld()->GetMapName() == "LVL_2_MainMenu")
						respuesta.orderType = 7;//Welcome
					gi->networkManager->SendNow(&respuesta, inPlayer);
					if (gi->GetWorld()->GetMapName() == "LVL_3_SelectChars_Lobby")
						respuesta.orderType = 8;//Welcome
					gi->networkManager->SendNow(&respuesta, inPlayer);
					break;

				case 1://GoToMainMenu
					gi->LoadMap("LVL_2_MainMenu");
					respuesta.orderType = 7; //ChangeToMainMenu
					gi->networkManager->SendNow(&respuesta, -1);
					break;
				case 2://GoToLobby
					gi->LoadMap("LVL_3_SelectChars_Lobby");
					respuesta.orderType = 8; //ChangeToLobby
					gi->networkManager->SendNow(&respuesta, -1);
					break;
				case 3://GoToMap
					//IniciarArray de readys con el numero de jugadores actual
					//Si uno se cae en este punto no se podra iniciar partida jamas xD

					break;
				case 4://ClientReady
					//Setear true en array de readys
					//Comprobar si todos son trues.

					/*if (gi->ready[inPlayer].Contains(false))
					{
						gi->ready->Insert(true, inPlayer);
						bool allReady = true;
						for (size_t i = 0; i < gi->ready->Num() && allReady; i++)
						{
							if (gi->ready[i].Contains(false))
								allReady = false;
						}
						if (allReady)
						{
							gi->LoadMap("LVL_4_GameMap");
							respuesta.orderType = 9; //ChangeToMap
							gi->networkManager->SendNow(&respuesta, -1);
							gi->sendMap();
						}
					}
					else//si ya habia pulsado reay antes se pone a falso
						gi->ready->Insert(false, inPlayer);
					*/
					gi->SetClientReady(inPlayer);

					if (gi->CheckForAllClientReady())
					{
						//Carga nuevo mapa + Envio de Mapa a CLIENTES
						gi->LoadMap("LVL_4_GameMap");
						//gi->InitGameMap();

					//Llamamos a enviar el mensaje de respuesta cuando ya este cargado nuestro mapa en InitGameMap();
						
						
					}
					break;
				case 11:
					respuesta.orderType = 9; //ChangeToLobby
					gi->networkManager->SendNow(&respuesta, -1);
					break;
				}

			}
		}
	};
	ObservadorPrueba* obs = new ObservadorPrueba(this);
	obs->setUpObserver(-1, UStructType::AllStructs);
	networkManager->RegisterObserver(obs);



}

void UPD_ServerGameInstance::Shutdown()
{
	Super::Shutdown();

	if (networkManager) {
	delete networkManager;
	}
	/*if (socketManager) {
		delete socketManager;
	}*/
}

void UPD_ServerGameInstance::InitializeNetworking()
{	
	//FString myip;
	InitializeServerAddress();

	networkManager= new PD_NW_NetworkManager();
	PD_NW_SocketManager* socketManager = networkManager->GetSocketManager();
	//socketManager = new PD_NW_SocketManager();
	socketManager->SetIsServer(true);

	APD_NW_TimerActor* ServerActorSpawned = (APD_NW_TimerActor*)GetWorld()->SpawnActor(APD_NW_TimerActor::StaticClass());
	socketManager->SetNetworkManager(networkManager);
	//Donde se pone el puerto que es como una constante global?
	socketManager->Init(ServerActorSpawned, serverIP, defaultServerPort);//Con esto empezaria el timer, quizas no lo queremos llamar aqui o queremos separarlo entre init y start
	
	
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


void UPD_ServerGameInstance::LoadMap(FString mapName)
{
	UGameplayStatics::OpenLevel((UObject*)this, FName(*mapName));
	

}

void UPD_ServerGameInstance::InitServerActoWhenLoadMap()
{
	APD_NW_TimerActor* ServerActorSpawned = (APD_NW_TimerActor*)GetWorld()->SpawnActor(APD_NW_TimerActor::StaticClass());

	//socketManager->InitServerActor(ServerActorSpawned);
	networkManager->GetSocketManager()->InitTimerActor(ServerActorSpawned);


}

void UPD_ServerGameInstance::sendMap()
{
	FStructGenericoHito2* m = new FStructGenericoHito2();
	m->orderType = 255; //Indica que no es una orden

	//m->stringMap=parserActor->GetStaticMap()->GetMapString(); // TODO ANGEL

	UE_LOG(LogTemp, Warning, TEXT("Enviando Map %s"), *m->stringMap);

	networkManager->SendNow(m, -1);
}



void UPD_ServerGameInstance::SetClientReady(int indexClient)
{
	networkManager->GetSocketManager()->SetSocketArrayIndex(indexClient);
}

bool UPD_ServerGameInstance::CheckForAllClientReady()
{
	bool allReady = true;

	for (int i = 0; i < networkManager->GetSocketManager()->GetReadyPlayersArray().Num(); i++)
	{
		allReady = allReady && networkManager->GetSocketManager()->GetReadyPlayersArray()[i];
	}

	return allReady;
}


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
		
		for (int i = 0; i < 1; i++)///Hay que recorrerlo m�s adelante mirando todas
		{
			auxmyIP = myIPAddress[i]->ToString(false);
			myIP.Append(auxmyIP);
		}
		//UE_LOG(LogTemp, Warning, TEXT("My ip %d"), myIP.Len());
		serverIP = myIP;
		serverName = myServerName;



		//castIP(myIP);

/*
		textToPrint = " - The Name of the Server is: " + myServerName;
		textToPrint.Append("\n - The List of Address hosted by this Host is: \n");
		textToPrint.Append(myIP);
		//textToPrint = textToPrint + "/n " + (TEXT("and the List of IP is : / n %s"), myIP);
		*/
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
	TArray<FString> playersConnected = TArray<FString>();
	FString auxNamePlayer = "";
	
	TArray<PD_NW_Socket*> socketArray = networkManager->GetSocketManager()->GetSocketArray();

	for (int i = 0; i < socketArray.Num(); i++)
	{
		auxNamePlayer = "Player ";
		auxNamePlayer.Append(FString::FromInt(i));
		playersConnected.Add(auxNamePlayer);
	}
	

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
	//TArray<bool> playersReadyArray = TArray<bool>();
	TArray<bool> playersReadyArray = networkManager->GetSocketManager()->GetReadyPlayersArray();

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

//Esta funcion la llama el actor del server map (ya colocado) para inicializar
void UPD_ServerGameInstance::InitGameMap() {
	
	UE_LOG(LogTemp, Warning, TEXT("Iniciando actor de parser"));

	// TODO ANGEL
	/* 
	AParserActor* ServerActorSpawned = (AParserActor*)GetWorld()->SpawnActor(AParserActor::StaticClass());
	//en el begin play del actor es donde se parsea el mapa desde un fichero.
	parserActor = ServerActorSpawned;

	parserActor->InitGameMap();

	sendMap();
	/**/

	
	//llamamos a la respuesta al cliente, el cliente carga el nivel del mapa
	//FStructGenericoHito2 respuesta = FStructGenericoHito2();
	//respuesta.orderType = 9; //ChangeToMap
	//networkManager->SendNow(&respuesta, -1);
	//LLamamos al send map cuando nosotros ya lo hemos parseado para poder tener el string.
	
}