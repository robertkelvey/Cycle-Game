//Code to play Make-a-Cycle, Avoid-a-Cycle Graph Traversal games
//optimalPlay returns a 1 if the game is in a win state, and a 0 if it's in a loss state

//MAKE-A-CYCLE VERSION HERE 

//In terminal needs to be compiled with special option at the end "--std=c++17"
//i.e. g++ gameMACV1.cpp -o gameMACV1.exe --std=c++17
//Mingw g++ version 9.2.0

#include <iostream>
#include <math.h>
#include <limits>
#include <climits>
#include <iomanip>
#include <string>
#include <fstream>

#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H
#include <filesystem>

int readInCounter(std::string filepath)
{
	std::string node1, node2;
	int intnode1, intnode2, numnodes;
	int max = -1 * std::numeric_limits<int>::infinity();


	std::ifstream data(filepath.c_str());
	if (data.is_open())
	{
		std::cout << "Successfully opened the node adjacency data text file. (readInCounter)" << std::endl;
	}
	else
	{
		std::cout << "Unable to open the node adjacency data text file." << std::endl;
		return -1;
	}

	while (!data.eof())
	{
		std::getline(data, node1, ',');
		std::getline(data, node2, '\n');

		intnode1 = std::stoi(node1);
		intnode2 = std::stoi(node2);
		if (intnode1 > max)
		{
			max = intnode1;
		}
		if (intnode2 > max)
		{
			max = intnode2;
		}
	}

	data.close();

	numnodes = max + 1;
	return numnodes;
}

void readIn(std::string filepath, int numnodes, int** AdjacencyMatrixFunction)
{
	std::string node1, node2;
	int intnode1, intnode2;


	for (int i = 0; i < numnodes; i++)
	{
		for (int j = 0; j < numnodes; j++)
		{
			AdjacencyMatrixFunction[i][j] = 0;
		}
	}

	std::ifstream data(filepath.c_str());
	if (data.is_open())
	{
		std::cout << "Successfully opened the node adjacency data text file. (readIn)" << std::endl;
	}
	else
	{
		std::cout << "Unable to open the node adjacency data text file." << std::endl;
		std::cout << "Return to the caller with a blank adjacency matrix." << std::endl;
		return;
	}

	while (!data.eof())
	{
		std::getline(data, node1, ',');
		std::getline(data, node2, '\n');

		intnode1 = std::stoi(node1);
		intnode2 = std::stoi(node2);

		AdjacencyMatrixFunction[intnode1][intnode2] = AdjacencyMatrixFunction[intnode2][intnode1] = 1;
	}

	data.close();

	return;
}

void clearchar(std::string s)
{
	if (!s.empty())
	{
		s.erase(s.size() - 1);
	}
	return;
}

std::string playerprint(int x)
{
	if (x == 0)
	{
		return "P1: ";
	}
	else if (x == 1)
	{
		return "P2: ";
	}
	return "";
}

void printmovehistory(int recursiondepth, int* matrix, std::ofstream& outputfile)
{
	std::cout << matrix[0];
	outputfile << matrix[0];
	for (int i = 1; i <= recursiondepth; i++)
	{
		std::cout << "->" << matrix[i];
		outputfile << "->" << matrix[i];
	}
	return;
}

int optimalPlay(int currentnode, int numnodes, int** AdjacencyMatrix, int** edgeusageMatrix, int* nodeusageMatrix, int* movehistory, std::string indentation, int whoseturn, int recursiondepth, std::ofstream& outputfile)
{
	indentation += "\t";
	std::cout << indentation << playerprint(whoseturn) << "Just reached node " << currentnode << std::endl; //write to console
	outputfile << indentation << playerprint(whoseturn) << "Just reached node " << currentnode << std::endl; //write to output file
	movehistory[recursiondepth] = currentnode;
	int edgecounter = 0; //keeps track of number unused edges connected to the current node
	int result = 0;

	std::cout << indentation << playerprint(whoseturn) << "Checking to see if there's any cycles that are one move away." << std::endl;
	outputfile << indentation << playerprint(whoseturn) << "Checking to see if there's any cycles that are one move away." << std::endl;
	for (int i = 0; i < numnodes; i++) //for all possible edges connected to this node in the adjacency matrix, scope issues here?
	{
		if ((AdjacencyMatrix[currentnode][i] == 1) && (edgeusageMatrix[currentnode][i] == 0)) //go through all the node's existing, unused edges
		{
			std::cout << indentation << playerprint(whoseturn) << "Checking the play from node " << currentnode << " to " << i << "." << std::endl;
			outputfile << indentation << playerprint(whoseturn) << "Checking the play from node " << currentnode << " to " << i << "." << std::endl;
			edgecounter++;
			if (nodeusageMatrix[i] == 1) //if it's been previously visited, going back creates a cycle!
			{
				std::cout << indentation << playerprint(whoseturn) << "Cycle detected.";
				outputfile << indentation << playerprint(whoseturn) << "Cycle detected.";
				std::cout << " Move History: ";
				outputfile << " Move History: ";
				printmovehistory(recursiondepth, movehistory, outputfile);
				std::cout << "->" << i;
				outputfile << "->" << i;
				std::cout << std::endl;
				outputfile << std::endl;
				clearchar(indentation);
				return 1; //if it's a winning move we're done here
			}
			std::cout << indentation << playerprint(whoseturn) << "No cycle detected." << std::endl;
			outputfile << indentation << playerprint(whoseturn) << "No cycle detected." << std::endl;
		}
	}
	if (edgecounter == 0) //if there's no legal moves the game is in a Loss state
	{
		std::cout << indentation << playerprint(whoseturn) << "No valid moves remaining" << std::endl;
		outputfile << indentation << playerprint(whoseturn) << "No valid moves remaining" << std::endl;
		std::cout << " Move History: ";
		outputfile << " Move History: ";
		printmovehistory(recursiondepth, movehistory, outputfile);
		std::cout << std::endl;
		outputfile << std::endl;
		clearchar(indentation);
		return 0;
	}
	std::cout << indentation << playerprint(whoseturn) << "Checking through all the available moves now." << std::endl;
	outputfile << indentation << playerprint(whoseturn) << "Checking through all the available moves now." << std::endl;
	for (int i = 0; i < numnodes; i++) //for all possible edges connected to this node in the adjacency matrix, scope issues here?
	{
		if ((AdjacencyMatrix[currentnode][i] == 1) && (edgeusageMatrix[currentnode][i] == 0)) //go through all the node's existing, unused edges
		{
			std::cout << indentation << playerprint(whoseturn) << "What if we play from " << currentnode << " to " << i << "?" << std::endl;
			outputfile << indentation << playerprint(whoseturn) << "What if we play from " << currentnode << " to " << i << "?" << std::endl;
			edgeusageMatrix[currentnode][i] = edgeusageMatrix[i][currentnode] = 1; //add the edge to the graph
			nodeusageMatrix[i] = 1;
			result = optimalPlay(i, numnodes, AdjacencyMatrix, edgeusageMatrix, nodeusageMatrix, movehistory, indentation, (whoseturn + 1) % 2, recursiondepth + 1, outputfile);
			std::cout << indentation << playerprint(whoseturn) << "Playing from " << currentnode << " to " << i << " results in a " << (result + 1) % 2 << ".";
			outputfile << indentation << playerprint(whoseturn) << "Playing from " << currentnode << " to " << i << " results in a " << (result + 1) % 2 << ".";
			std::cout << " Move History: ";
			outputfile << " Move History: ";
			printmovehistory(recursiondepth, movehistory, outputfile);
			std::cout << std::endl;
			outputfile << std::endl;
			nodeusageMatrix[i] = 0; //reset the node's visited status now that we're returning
			edgeusageMatrix[currentnode][i] = edgeusageMatrix[i][currentnode] = 0; //reset the edge's usage status now that we're returning	
			if (result == 0) //if the move puts the game in a loss state for the other player, it's a win state for us
			{
				std::cout << indentation << playerprint(whoseturn) << "Win state at node " << currentnode << ".";
				outputfile << indentation << playerprint(whoseturn) << "Win state at node " << currentnode << ".";
				std::cout << " Move History: ";
				outputfile << " Move History: ";
				printmovehistory(recursiondepth, movehistory, outputfile);
				std::cout << std::endl;
				outputfile << std::endl;
				clearchar(indentation);
				return 1;
			}
		}
	}
	std::cout << indentation << playerprint(whoseturn) << "There are no good moves here, the game is in a loss state at node " << currentnode << "." << std::endl;
	outputfile << indentation << playerprint(whoseturn) << "There are no good moves here, the game is in a loss state at node " << currentnode << "." << std::endl;
	clearchar(indentation);
	return 0; //no moves to place the game in a Loss state? Then this is a loss state
}

int main()
{
	//{Add in game selection (MAC vs. AAC) here?


	//}//end of game selection

	//{Graph selection here
	int startingnode = -1;
	int graphselection;
	std::cout << "What graph would you like to play on? The pre-loaded graphs are:" << std::endl;
	std::string selectionpath = "C:\\Users\\willl\\Desktop\\2021 Summer Research\\Adjacency Matrices\\";
	int filecounter = 0;
	int fileselector = 0;

	for (const auto& entry : std::filesystem::directory_iterator(selectionpath)) //wtf?
	{
		std::cout << "[" << filecounter << "] " << entry.path() << std::endl;
		filecounter++;
	}
	std::cout << "Enter a selection (0 - " << filecounter - 1 << ")" << std::endl;
	std::cin >> graphselection;
	std::cin.clear();
	std::cin.ignore(INT_MAX, '\n');
	while ((graphselection < 0) || (graphselection > filecounter - 1))
	{
		std::cout << "Invalid selection number! Please enter a value from 0 to " << filecounter - 1 << "." << std::endl;
		std::cin >> graphselection;
		std::cin.clear();
		std::cin.ignore(INT_MAX, '\n');
	}

	for (const auto& entry : std::filesystem::directory_iterator(selectionpath)) //wtf?
	{
		if (fileselector == graphselection)
		{
			selectionpath = std::filesystem::absolute(entry.path()).string();
			break;
		}
		fileselector++;
	}

	//std::cout<<"Chosen file path:"<<std::endl; //debug print check for file selection
	//std::cout<<selectionpath<<std::endl;

	//} end of graph selection block

	//{Some initialization business (DON'T MOVE THIS BLOCK)
	int numnodes = readInCounter(selectionpath);
	std::string indentation, outputfilename;
	int recursiondepth = 0;
	int whoseturn = 0;

	int* AdjacencyMatrix[numnodes];
	int* edgeusageMatrix[numnodes];
	int nodeusageMatrix[numnodes];
	int movehistory[numnodes];

	//maybe combine loops 1 and 2?
	for (int i = 0; i < numnodes; i++)
	{
		AdjacencyMatrix[i] = new int[numnodes];
		edgeusageMatrix[i] = new int[numnodes];
	}

	for (int i = 0; i < numnodes; i++)
	{
		for (int j = 0; j < numnodes; j++)
		{
			edgeusageMatrix[i][j] = 0;
		}
	}

	for (int i = 0; i < numnodes; i++)
	{
		nodeusageMatrix[i] = 0;
	}
	//} End of initialization business

	//{Output file stuff
	std::string filepathout = "C:\\Users\\willl\\Desktop\\2021 Summer Research\\Code Output\\";
	std::cout << "What do you want to name the output file?" << std::endl;
	std::cin >> outputfilename;
	std::cin.clear();
	std::cin.ignore(INT_MAX, '\n');
	outputfilename.append(".txt");
	filepathout.append(outputfilename);
	std::ofstream outputfile;
	outputfile.open(filepathout.c_str());

	if (outputfile.is_open())
	{
		std::cout << "Successfully opened the output text file to store the code's print statements." << std::endl;
	}
	else
	{
		std::cout << "Unable to open the output text file to store the code's print statements." << std::endl;
		std::cout << "The code will continue to run but no results will be permanently saved." << std::endl;
	}

	readIn(selectionpath, numnodes, AdjacencyMatrix);

	std::cout << "Printout of the adjacency matrix for debugging purposes:" << std::endl;
	outputfile << "Printout of the adjacency matrix for debugging purposes:" << std::endl;
	for (int i = 0; i < numnodes; i++)
	{
		for (int j = 0; j < numnodes; j++)
		{
			std::cout << AdjacencyMatrix[i][j] << " ";
			outputfile << AdjacencyMatrix[i][j] << " ";
		}
		std::cout << std::endl;
		outputfile << std::endl;
	}
	//} End of output file stuff

	//{Here we go...
	std::cout << "At which node will we begin? (0 - " << numnodes - 1 << ")" << std::endl;
	std::cin >> startingnode;
	std::cin.clear();
	std::cin.ignore(INT_MAX, '\n');
	while ((startingnode < 0) || (startingnode > numnodes - 1))
	{
		std::cout << "Invalid node number! Please enter a value from 0 to " << numnodes - 1 << "." << std::endl;
		std::cin >> startingnode;
		std::cin.clear();
		std::cin.ignore(INT_MAX, '\n');
	}

	nodeusageMatrix[startingnode] = 1;
	std::cout << "Beginning the game at node " << startingnode << ", on the graph constructed based off of: " << std::endl << selectionpath << std::endl;
	outputfile << "Beginning the game at node " << startingnode << ", on the graph constructed based off of: " << std::endl << selectionpath << std::endl;

	std::cout << optimalPlay(startingnode, numnodes, AdjacencyMatrix, edgeusageMatrix, nodeusageMatrix, movehistory, indentation, whoseturn, recursiondepth, outputfile) << std::endl;

	outputfile.close();

	//}That's all folks

	return 0;
}

#endif



