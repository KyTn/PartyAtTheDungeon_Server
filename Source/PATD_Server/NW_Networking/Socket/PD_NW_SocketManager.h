// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//forward declarations
class PD_NW_Socket;

/**
*
*/
class PATD_SERVER_API PD_NW_SocketManager
{
private:
	TArray<PD_NW_Socket*> socketArray;

	//Solo hay una funcion de timer, que ejecuta y guarda el SocketManager. Lo que hace es llamar a la rutina de loopPendingFunction de cada socket.
	//Para mutear un socket?
	FTimerHandle timer;
	FTimerManager* timerManager;

	PD_NW_Socket* listenerSocket;


	


	//Functions ======



	///Gestion interna
	//Esta es la funcion que repetira el timer. Deberia recopilar los datos recibidos en los sockets e ir llamando a las funciones de salida
	void timerRefreshFunction();


	


	///Comunicacion con la capa superior 

	//Recepcion

	/*Funcion que llama la funcion del timer cuando un socket recibe data.
	* Deberia llamar a los objetos registrados como pendientes de ese socket (por el int).
	O ser abstracta y que se implemente en cada implementacion del socketManager (sin usar objetos registrados)
	*/
	void socketHasReceivedData(TArray<uint8> data, int socketIndex);

	/*Funcion que llama la funcion del timer cuando tiene una conexion nueva.
	//Deberia llamar a los objetos registrados como pendientes de nueva conexion.
	O ser abstracta y que se implemente en cada implementacion del socketManager (sin usar objetos registrados)
	*/
	void listenerNewConexion(PD_NW_Socket* newSocket);




public:
	///Inicializacion
	PD_NW_SocketManager();
	~PD_NW_SocketManager();

	//Inicializa el socketManager y lo pone a funcionar.
	void init() {};
	//Esta funcion inicia el timer. Se llamara desde el init, pero tambien individualmente al cambiar de mapa.
	void initTimer();
	void deleteTimer();

	bool initListener(int port);
	bool closeListener();

	//Setear TimerManager
	//Al cambiar de mundo, la referencia a TimerManager (o a actor) que tengamos se corrompera, asique necesitaremos una nueva.
	void setFTimerManager(FTimerManager* timerManager) {};



	///Gestion de registro a eventos.
	bool registerToSocket(int socketIndex/*,PD_NW_iSocketManagerObserver*/);




	///Comunicacion con la capa superior 

		//Envio
			bool SendInfoTo(int isocket, FString data);





};
