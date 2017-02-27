#pragma once
<<<<<<< HEAD
enum class EGameState { WaitingPlayerOrders, ExecutingPlayersLogic, ExecutingPlayersVisualization,EndOfTurn };
=======
enum class EGameState { WaitingPlayerOrders, ExecutingPlayersLogic, ExecutingPlayersVisualization, ExecutingActionOrders};
>>>>>>> a96fa556403e4c379908d71d066954f01c37f669
enum class EServerState { WaitingClientMaster, WaitingGameConfiguration, WaitingReady,GameInProcess };
enum class InteractionStates { Ready, Working, Finish };
enum class EActionPhase { Move=0, Attack=1, EndTurn=2 };


enum class EClientState { NoConnection, ConfigureGame, WaitingConfiguration, ConfigureCharacter, GameInProcess };
enum class ActiveSkills { Punch = 1, Fireball = 2, JustInTheEye = 3, ForTheKing = 4, LookingForCuenca = 5 };
enum class PasiveSkills { NotInMyGuard = 1, TheGoodBier = 2, KingInTheNorth = 3, HelloWorld = 4, YouShallNotPass = 5 };