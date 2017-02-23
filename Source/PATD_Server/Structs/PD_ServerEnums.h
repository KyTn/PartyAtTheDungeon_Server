#pragma once
enum class EGameState { WaitingPlayerOrders, ExecutingActionOrders };
enum class EServerState { WaitingClientMaster, WaitingGameConfiguration, WaitingReady,GameInProcess };
enum class InteractionStates { Ready, Working, Finish };