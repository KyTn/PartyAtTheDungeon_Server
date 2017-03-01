#pragma once

enum class EGameState {Instantiate_Map, Start_Match, WaitingPlayerOrders, ExecutingPlayersLogic, ExecutingPlayersVisualization, ExecutingEnemiesLogic, ExecutingEnemiesVisualization,EndOfTurn };

enum class EServerState { WaitingClientMaster, WaitingGameConfiguration, WaitingReady,GameInProcess };
enum class InteractionStates { Ready, Working, Finish };
enum class EActionPhase { Move=0, Attack=1, EndPhase=2 };



enum class EClientState { NoConnection, ConfigureGame, WaitingConfiguration, ConfigureCharacter, GameInProcess };
enum class ActiveSkills { Punch = 1, Fireball = 2, JustInTheEye = 3, ForTheKing = 4, LookingForCuenca = 5 };
enum class PasiveSkills { NotInMyGuard = 1, TheGoodBier = 2, KingInTheNorth = 3, HelloWorld = 4, YouShallNotPass = 5 };


enum class ECharacterType { Player, Archer, Zombie };

enum class EAnimationType {Iddle,Move,Attack};