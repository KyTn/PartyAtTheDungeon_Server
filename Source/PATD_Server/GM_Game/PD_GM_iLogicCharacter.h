// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

/**
 * 
 */
class PATD_SERVER_API PD_GM_iLogicCharacter
{
public:
	PD_GM_iLogicCharacter();
	~PD_GM_iLogicCharacter();

	//Aqui irian funciones comunes a todos los characters, como moverse atacar y demas
	//Pero para poder calcular el da�o, el rango, etc, serian funciones virutales, 
	//pues los enemigos y los personajes lo calculan de diferente manera (unos llevan armas y dependera de esta y otros no, es fijo)

	//Tambien guardaremos punteros a las cosas que nos interesen, como el actor fisico que lo represente.
};
