#pragma once
enum class EGameState { WaitingPlayerOrders, ExecutingPlayersLogic, ExecutingPlayersVisualization, ExecutingActionOrders};
enum class EServerState { WaitingClientMaster, WaitingGameConfiguration, WaitingReady,GameInProcess };
enum class InteractionStates { Ready, Working, Finish };
enum class EActionPhase { Move=0, Attack=1, EndTurn=2 };