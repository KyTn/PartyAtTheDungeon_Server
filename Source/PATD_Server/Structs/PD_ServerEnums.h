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
enum class ActiveSkills { Punch = 1, Fireball = 2, JustInTheEye = 3, ForTheKing = 4, LookingForCuenca = 5 };
enum class PasiveSkills { NotInMyGuard = 1, TheGoodBier = 2, KingInTheNorth = 3, HelloWorld = 4, YouShallNotPass = 5 };

//Lista que define TODOS las posibles acciones (ataques basicos, habilidades, etc) que puede hacer un character (enemigo o jugador)
UENUM(BlueprintType)
enum class ActionsByCharacter
{
	BasicAttack = 1
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