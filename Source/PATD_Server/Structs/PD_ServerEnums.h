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

enum class EGameState {Instantiate_Map, Start_Match, WaitingPlayerOrders, ExecutingPlayersLogic, ExecutingPlayersVisualization, ExecutingEnemiesLogic, ExecutingEnemiesVisualization,EndOfTurn };

enum class EServerState { WaitingClientMaster, WaitingGameConfiguration, WaitingReady,GameInProcess };
enum class InteractionStates { Ready, Working, Finish };
enum class EActionPhase { Move=0, Attack=1, EndPhase=2 };



enum class EClientState { NoConnection, ConfigureGame, WaitingConfiguration, ConfigureCharacter, GameInProcess };
enum class ActiveSkills { Punch = 1, Fireball = 2, JustInTheEye = 3, ForTheKing = 4, LookingForCuenca = 5 };
enum class PasiveSkills { NotInMyGuard = 1, TheGoodBier = 2, KingInTheNorth = 3, HelloWorld = 4, YouShallNotPass = 5 };


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