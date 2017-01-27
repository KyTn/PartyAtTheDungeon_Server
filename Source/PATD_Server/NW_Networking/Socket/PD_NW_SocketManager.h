// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//forward declarations
class PD_NW_Socket;
class APD_NW_ServerActor;

/**
*
*/
class PATD_SERVER_API PD_NW_SocketManager
{
private:

	TArray<PD_NW_Socket*> socketArray;

	//Solo hay una funcion de timer, que ejecuta y guarda el SocketManager. Lo que hace es llamar a la rutina de loopPendingFunction de cada socket.
	//Para mutear un socket?
	FTimerHandle timerHandleServer;
	//FTimerManager timerManager;

	PD_NW_Socket* listenerSocket;


	APD_NW_ServerActor* myServerActor;


	//Functions ======


	///Comunicacion con la capa superior 

	//Recepcion

	/*Funcion que llama la funcion del timer cuando un socket recibe data.
	* Deberia llamar a los objetos registrados como pendientes de ese socket (por el int).
	O ser abstracta y que se implemente en cada implementacion del socketManager (sin usar objetos registrados)
	*/
	void SocketHasReceivedData(TArray<uint8> data, int socketIndex);

	/*Funcion que llama la funcion del timer cuando tiene una conexion nueva.
	//Deberia llamar a los objetos registrados como pendientes de nueva conexion.
	O ser abstracta y que se implemente en cada implementacion del socketManager (sin usar objetos registrados)
	*/
	void ListenerNewConexion(PD_NW_Socket* newSocket);




public:
	///Inicializacion
	PD_NW_SocketManager();
	~PD_NW_SocketManager();


	void SetServerActor(APD_NW_ServerActor* InmyServerActor);

	//Inicializa el socketManager y lo pone a funcionar.
	void Init(APD_NW_ServerActor* InmyServerActor);
	//Esta funcion inicia el timer. Se llamara desde el init, pero tambien individualmente al cambiar de mapa.
	void InitTimer();
	void DeleteTimer();

	bool InitListener(int port);
	bool CloseListener();


	///Gestion de registro a eventos.
	bool RegisterToSocket(int socketIndex/*,PD_NW_iSocketManagerObserver*/);

	///Gestion interna
	//Esta es la funcion que repetira el timer. Deberia recopilar los datos recibidos en los sockets e ir llamando a las funciones de salida
	void TimerRefreshFunction();


	///Comunicacion con la capa superior 

	//Envio
	bool SendInfoTo(int isocket, FString data);


};
