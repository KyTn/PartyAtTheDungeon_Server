// Fill out your copyright notice in the Description page of Project Settings.

#include "PATD_Server.h"
#include "PD_GM_Pathfinder.h"

#include "MapGeneration/PD_MG_LogicPosition.h"

PD_GM_Pathfinder::PD_GM_Pathfinder(PD_GM_MapManager* inMapManager)
{
	mapManager = inMapManager;
}

PD_GM_Pathfinder::~PD_GM_Pathfinder()
{
}





////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// STL A* Search implementation
// (C)2001 Justin Heyes-Jones
//
// Finding a path on a simple grid maze
// This shows how to do shortest path finding using A*

////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#include "GM_Game/PD_GM_MapManager.h"
//#include <iostream>
//#include <stdio.h>
//#include <math.h>




// Definitions

class MapSearchNode
{
public:
	int x;	 // the (x,y) positions of the node
	int y;
	PD_GM_MapManager* mapManager;


	MapSearchNode() { x = y = 0; }
	MapSearchNode(int px, int py, PD_GM_MapManager* inMapManager) { x = px; y = py; mapManager = inMapManager; }

	float GoalDistanceEstimate(MapSearchNode &nodeGoal);
	bool IsGoal(MapSearchNode &nodeGoal);
	bool GetSuccessors(AStarSearch<MapSearchNode> *astarsearch, MapSearchNode *parent_node);
	float GetCost(MapSearchNode &successor);
	bool IsSameState(MapSearchNode &rhs);

	void PrintNodeInfo();


};

bool MapSearchNode::IsSameState(MapSearchNode &rhs)
{

	// same state in a maze search is simply when (x,y) are the same
	if ((x == rhs.x) &&
		(y == rhs.y))
	{
		return true;
	}
	else
	{
		return false;
	}

}

void MapSearchNode::PrintNodeInfo()
{
	UE_LOG(LogTemp, Log, TEXT("MapSearchNode::PrintNodeInfo: x:%d , y:%d"),this->x,this->y);
}

// Here's the heuristic function that estimates the distance from a Node
// to the Goal. 

float MapSearchNode::GoalDistanceEstimate(MapSearchNode &nodeGoal)
{
	return fabsf(x - nodeGoal.x) + fabsf(y - nodeGoal.y);
}

bool MapSearchNode::IsGoal(MapSearchNode &nodeGoal)
{

	if ((x == nodeGoal.x) &&
		(y == nodeGoal.y))
	{
		return true;
	}

	return false;
}

// This generates the successors to the given Node. It uses a helper function called
// AddSuccessor to give the successors to the AStar class. The A* specific initialisation
// is done for each node internally, so here you just set the state information that
// is specific to the application
bool MapSearchNode::GetSuccessors(AStarSearch<MapSearchNode> *astarsearch, MapSearchNode *parent_node)
{

	int parent_x = -1;
	int parent_y = -1;

	if (parent_node)
	{
		parent_x = parent_node->x;
		parent_y = parent_node->y;
	}

	TArray<PD_MG_LogicPosition> adyacentsList= mapManager->Get_LogicPosition_Adyacents_To(PD_MG_LogicPosition(parent_x, parent_y));

	for (PD_MG_LogicPosition adyacentLogicPosition : adyacentsList) {
		if (mapManager->IsThereWall(adyacentLogicPosition.GetX(), adyacentLogicPosition.GetY())) {
			MapSearchNode NewNode;
			NewNode = MapSearchNode(adyacentLogicPosition.GetX(), adyacentLogicPosition.GetY(), mapManager);
			astarsearch->AddSuccessor(NewNode);
		}

	}
	


	return true;
}

// given this node, what does it cost to move to successor. In the case
// of our map the answer is the map terrain value at this node since that is 
// conceptually where we're moving

float MapSearchNode::GetCost(MapSearchNode &successor)
{
	//return (float)GetMap(x, y);
	return 1;

}


// Main

TArray<PD_MG_LogicPosition> PD_GM_Pathfinder::getPathFromTo(PD_MG_LogicPosition posFrom, PD_MG_LogicPosition posTo)
{
	AStarSearch<MapSearchNode> astarsearch;
	TArray<PD_MG_LogicPosition> returnList;
		// Create a start state
		MapSearchNode nodeStart;
		nodeStart.x = posFrom.GetX();
		nodeStart.y = posFrom.GetY();
		nodeStart.mapManager = mapManager;

		// Define the goal state
		MapSearchNode nodeEnd;
		nodeEnd.x = posTo.GetX();
		nodeEnd.y = posTo.GetY();
		nodeEnd.mapManager = mapManager;
		// Set Start and goal states

		astarsearch.SetStartAndGoalStates(nodeStart, nodeEnd);

		int searchState;
		int searchSteps = 0;

		do
		{
			searchState = astarsearch.SearchStep();

			searchSteps++;



		} while (searchState == AStarSearch<MapSearchNode>::SEARCH_STATE_SEARCHING);

		//Aqui la busqueda ha terminado

		if (searchState == AStarSearch<MapSearchNode>::SEARCH_STATE_SUCCEEDED)
		{

			MapSearchNode *node = astarsearch.GetSolutionStart();

			int steps = 0;

			node->PrintNodeInfo();
			for (;; )
			{
				node = astarsearch.GetSolutionNext();

				if (!node)
				{
					break;
				}

				returnList.Add(PD_MG_LogicPosition(node->x,node->y));
				node->PrintNodeInfo();

				steps++;

			};

			// Once you're done with the solution you can free the nodes up
			astarsearch.FreeSolutionNodes();


		}
		else if (searchState == AStarSearch<MapSearchNode>::SEARCH_STATE_FAILED)
		{
			UE_LOG(LogTemp, Log, TEXT("PD_GM_Pathfinder::getPathFromTo: Error: SEARCH_STATE_FAILED"));

		}



		astarsearch.EnsureMemoryFreed();

		return returnList;
	}

	//return 0;


