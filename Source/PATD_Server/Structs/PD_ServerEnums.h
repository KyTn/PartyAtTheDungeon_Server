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

enum class EGameState {Instantiate_Map, Start_Match, WaitingPlayerOrders, ExecutingPlayersTurn, WaitingEnemiesOrders, ExecutingEnemiesTurn, EndOfTurn };

enum class EServerState { StartApp, WaitingMasterClient, WaitingGameConfiguration, Lobby_Tabern,  Launch_Match, GameInProcess, Podium, OnExit };

//Maquina de estados de fases
enum class EServerPhase {StartPhases, ConsumableIni, ConsumableCamera, ConsumableTick, MoveIni,MoveCamera, MoveTick, InteractionIni,InteractionCamera,InteractionTick, AttackIni, AttackCamera, AttackTick, EndAllPhases};

//Fases que tenemos
enum class EActionPhase {Move,Attack};


enum class EClientState { NoConnection, ConfigureGame, WaitingConfiguration, ConfigureCharacter, GameInProcess };

//Cabezones
UENUM(BlueprintType)
enum class SkinHeads
{
	Legionary_SwordAndShield = 1,
	Legionary_2HandSword = 2,
	Legionary_Daggers = 3,
	Assasin_CrossbowAndShield = 4,
	Assasin_2Guns = 5,
	Assasin_Bow = 6,
	Wizard_WandAndShield = 7,
	Wizard_ScepterAndArcaneBook = 8,
	Wizard_Staff = 9

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
	TwoHandSword = 12,
	Daggers = 13,
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
	ShotMana = 11
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
enum class EAnimationType {Iddle,Move,Attack};

enum class StaticMapElement { EMPTY= 'O', NORMAL_TILE = '.', SPECIAL_TILE = ',', WALL_ONLY = 'W', WALL_OR_DOOR = 'w', DOOR = 'd', SPAWN_POINT = 's'};
enum class MapSkinType{DUNGEON_NORMAL = 0, GARDEN = 10, LIBRARY = 20, SACRIFICE = 30, BOSS = 100};

#pragma region MATCH CONFIG

// Cuando se envía por un FStructMatchConfig, cada ID se correspone al comentario que hay encima de cada uno


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