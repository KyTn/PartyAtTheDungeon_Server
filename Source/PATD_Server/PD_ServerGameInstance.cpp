// Fill out your copyright notice in the Description page of Project Settings.

#include "PATD_Server.h"
#include "PD_ServerGameInstance.h"

#include "NW_NetWorking/PD_NW_ServerActor.h"
#include "NW_NetWorking/PD_NW_NetworkManager.h"

//Includes of forward declaration
#include "NW_NetWorking/Socket/PD_NW_SocketManager.h"

//Includes de prueba
#include "NW_Networking/EventLayer/PD_NW_iEventObserver.h"



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

			if (dataStruct->orderType != -1) { //NullOrder
				FStructGenericoHito2 respuesta =  FStructGenericoHito2();
				switch (dataStruct->orderType) {
				case 0: //New connection

					gi->numPlayers++;
					if (gi->GetWorld()->GetMapName() != "LVL_4_GameMap") {
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
					gi->ready->Init(false,gi->numPlayers);
					//IniciarArray de readys con el numero de jugadores actual
					//Si uno se cae en este punto no se podra iniciar partida jamas xD

					break;
				case 4://ClientReady
					if (gi->ready[inPlayer].Contains(false)) 
					{
						gi->ready->Insert(true, inPlayer);
						bool allReady = true;
						for (size_t i = 0; i < gi->ready->Num() && allReady; i++)
						{
							if (gi->ready[inPlayer].Contains(false))
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
						gi->ready->Insert(false,inPlayer);
					
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

	networkManager= new PD_NW_NetworkManager();
	PD_NW_SocketManager* socketManager = networkManager->GetSocketManager();
	//socketManager = new PD_NW_SocketManager();
	socketManager->SetIsServer(true);

	APD_NW_ServerActor* ServerActorSpawned = (APD_NW_ServerActor*)GetWorld()->SpawnActor(APD_NW_ServerActor::StaticClass());
	socketManager->SetNetworkManager(networkManager);
	//Donde se pone el puerto que es como una constante global?
	socketManager->Init(ServerActorSpawned, "", defaultServerPort);//Con esto empezaria el timer, quizas no lo queremos llamar aqui o queremos separarlo entre init y start
	
	
}
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
	APD_NW_ServerActor* ServerActorSpawned = (APD_NW_ServerActor*)GetWorld()->SpawnActor(APD_NW_ServerActor::StaticClass());

	//socketManager->InitServerActor(ServerActorSpawned);
	networkManager->GetSocketManager()->InitServerActor(ServerActorSpawned);


}

void UPD_ServerGameInstance::sendMap(){
FStructGenericoHito2* m = new FStructGenericoHito2();
m->orderType = -1; //Indica que no es una orden
UE_LOG(LogTemp, Warning, TEXT("Enviando Map"));

networkManager->SendNow(m,-1);
}