#pragma once

/*template <typename T>
class EnumParser
{
std::map <string, T> enumMap;
public:
EnumParser(){};

T ParseSomeEnum(const string &value)
{
std::map <string, T>::const_iterator iValue = enumMap.find(value);
if (iValue  == enumMap.end())
throw runtime_error("");
return iValue->second;
}
};
*/

enum class EGameState { Instantiate_Map, Start_Match, WaitingPlayerOrders, ExecutingPlayersTurn, WaitingEnemiesOrders, ExecutingEnemiesTurn, EndOfTurn };

enum class EServerState { StartApp, WaitingMasterClient, WaitingGameConfiguration, Lobby_Tabern, Launch_Match, GameInProcess, Podium, OnExit };

//Maquina de estados de fases
enum class EServerPhase { StartPhases, ConsumableIni, ConsumableCamera, ConsumableTick, MoveIni, MoveCamera, MoveTick, InteractionIni, InteractionCamera, InteractionTick, AttackIni, AttackCamera, AttackTick, EndAllPhases };

//Fases que tenemos
enum class EActionPhase { Move, Attack };


enum class EClientState { NoConnection, ConfigureGame, WaitingConfiguration, ConfigureCharacter, GameInProcess };

//Cabezones
UENUM(BlueprintType)
enum class SkinHeads
{
	Legionary_SwordAndShield = 0,
	Legionary_2HandSword = 1,
	Legionary_Daggers = 2,
	Assasin_CrossbowAndShield = 3,
	Assasin_2Guns = 4,
	Assasin_Bow = 5,
	Wizard_WandAndShield = 6,
	Wizard_ScepterAndArcaneBook = 7,
	Wizard_Staff = 8

};

//clase de Arma // Rama del personaje
UENUM(BlueprintType)
enum class ClassWeapons
{
	Melee = 10,
	Range = 20,
	Magic = 30
};

//Tipo de Arma
UENUM(BlueprintType)
enum class TypeWeapons
{
	SwordAndShield = 11,
	Daggers = 12,
	TwoHandSword = 13,
	CrossbowAndShield = 21,
	TwoGuns = 22,
	Bow = 23,
	WandAndShield = 31,
	ScepterAndArcaneBook = 32,
	Staff = 33
};

//Lista que define TODOS las posibles acciones (ataques basicos, habilidades, etc) que puede hacer un character (enemigo o jugador)
UENUM(BlueprintType)
enum class ActiveSkills
{
	BasicAttack = 0,
	Defense = 1,
	WhenFua = 2,
	JumpFatTigger = 3,
	Hostion = 4,
	SomeHit = 5,
	RightInTheAsshole = 6,
	GiveMeTheFireBlast = 7,
	Exclaimchas = 8,
	BeInCrossroads = 9,
	WhoHeal = 10,
	ShotMana = 11,
	GetHurt = 250,
};

//Lista que define TODOS las posibles habilidades pasivas
UENUM(BlueprintType)
enum class PasiveSkills
{
	IamTank = 0,
	KingMistery = 1,
	BlindFury = 2,
	IamTankInDistance = 3,
	RedBullets = 4,
	SquabEye = 5,
	MagicShieldBro = 6,
	WaitForNotes = 7,
	PowerfulStick = 8,
	HotestBlod = 9,
	Borderline = 10,
	JokesWithSparkle = 11,
	LaRemedios = 12,
	TwoblackCandles = 13,
	TheSmarty = 14,
	AsBull = 15,
	TheSweeper = 16
};

enum class ECharacterType { NoCharacter, Player, Archer, Zombie };
/*
EnumParser<ECharacterType>::EnumParser()
{
enumMap["NoCharacter"] = NoCharacter;
enumMap["Player"] = Player;
enumMap["Archer"] = Archer;
enumMap["Zombie"] = Zombie;
}
*/
enum class EAnimationType { Iddle, Move, Attack };


// tipo de mapskin
UENUM(BlueprintType)
enum class MapSkinType { DUNGEON_NORMAL = 0, GARDEN = 10, LIBRARY = 20, SACRIFICE = 30, BOSS = 100 };

// elementos que puedes encontrar en el fichero de roomtemplates 
enum class StaticMapElementInRoomTemplate { EMPTY = 'O', NORMAL_TILE = '.', SPECIAL_TILE = ',', WALL_ONLY = 'W', WALL_OR_DOOR = 'w', DOOR = 'd', /*SPAWN_POINT = 's',*/ COLUMN = 'c' };

// id de cada tipo diferente de prop (dependiente del mapskintype)
enum class StaticMapElement {
	// del 0 al 9, elementos empty default
	EMPTY = 0,
	// del 10 al 19 elementos default
	NORMAL_TILE = 10, SPECIAL_TILE = 11,

	// del 20 al 149, props
	TREE_OR_COLUMN_00 = 20, TREE_OR_COLUMN_01 = 21, TREE_OR_COLUMN_02 = 22,

	// a partir del 150, interactuables 
	DOOR = 150, LEVEL = 151, PRESURE_PLATE = 152, LARGE_CHEST = 153, SMALL_CHEST = 154,

	// elementos especiales 
};
/*

StaticMapElement* GetMyId(MapSkinType mst, StaticMapElementInRoomTemplate melem ) {

switch (mst) {

case MapSkinType::DUNGEON_NORMAL: {

switch (melem) {

case StaticMapElementInRoomTemplate::COLUMN :

// cuales son los elementos que tengo disponibles
// COLUMN_00 = 20, COLUMN_01 = 21, COLUMN_02 = 22,

return aleatorio
}
}


case MapSkinType::GARDEN: {

switch (melem) {

case StaticMapElementInRoomTemplate::COLUMN:


// cuales son los elementos que tengo disponibles
// COLUMN_00 = 20

return aleatorio


}
}


}

}

AActor* Instantiator(MapSkinType mst, StaticMapElement sme) {

switch (mst) {

case MapSkinType::DUNGEON_NORMAL: {

switch (melem) {

case StaticMapElement::COLUMN_00:

// instanciar el BP que sea

return actor;
}
}


}
*/

#pragma region MATCH CONFIG

// Cuando se env�a por un FStructMatchConfig, cada ID se correspone al comentario que hay encima de cada uno


// ID 0
UENUM(BlueprintType)        //"BlueprintType" is essential to include
enum class MATCHCONFIG_MISSIONTYPE : uint8
{
	DefeatBoss     UMETA(DisplayName = "DefeatBoss"),
	DefeatAll    UMETA(DisplayName = "DefeatAll"),
	RecoverTreasure     UMETA(DisplayName = "RecoverTreasure")
};

// ID 1
enum class MATCHCONFIG_MAPSIZE {
	SMALL_SIZE, NORMAL_SIZE, LARGE_SIZE
};

// ID 2
enum class MATCHCONFIG_DIFFICULTY {
	EASY_DIFFICULTY, NORMAL_DIFFICULTY, DIFFICULT_DIFFICULTY
};

#pragma endregion