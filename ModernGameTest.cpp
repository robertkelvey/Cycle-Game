//Code to play Make-a-Cycle, Avoid-a-Cycle Graph Traversal games
//optimalPlay returns a 1 if the game is in a win state, and a 0 if it's in a loss state
//MAKE-A-CYCLE + AVOID-A-CYCLE VERSION HERE 

//In terminal needs to be compiled with special option at the end "--std=c++17"
//i.e. g++ gameMACV1.cpp -o gameMACV1.exe --std=c++17
//Mingw g++ version 9.2.0
//Doesn't seem to be necessary with Virtual Studio, did manually set the C++ compiler in preferences to c++17 however

#include <iostream>
#include <math.h>
#include <limits>
#include <climits>
#include <iomanip>
#include <string>
#include <fstream>
#include <filesystem>
#include <vector>
//#include "ThreadPool.h" //might try to multithread later
#include "Profile.h"

template<typename T> 
class Tuple 
{
private:
	std::vector<T> tuple;
public:
	//parametrized constructor
	Tuple(unsigned numentries)
	{
		tuple.resize(numentries);
	}
	void setEntry(unsigned place, T value)
	{
		tuple[place] = value;
	}

	T getEntry(unsigned place)
	{
		return tuple[place];
	}

	unsigned getSize()
	{
		return (unsigned) tuple.size();
	}

	void printTuple()
	{
		std::cout << '(';
		for (unsigned i = 0; i < getSize() - 1; i++)
		{
			std::cout << tuple[i] << ", ";
		}
		std::cout << tuple[getSize() - 1] << ')';
	}
};

unsigned Index2Dto1D(unsigned i, unsigned j, unsigned numnodes)
{
	return (numnodes * i) + j;
}

//tells whether .txt file contains an adjacency matrix or adjacency listing by the heading
int readinInitial(std::string filepath)
{
	PROFILE_SCOPE("readInitial");

	std::string filelabel;
	std::ifstream data(filepath.c_str());
	if (data.is_open())
	{
		std::cout << "Successfully opened the node adjacency data text file. (readinInitial)" << std::endl;
	}
	else
	{
		std::cout << "Unable to open the node adjacency data text file. (readinInitial)" << std::endl;
		return -1;
	}

	std::getline(data, filelabel, '\n');

	if (filelabel == "Adjacency Listing")
	{
		std::cout << "File marked as an Adjacency Listing." << std::endl;
		data.close();
		return 0;
	}
	else if (filelabel == "Adjacency Matrix")
	{
		std::cout << "File marked as an Adjacency Matrix." << std::endl;
		data.close();
		return 1;
	}
	else
	{
		std::cout << "File marking not present or unknown." << std::endl;
		data.close();
		return -1;
	}

}

//counts number of columns in adjacency matrix
int readinMCounter(std::string filepath)
{
	PROFILE_SCOPE("readinMCounter");
	int counter = 0;
	std::string line;
	std::ifstream data(filepath.c_str());
	if (data.is_open())
	{
		std::cout << "Successfully opened the node adjacency data text file. (readinMCounter)" << std::endl;
	}
	else
	{
		std::cout << "Unable to open the node adjacency data text file. (readinMCounter)" << std::endl;
		return -1;
	}
	data.ignore(INT_MAX, '\n'); //ignore the matrix/ listing label
	std::getline(data, line, '\n');
	
	for (unsigned i = 0; i < line.size(); i++)
	{
		if (line[i] == ',')
		{
			counter++;
		}
	}
	//std::cout << "The line: " << line << std::endl; //debug tests
	//std::cout << "Comma count: " << counter << std::endl;
	data.close();

	return counter + 1; //number of elements is one more than number of commas
}

void readinMatrix2D(std::string filepath, int n, std::vector<std::vector<int>>& matrix)
{
	PROFILE_SCOPE("readinMatrix2D");
	
	std::ifstream data(filepath.c_str());
	std::string number;
	if (data.is_open())
	{
		std::cout << "Successfully opened the node adjacency data text file. (readinMatrix2D)" << std::endl;
	}
	else
	{
		std::cout << "Unable to open the node adjacency data text file. (readinMatrix2D)" << std::endl;
		return;
	}

	data.ignore(INT_MAX, '\n'); //ignore the matrix/ listing label

	for (unsigned i = 0; i < (unsigned) n; i++)
	{
		for (unsigned j = 0; j < ((unsigned)n - 1); j++)
		{
			std::getline(data, number, ',');
			matrix[i][j] = std::stoi(number);
		}
		std::getline(data, number, '\n');
		matrix[i][static_cast<__int64>(n) - 1] = std::stoi(number); //Visual Studio said to add that cast to avoid potential overflow
		//matrix[i][n - 1] = std::stoi(number); //original line from ^
	}

	data.close();
	return;
}

void readinMatrix1D(std::string filepath, int n, std::vector<int>& matrix)
{
	PROFILE_SCOPE("readinMatrix1D");

	std::ifstream data(filepath.c_str());
	std::string number;
	if (data.is_open())
	{
		std::cout << "Successfully opened the node adjacency data text file. (readinMatrix1D)" << std::endl;
	}
	else
	{
		std::cout << "Unable to open the node adjacency data text file. (readinMatrix1D)" << std::endl;
		return;
	}

	data.ignore(INT_MAX, '\n'); //ignore the matrix/ listing label

	for (unsigned i = 0; i < (unsigned)n; i++)
	{
		for (unsigned j = 0; j < ((unsigned)n - 1); j++)
		{
			std::getline(data, number, ',');
			matrix[Index2Dto1D(i, j, n)] = std::stoi(number);
		}
		std::getline(data, number, '\n');
		matrix[Index2Dto1D(i, n - 1, n)] = std::stoi(number);
	}

	data.close();
	return;
}

int readInCounter(std::string filepath)
{
	PROFILE_SCOPE("readInCounter");
	//reads an adjacency listing file and returns the number of nodes in the graph
	//assumes connected graph 
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

	data.ignore(INT_MAX, '\n'); //ignore the matrix/ listing label
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
	std::cout << "(readInCounter) Numnodes: " << numnodes << std::endl;
	return numnodes;
}

void readIn2D(std::string filepath, int numnodes, std::vector<std::vector<int>>& AdjacencyMatrixFunction)
{
	PROFILE_SCOPE("readIn2D");
	//reads in values from adjacency listing file and enters them into an adjacency matrix in the form of a 2D vector
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
		std::cout << "Successfully opened the node adjacency data text file. (readIn2D)" << std::endl;
	}
	else
	{
		std::cout << "Unable to open the node adjacency data text file." << std::endl;
		std::cout << "Return to the caller with a blank adjacency matrix." << std::endl;
		return;
	}

	data.ignore(INT_MAX, '\n'); //ignore the matrix/ listing label
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

void readIn1D(std::string filepath, int numnodes, std::vector<int>& AdjacencyMatrixFunction)
{
	PROFILE_SCOPE("readIn1D");
	//reads in values from adjacency listing file and enters them into an adjacency matrix in the form of a 2D vector
	std::string node1, node2;
	int intnode1, intnode2;


	for (int i = 0; i < numnodes; i++)
	{
		for (int j = 0; j < numnodes; j++)
		{
			AdjacencyMatrixFunction[Index2Dto1D(i, j, numnodes)] = 0;
		}
	}

	std::ifstream data(filepath.c_str());
	if (data.is_open())
	{
		std::cout << "Successfully opened the node adjacency data text file. (readIn1D)" << std::endl;
	}
	else
	{
		std::cout << "Unable to open the node adjacency data text file." << std::endl;
		std::cout << "Return to the caller with a blank adjacency matrix." << std::endl;
		return;
	}

	data.ignore(INT_MAX, '\n'); //ignore the matrix/ listing label
	while (!data.eof())
	{
		std::getline(data, node1, ',');
		std::getline(data, node2, '\n');

		intnode1 = std::stoi(node1);
		intnode2 = std::stoi(node2);

		AdjacencyMatrixFunction[Index2Dto1D(intnode1, intnode2, numnodes)] = AdjacencyMatrixFunction[Index2Dto1D(intnode2, intnode1, numnodes)] = 1;
	}

	data.close();

	return;
}

void clearchar(std::string s)
{
	PROFILE_SCOPE("clearchar");
	//pops the last character off of a string
	if (!s.empty())
	{
		s.erase(s.size() - 1);
	}
	return;
}

std::string playerprint(int x)
{
	PROFILE_SCOPE("playerprint");
	//helps determine who's turn it is for print statements in the optimalPlay functions
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

void printmovehistory(int recursiondepth, std::vector<int>& matrix, std::ofstream& outputfile)
{
	PROFILE_SCOPE("printmovehistory");
	//prints the series of nodes visited for a given moves set in the optimalPlay functions
	std::cout << matrix[0];
	outputfile << matrix[0];
	for (int i = 1; i <= recursiondepth; i++)
	{
		std::cout << "->" << matrix[i];
		outputfile << "->" << matrix[i];
	}
	return;
}

int optimalPlayMACLoud(int currentnode, int numnodes, std::vector<std::vector<int>>& AdjacencyMatrix, std::vector<std::vector<int>>& edgeusageMatrix, std::vector<int>& nodeusageMatrix, std::vector<int>& movehistory, std::string indentation, int whoseturn, int recursiondepth, std::ofstream& outputfile)
{
	{
		PROFILE_SCOPE("optimalPlayMACLoud");
		//determines the winner of the Make-A-Cycle game on a given graph, prints out move set and saves it to a file as it goes
		indentation += "\t";
		std::cout << indentation << playerprint(whoseturn) << "Just reached node " << currentnode << std::endl; //write to console
		outputfile << indentation << playerprint(whoseturn) << "Just reached node " << currentnode << std::endl; //write to output file
		movehistory[recursiondepth] = currentnode;
		int edgecounter = 0; //keeps track of number unused edges connected to the current node
		int result = 0;
		{
			PROFILE_SCOPE("Cycle Loop");
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
						std::cout << "->" << i; //the program doesn't actually "move" to these nodes,
						outputfile << "->" << i; //so we add them to the output file/ console manually
						std::cout << std::endl;
						outputfile << std::endl;
						clearchar(indentation);
						return 1; //if it's a winning move we're done here
					}
					std::cout << indentation << playerprint(whoseturn) << "No cycle detected." << std::endl;
					outputfile << indentation << playerprint(whoseturn) << "No cycle detected." << std::endl;
				}
			}
		}
		{
			PROFILE_SCOPE("No Moves");
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
		}
		{
			PROFILE_SCOPE("Recursion all moves");
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
					result = optimalPlayMACLoud(i, numnodes, AdjacencyMatrix, edgeusageMatrix, nodeusageMatrix, movehistory, indentation, (whoseturn + 1) % 2, recursiondepth + 1, outputfile);
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
		}
		std::cout << indentation << playerprint(whoseturn) << "There are no good moves here, the game is in a loss state at node " << currentnode << "." << std::endl;
		outputfile << indentation << playerprint(whoseturn) << "There are no good moves here, the game is in a loss state at node " << currentnode << "." << std::endl;
		clearchar(indentation);
		return 0; //no moves to place the game in a Loss state? Then this is a loss state
	}
}

int optimalPlayAACLoud(int currentnode, int numnodes, std::vector<std::vector<int>>& AdjacencyMatrix, std::vector<std::vector<int>>& edgeusageMatrix, std::vector<int>& nodeusageMatrix, std::vector<int>& movehistory, std::string indentation, int whoseturn, int recursiondepth, std::ofstream& outputfile)
{
	PROFILE_SCOPE("optimalPlayAACLoud");
	//determines the winner of the Avoid-A-Cycle game on a given graph, prints out move set and saves it to a file as it goes
	indentation += "\t";
	std::cout << indentation << playerprint(whoseturn) << "Just reached node " << currentnode << std::endl;
	outputfile << indentation << playerprint(whoseturn) << "Just reached node " << currentnode << std::endl; //write to output file
	int edgecounter = 0; //keeps track of number unused edges connected to the current node
	int result = 0;
	int nocycle = 0;

	for (int i = 0; i < numnodes; i++) //for all possible edges connected to this node in the adjacency matrix
	{
		if ((AdjacencyMatrix[currentnode][i] == 1) && (edgeusageMatrix[currentnode][i] == 0)) //go through all the node's existing, unused edges
		{
			std::cout << indentation << playerprint(whoseturn) << "Checking the play from node " << currentnode << " to " << i << "." << std::endl;
			outputfile << indentation << playerprint(whoseturn) << "Checking the play from node " << currentnode << " to " << i << "." << std::endl;
			edgecounter++;

			if (nodeusageMatrix[i] == 1) //if it's been previously visited, going back creates a cycle!
			{
				std::cout << indentation << playerprint(whoseturn) << "Cycle detected, don't want to go here." << std::endl;
				outputfile << indentation << playerprint(whoseturn) << "Cycle detected, don't want to go here." << std::endl;
			}
			else //no cycle 
			{
				std::cout << indentation << playerprint(whoseturn) << "No immediate cycle detected." << std::endl;
				outputfile << indentation << playerprint(whoseturn) << "No immediate cycle detected." << std::endl;
				std::cout << indentation << playerprint(whoseturn) << "What if we play from " << currentnode << " to " << i << "?" << std::endl;
				outputfile << indentation << playerprint(whoseturn) << "What if we play from " << currentnode << " to " << i << "?" << std::endl;
				edgeusageMatrix[currentnode][i] = edgeusageMatrix[i][currentnode] = 1; //add the edge to the graph
				nodeusageMatrix[i] = 1;
				result = optimalPlayAACLoud(i, numnodes, AdjacencyMatrix, edgeusageMatrix, nodeusageMatrix, movehistory, indentation, (whoseturn + 1) % 2, recursiondepth + 1, outputfile);
				std::cout << indentation << playerprint(whoseturn) << "Playing from " << currentnode << " to " << i << " results in a " << (result + 1) % 2 << ".";
				outputfile << indentation << playerprint(whoseturn) << "Playing from " << currentnode << " to " << i << " results in a " << (result + 1) % 2 << ".";
				std::cout << " Move History: ";
				outputfile << " Move History: ";
				printmovehistory(recursiondepth, movehistory, outputfile);
				std::cout << "->" << i; //the program has already returned and "unvisited" these nodes,
				outputfile << "->" << i; //so we add them to the output file/ console manually
				std::cout << std::endl;
				outputfile << std::endl;
				nodeusageMatrix[i] = 0; //reset the node's visited status now that we're returning
				edgeusageMatrix[currentnode][i] = edgeusageMatrix[i][currentnode] = 0; //reset the edge's usage status now that we're returning	
				if (result == 0)
				{
					clearchar(indentation);
					return 1;
				}
			}
		}
	}
	if (edgecounter == 0) //if there's no legal moves the game is in a Loss state
	{
		std::cout << indentation << playerprint(whoseturn) << "No valid moves remaining";
		outputfile << indentation << playerprint(whoseturn) << "No valid moves remaining";
		std::cout << " Move History: ";
		outputfile << " Move History: ";
		printmovehistory(recursiondepth, movehistory, outputfile);
		std::cout << std::endl;
		outputfile << std::endl;
		clearchar(indentation);
		return 0;
	}
	std::cout << indentation << playerprint(whoseturn) << "No winning moves at this state.";
	outputfile << indentation << playerprint(whoseturn) << "No winning moves at this state.";
	std::cout << " Move History: ";
	outputfile << " Move History: ";
	printmovehistory(recursiondepth, movehistory, outputfile);
	std::cout << std::endl;
	outputfile << std::endl;
	clearchar(indentation);
	return 0; //no winning moves detected if we haven't returned at this point
}

int optimalPlayMACQuiet(int currentnode, int numnodes, std::vector<std::vector<int>>& AdjacencyMatrix, std::vector<std::vector<int>>& edgeusageMatrix, std::vector<int>& nodeusageMatrix)
{
	{
		PROFILE_SCOPE("optimalPlayMACQuiet");
		//determines the winner of the Make-A-Cycle game on a given graph, prints out move set and saves it to a file as it goes
		int edgecounter = 0; //keeps track of number unused edges connected to the current node
		int result = 0;

		{
			PROFILE_SCOPE("Cycle Loop");
			for (int i = 0; i < numnodes; i++) //for all possible edges connected to this node in the adjacency matrix
			{
				if ((AdjacencyMatrix[currentnode][i] == 1) && (edgeusageMatrix[currentnode][i] == 0)) //go through all the node's existing, unused edges
				{
					edgecounter++;
					if (nodeusageMatrix[i] == 1) //if it's been previously visited, going back creates a cycle!
					{
						return 1; //if it's a winning move we're done here
					}
				}
			}
		}
		{
			PROFILE_SCOPE("No Moves");
			if (edgecounter == 0) //if there's no legal moves the game is in a Loss state
			{
				return 0;
			}
		}
		{
			PROFILE_SCOPE("Recursion all moves");
			for (int i = 0; i < numnodes; i++) //for all possible edges connected to this node in the adjacency matrix, scope issues here?
			{
				if ((AdjacencyMatrix[currentnode][i] == 1) && (edgeusageMatrix[currentnode][i] == 0)) //go through all the node's existing, unused edges
				{
					edgeusageMatrix[currentnode][i] = edgeusageMatrix[i][currentnode] = 1; //add the edge to the graph
					nodeusageMatrix[i] = 1;
					result = optimalPlayMACQuiet(i, numnodes, AdjacencyMatrix, edgeusageMatrix, nodeusageMatrix);
					nodeusageMatrix[i] = 0; //reset the node's visited status now that we're returning
					edgeusageMatrix[currentnode][i] = edgeusageMatrix[i][currentnode] = 0; //reset the edge's usage status now that we're returning	
					if (result == 0) //if the move puts the game in a loss state for the other player, it's a win state for us
					{
						return 1;
					}
				}
			}
		}
	}
	return 0; //no moves to place the game in a Loss state? Then this is a loss state
}

int optimalPlayAACQuiet(int currentnode, int numnodes, std::vector<std::vector<int>>& AdjacencyMatrix, std::vector<std::vector<int>>& edgeusageMatrix, std::vector<int>& nodeusageMatrix)
{
	PROFILE_SCOPE("optimalPlayAACQuiet");
	//determines the winner of the Avoid-A-Cycle game on a given graph, prints out move set and saves it to a file as it goes
	int edgecounter = 0; //keeps track of number unused edges connected to the current node
	int result = 0;
	int nocycle = 0;

	for (int i = 0; i < numnodes; i++) //for all possible edges connected to this node in the adjacency matrix
	{
		if ((AdjacencyMatrix[currentnode][i] == 1) && (edgeusageMatrix[currentnode][i] == 0)) //go through all the node's existing, unused edges
		{
			edgecounter++;

			if (nodeusageMatrix[i] == 1) //if it's been previously visited, going back creates a cycle!
			{
				;
			}
			else //no cycle 
			{
				edgeusageMatrix[currentnode][i] = edgeusageMatrix[i][currentnode] = 1; //add the edge to the graph
				nodeusageMatrix[i] = 1;
				result = optimalPlayAACQuiet(i, numnodes, AdjacencyMatrix, edgeusageMatrix, nodeusageMatrix);
				nodeusageMatrix[i] = 0; //reset the node's visited status now that we're returning
				edgeusageMatrix[currentnode][i] = edgeusageMatrix[i][currentnode] = 0; //reset the edge's usage status now that we're returning	
				if (result == 0)
				{
					return 1;
				}
			}
		}
	}
	if (edgecounter == 0) //if there's no legal moves the game is in a Loss state
	{
		return 0;
	}
	return 0; //no winning moves detected if we haven't returned at this point
}

int optimalPlayMACQuietNew(int currentnode, int numnodes, std::vector<int>& AdjacencyMatrix, std::vector<int>& edgeusageMatrix, std::vector<int>& nodeusageMatrix)
{
	{
		PROFILE_SCOPE("optimalPlayMACQuiet");
		//determines the winner of the Make-A-Cycle game on a given graph, prints out move set and saves it to a file as it goes
		int edgecounter = 0; //keeps track of number unused edges connected to the current node
		int result = 0;

		{
			PROFILE_SCOPE("Cycle Loop");
			for (int i = 0; i < numnodes; i++) //for all possible edges connected to this node in the adjacency matrix
			{
				if ((AdjacencyMatrix[Index2Dto1D(currentnode, i, numnodes)] == 1) && (edgeusageMatrix[Index2Dto1D(currentnode, i, numnodes)] == 0)) //go through all the node's existing, unused edges
				{
					edgecounter++;
					if (nodeusageMatrix[i] == 1) //if it's been previously visited, going back creates a cycle!
					{
						return 1; //if it's a winning move we're done here
					}
				}
			}
		}
		{
			PROFILE_SCOPE("No Moves");
			if (edgecounter == 0) //if there's no legal moves the game is in a Loss state
			{
				return 0;
			}
		}
		{
			PROFILE_SCOPE("Recursion all moves");
			for (int i = 0; i < numnodes; i++) //for all possible edges connected to this node in the adjacency matrix, scope issues here?
			{
				if ((AdjacencyMatrix[Index2Dto1D(currentnode, i, numnodes)] == 1) && (edgeusageMatrix[Index2Dto1D(currentnode, i, numnodes)] == 0)) //go through all the node's existing, unused edges
				{
					edgeusageMatrix[Index2Dto1D(currentnode, i, numnodes)] = edgeusageMatrix[Index2Dto1D(i, currentnode, numnodes)] = 1; //add the edge to the graph
					nodeusageMatrix[i] = 1;
					result = optimalPlayMACQuietNew(i, numnodes, AdjacencyMatrix, edgeusageMatrix, nodeusageMatrix);
					nodeusageMatrix[i] = 0; //reset the node's visited status now that we're returning
					edgeusageMatrix[Index2Dto1D(currentnode, i, numnodes)] = edgeusageMatrix[Index2Dto1D(i, currentnode, numnodes)] = 0; //reset the edge's usage status now that we're returning	
					if (result == 0) //if the move puts the game in a loss state for the other player, it's a win state for us
					{
						return 1;
					}
				}
			}
		}
	}
	return 0; //no moves to place the game in a Loss state? Then this is a loss state
}

int optimalPlayAACQuietNew(int currentnode, int numnodes, std::vector<int>& AdjacencyMatrix, std::vector<int>& edgeusageMatrix, std::vector<int>& nodeusageMatrix)
{
	PROFILE_SCOPE("optimalPlayAACQuiet");
	//determines the winner of the Avoid-A-Cycle game on a given graph, prints out move set and saves it to a file as it goes
	int edgecounter = 0; //keeps track of number unused edges connected to the current node
	int result = 0;
	int nocycle = 0;

	for (int i = 0; i < numnodes; i++) //for all possible edges connected to this node in the adjacency matrix
	{
		
		if ((AdjacencyMatrix[Index2Dto1D(currentnode, i, numnodes)] == 1) && (edgeusageMatrix[Index2Dto1D(currentnode, i, numnodes)] == 0)) //go through all the node's existing, unused edges
		{
			edgecounter++;

			if (nodeusageMatrix[i] == 1) //if it's been previously visited, going back creates a cycle!
			{
				;
			}
			else //no cycle 
			{
				edgeusageMatrix[Index2Dto1D(currentnode, i, numnodes)] = edgeusageMatrix[Index2Dto1D(i, currentnode, numnodes)] = 1; //add the edge to the graph
				nodeusageMatrix[i] = 1;
				result = optimalPlayAACQuietNew(i, numnodes, AdjacencyMatrix, edgeusageMatrix, nodeusageMatrix);
				nodeusageMatrix[i] = 0; //reset the node's visited status now that we're returning
				edgeusageMatrix[Index2Dto1D(currentnode, i, numnodes)] = edgeusageMatrix[Index2Dto1D(i, currentnode, numnodes)] = 0; //reset the edge's usage status now that we're returning	
				if (result == 0)
				{
					return 1;
				}
			}
		}
	}
	if (edgecounter == 0) //if there's no legal moves the game is in a Loss state
	{
		return 0;
	}
	return 0; //no winning moves detected if we haven't returned at this point
}

void nTunnelDihedralGeneration(std::ofstream& outputfile, int n, int m)
{
	PROFILE_SCOPE("nTunnelDihedralGeneration");
	outputfile << "Adjacency Listing" << std::endl;
	//generates an adjacency listing file for Dihedral/ Dihedral Tunnel graphs
	for (int i = 0; i < m; i++) //how deep
	{
		for (int j = 0; j < n - 1; j++) // how many around
		{
			outputfile << j + (i * n) << ',' << j + 1 + (i * n) << std::endl;
		}
		outputfile << n - 1 + (i * n) << ',' << 0 + (i * n);
		if (i != (m - 1)) //don't want to place an std::endl char at the end of the file, messes with std::stoi later on
		{
			outputfile << std::endl;
		}
		if (i < m - 1)
		{
			for (int k = 0; k < n; k++)
			{
				outputfile << k + (i * n) << ',' << k + ((i + 1) * n) << std::endl;
			}
		}
	}
	return;
}

unsigned tupleDifference(Tuple<unsigned> tuple1, Tuple<unsigned> tuple2)
{
	PROFILE_SCOPE("tupleDifference");
	unsigned difference = 0;
	if (tuple1.getSize() != tuple2.getSize())
	{
		std::cout << "Tuples are of unequal size and can't be compared." << std::endl;
		return 0;
	}

	for (unsigned i = 0; i < tuple1.getSize(); i++)
	{
		difference += std::abs((int)tuple1.getEntry(i) - (int)tuple2.getEntry(i)); //get the accumulated difference, entry by entry
	}
	return difference;
}

void zmnGeneration(std::vector<Tuple<unsigned>>& directProduct, std::vector<unsigned>& valueholder, unsigned placeinTuple, unsigned& placeinVector, unsigned m, unsigned n)
{
	PROFILE_SCOPE("zmnGeneration");
	//initial call would be zmnGeneration(directProduct,0,0,m,n)

	if (placeinTuple < n - 1)
	{
		for (unsigned i = 0; i < m; i++)
		{
			valueholder[placeinTuple] = i;
			zmnGeneration(directProduct, valueholder, placeinTuple + 1, placeinVector, m, n);
		}
	}
	else
	{
		for (unsigned i = 0; i < m; i++)
		{
			valueholder[placeinTuple] = i; //increment the last value in the tuple (we're iterating through all of them)
			//std::cout << "(";
			for (unsigned j = 0; j < valueholder.size(); j++)
			{
				directProduct[placeinVector].setEntry(j, valueholder[j]); //assign the entries of the tuple one by one
				//std::cout << valueholder[j] << ",";
			}
			//std::cout << ")" << std::endl; //
			//std::cout << "Place in vector = " << placeinVector << std::endl;
			placeinVector++; //move to the next empty storage spot
		}
	}
	return;
}

void zmnAdjacencyGeneration(std::ofstream& outputfile, unsigned m, unsigned n)
{
	PROFILE_SCOPE("zmnAdjacencyGeneration");
	//call zmnGeneration
	//loop through resutling directProduct vector
		//compare each element to the others, if distance is 1-> adjacent, otherwise not
		//write into 2D vector(i,j and j,i), and then output to file 

	//need to make placeinVector a global variable so that its value is preserved throughout the recursive calls->pass by reference?

	unsigned placeinVector = 0;
	std::vector<unsigned> valueholder(n);
	std::vector<Tuple<unsigned>> directProduct((unsigned)std::pow(m, n), n);
	std::vector<std::vector<unsigned>> matrix((unsigned)std::pow(m, n));
	for (unsigned i = 0; i < (unsigned)std::pow(m, n); i++)
	{
		matrix[i] = std::vector<unsigned>((unsigned)std::pow(m, n)); //make matrix a 2D vector
	}

	zmnGeneration(directProduct, valueholder, 0, placeinVector, m, n); //generate all the elements of (Z_m)^n, placed into directProduct vector

	std::cout << "Testing Time!" << std::endl;
	for (unsigned i = 0; i < (unsigned)std::pow(m, n); i++)
	{
		directProduct[i].printTuple();
		std::cout << std::endl;
	}

	//iterate through all combinations to figure out adjacency matrix
	for (unsigned i = 0; i < (unsigned)std::pow(m, n); i++) //tighter bound here?
	{
		for (unsigned j = i; j < (unsigned)std::pow(m, n); j++)
		{
			if (tupleDifference(directProduct[i], directProduct[j]) == 1)
			{
				matrix[i][j] = matrix[j][i] = 1;
			}
			else
			{
				matrix[i][j] = matrix[j][i] = 0;
			}
		}
	}

	outputfile << "Adjacency Matrix" << std::endl;
	//output adjacency matrix to .txt file
	for (unsigned i = 0; i < (unsigned)std::pow(m, n); i++)
	{
		for (unsigned j = 0; j < (unsigned)std::pow(m, n); j++)
		{
			outputfile << matrix[i][j];
			if (j < ((unsigned)std::pow(m, n) - 1)) //if we're not at the last entry in a line
			{
				outputfile << ',';
			}
			else if (i < ((unsigned)std::pow(m, n) - 1)) //if we're not at the end of the line
			{
				outputfile << std::endl;
			}
		}
	}

	return;
}

void GeneralizedPetersenNMGeneration(std::ofstream& outputfile, unsigned n, unsigned k)
{
	outputfile << "Adjacency Listing" << std::endl;
	for (unsigned i = 0; i < n; i++)
	{
		outputfile << i << ',' << (i + 1) % n << std::endl;
	}
	for (unsigned i = 0; i < n; i++)
	{
		outputfile << i << ',' << i + n << std::endl;
	}
	for (unsigned i = n; i < ((2 * n) - 1); i++)
	{
		outputfile << i << ',' << ((i + k) % n) + n << std::endl;
	}
	outputfile << ((2 * n) - 1) << ',' << ((((2 * n) - 1) + k) % n) + n;

	return;
}

int main()
{
	Instrumentor::Get().BeginSession("Profile");
	//PROFILE_FUNCTION();
	//driver function

	//ThreadPool::ThreadPool pool(5);


	int taskselection;
	std::cout << "Do you want to play a game or generate an adjacency listing/matrix?" << std::endl;
	std::cout << "[0] Play a game." << std::endl;
	std::cout << "[1] Generate an adjacency listing." << std::endl;

	std::cin >> taskselection;
	std::cin.clear();
	std::cin.ignore(INT_MAX, '\n');
	while (!((taskselection == 0) || (taskselection == 1)))
	{
		std::cout << "Invalid selection number! Please enter a \"0\" to play a game or a \"1\" to generate an adjacency listing." << std::endl;
		std::cin >> taskselection;
		std::cin.clear();
		std::cin.ignore(INT_MAX, '\n');
	}

	if (taskselection == 1) //Generating an adjacency listing
	{
		int generationselection;
		int generationselection2;
		int generationselection3;

		std::cout << "What type of graph would you like to generate an adjacency listing for?" << std::endl;
		std::cout << "[0] Dihedral Graph D_n." << std::endl;
		std::cout << "[1] Dihedral Tunnel Graph D_n - m." << std::endl;
		std::cout << "[2] (Z_m)^n Direct Product Graph." << std::endl;
		std::cout << "[3] Generalized Petersen GP(n,k) Graph." << std::endl;

		std::cin >> generationselection;
		std::cin.clear();
		std::cin.ignore(INT_MAX, '\n');
		while (!((generationselection == 0) || (generationselection == 1) || (generationselection == 2) || (generationselection == 3)))
		{
			std::cout << "Invalid selection number! Please enter a number that corresponds to one of the following options:" << std::endl;
			std::cout << "[0] Dihedral Graph D_n." << std::endl;
			std::cout << "[1] Dihedral Tunnel Graph D_n - m." << std::endl;
			std::cout << "[2] (Z_m)^n Direct Product Graph." << std::endl;
			std::cout << "[3] Generalized Petersen GP(n,k) Graph." << std::endl;
			std::cin >> generationselection;
			std::cin.clear();
			std::cin.ignore(INT_MAX, '\n');
		}

		std::string generationfilepathout = "C:\\Users\\willl\\Desktop\\2021 Summer Research\\Adjacency Info\\Computer Generated\\";
		std::string outputfilename;
		std::cout << "What do you want to name the output file?" << std::endl;
		std::cin >> outputfilename;
		std::cin.clear();
		std::cin.ignore(INT_MAX, '\n');
		outputfilename.append(".txt");
		generationfilepathout.append(outputfilename);
		std::ofstream outputfile;
		outputfile.open(generationfilepathout.c_str());

		if (outputfile.is_open())
		{
			std::cout << "Successfully opened the output text file to store the adjacency listing/ matrix." << std::endl;
		}
		else
		{
			std::cout << "Unable to open the output text file to store the adjacency listing/ matrix." << std::endl;
			std::cout << "The code will continue to run but no results will be saved." << std::endl;
		}

		if (generationselection == 0)
		{
			std::cout << "You've selected to generate a Dihedral graph adjacency listing. What size would you like? (3-100)" << std::endl;
			generationselection = -1;
			std::cin >> generationselection;
			std::cin.clear();
			std::cin.ignore(INT_MAX, '\n');
			while (generationselection < 3 || generationselection > 100)
			{
				std::cout << "Invalid selection number! Please enter a number in the range [3,100]." << std::endl;
				std::cin >> generationselection;
				std::cin.clear();
				std::cin.ignore(INT_MAX, '\n');
			}

			nTunnelDihedralGeneration(outputfile, generationselection, 2);
		}
		else if (generationselection == 1)
		{
			std::cout << "You've selected to generate a Dihedral Tunnel graph adjacency listing." << std::endl; 
			std::cout << "What size would you like the graph to be circumferentially? (i.e. the \"n\" in D_n - m) (3 - 100)" << std::endl;
			
			generationselection = -1;
			std::cin >> generationselection;
			std::cin.clear();
			std::cin.ignore(INT_MAX, '\n');
			while (generationselection < 3 || generationselection > 100)
			{
				std::cout << "Invalid selection number! Please enter a number in the range [3,100]." << std::endl;
				std::cin >> generationselection;
				std::cin.clear();
				std::cin.ignore(INT_MAX, '\n');
			}

			std::cout << "How deep would you like the graph be? (i.e. the \"m\" in D_n - m) (3 - 100)" << std::endl;

			generationselection2 = -1;
			std::cin >> generationselection2;
			std::cin.clear();
			std::cin.ignore(INT_MAX, '\n');
			while (generationselection2 < 3 || generationselection2 > 100)
			{
				std::cout << "Invalid selection number! Please enter a number in the range [3,100]." << std::endl;
				std::cin >> generationselection2;
				std::cin.clear();
				std::cin.ignore(INT_MAX, '\n');
			}

			nTunnelDihedralGeneration(outputfile, generationselection, generationselection2);
		}
		else if (generationselection == 2)
		{
			std::cout << "You've selected to generate a (Z_m)^n Direct Product Graph." << std::endl;
			std::cout << "Please enter the neccessary parameters:" << std::endl << "(In the range [2,100])" << std::endl << "m:" << std::endl;
			generationselection2 = -1;
			std::cin >> generationselection2;
			std::cin.clear();
			std::cin.ignore(INT_MAX, '\n');
			while (generationselection2 < 2 || generationselection2 > 100)
			{
				std::cout << "Invalid selection number! Please enter a number in the range [2,100]." << std::endl;
				std::cin >> generationselection2;
				std::cin.clear();
				std::cin.ignore(INT_MAX, '\n');
			}

			std::cout << "(In the range [1,100])" << std::endl << "n:" << std::endl;

			generationselection3 = -1;
			std::cin >> generationselection3;
			std::cin.clear();
			std::cin.ignore(INT_MAX, '\n');
			while (generationselection3 < 1 || generationselection3 > 100)
			{
				std::cout << "Invalid selection number! Please enter a number in the range [1,100]." << std::endl;
				std::cin >> generationselection3;
				std::cin.clear();
				std::cin.ignore(INT_MAX, '\n');
			}

			//std::vector<unsigned> valueholder(generationselection3);
			//std::vector<Tuple<unsigned>> directProduct((unsigned) std::pow(generationselection2, generationselection3));
			//std::vector<Tuple<unsigned>> directProduct((unsigned)std::pow(generationselection2, generationselection3), generationselection3);

			//zmnGeneration(directProduct, valueholder, 0, 0, generationselection2, generationselection3);
			//void zmnAdjacencyGeneration(std::ofstream & outputfile, std::vector<Tuple<unsigned>>&directProduct, unsigned m, unsigned n)
			zmnAdjacencyGeneration(outputfile, generationselection2, generationselection3);
		}
		else if (generationselection == 3)
		{
			std::cout << "You've selected to generate a Generalized Petersen GP(n,k) Graph." << std::endl;
			std::cout << "Please enter the neccessary parameters:" << std::endl << "(In the range [3,100])" << std::endl << "n:" << std::endl;
			generationselection2 = -1;
			std::cin >> generationselection2;
			std::cin.clear();
			std::cin.ignore(INT_MAX, '\n');
			while (generationselection2 < 2 || generationselection2 > 100)
			{
				std::cout << "Invalid selection number! Please enter a number in the range [2,100]." << std::endl;
				std::cin >> generationselection2;
				std::cin.clear();
				std::cin.ignore(INT_MAX, '\n');
			}

			std::cout << "(In the range[1,n/2])." << std::endl << "k:" << std::endl;

			generationselection3 = -1;
			std::cin >> generationselection3;
			std::cin.clear();
			std::cin.ignore(INT_MAX, '\n');
			while (generationselection3 < 1 || generationselection3 > (generationselection2/2) )
			{
				std::cout << "Invalid selection number! Please enter a number in the range [1,n/2]." << std::endl;
				std::cin >> generationselection3;
				std::cin.clear();
				std::cin.ignore(INT_MAX, '\n');
			}

			GeneralizedPetersenNMGeneration(outputfile, generationselection2, generationselection3);
		}

		std::cout << "Finished generating the adjacency listing/ matrix." << std::endl;
		std::cout << "The .txt file holding the adjacency listing/ matrix can be found at:" << std::endl;
		std::cout << generationfilepathout << std::endl;

		outputfile.close();
	}

	else if (taskselection == 0) //Playing the game
	{
		//Output selection here
		int numnodes;
		int outputselection;
		std::cout << "Would you like the results of this code outputted to the console and saved to a .txt file, or do you want to just run the game?" << std::endl;
		std::cout << "[0] Full Output" << std::endl;
		std::cout << "[1] Quiet Run" << std::endl;
		std::cin >> outputselection;
		std::cin.clear();
		std::cin.ignore(INT_MAX, '\n');
		while (!((outputselection == 0) || (outputselection == 1)))
		{
			std::cout << "Invalid selection number! Please enter a \"0\" for a Full Output or a \"1\" for a Quiet Run." << std::endl;
			std::cin >> outputselection;
			std::cin.clear();
			std::cin.ignore(INT_MAX, '\n');
		}

		//{Game selection (MAC vs. AAC) here
		int gameselection;
		std::cout << "What game would you like to play?" << std::endl;
		std::cout << "[0] Make-A-Cycle (REL)" << std::endl;
		std::cout << "[1] Avoid-A-Cycle (RAV)" << std::endl;

		std::cin >> gameselection;
		std::cin.clear();
		std::cin.ignore(INT_MAX, '\n');
		while (!((gameselection == 0) || (gameselection == 1)))
		{
			std::cout << "Invalid selection number! Please enter a \"0\" for the Make-A-Cycle (REL) game or a \"1\" for the Avoid-A-Cycle (RAV) game." << std::endl;
			std::cin >> gameselection;
			std::cin.clear();
			std::cin.ignore(INT_MAX, '\n');
		}

		if (gameselection == 0)
		{
			std::cout << "Selected the Make-A-Cycle (REL) game." << std::endl;
		}
		else if (gameselection == 1)
		{
			std::cout << "Selected the Avoid-A-Cycle (RAV) game." << std::endl;
		}

		//}//end of game selection

		//{Graph selection here
		int startingnode = -1;
		int graphselection;
		std::cout << "What graph would you like to play on? The pre-loaded graphs are:" << std::endl; 
		std::string selectionpath = "C:\\Users\\willl\\Desktop\\2021 Summer Research\\Adjacency Info\\"; //put this up top with a #DEFINE perhaps?
		std::string tempstring;
		int filecounter = 0;
		int fileselector = 0;

		//way to strip off directory and just have file name printed???
		for (const auto& entry : std::filesystem::directory_iterator(selectionpath)) //wtf?
		{
			if (entry.is_regular_file()) //don't want to list out folders as choices
			{
				tempstring = std::filesystem::absolute(entry.path()).string(); //all this nonsense just makes it that the file name is printed instead of the whole file path
				tempstring.erase(0, selectionpath.length()); //(makes it easier to read^)
				std::cout << "[" << filecounter << "] " << tempstring << std::endl;
				filecounter++;
			}
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
			if (entry.is_regular_file()) //again ignoring folders since they're not files...
			{
				if (fileselector == graphselection)
				{
					selectionpath = std::filesystem::absolute(entry.path()).string();
					break;
				}
				fileselector++;
			}
			
		}

		std::cout<<"Chosen file path:"<<std::endl; //debug print check for file selection
		std::cout<<selectionpath<<std::endl;

		//} end of graph selection block

		//{Some initialization business (DON'T MOVE THIS BLOCK)

		int whichone = readinInitial(selectionpath);
		if (whichone == 0)
		{
			std::cout << "The selected file contains an adjacency listing." << std::endl;
			numnodes = readInCounter(selectionpath);
		}
		else if (whichone == 1)
		{
			std::cout << "The selected file contains an adjacency matrix." << std::endl;
			numnodes = readinMCounter(selectionpath);
		}
		else
		{
			std::cout << "Something broke while reading in the file, killing the program now." << std::endl;
			return -1;
		}

		//initializing stuff
		std::string indentation, outputfilename;
		int recursiondepth = 0;
		int whoseturn = 0;

		//would throw this inside an if block but VS doesn't like that for some reason
		//2-Dimensional vectors
		std::vector<std::vector<int>> AdjacencyMatrix(numnodes);
		std::vector<std::vector<int>> edgeusageMatrix(numnodes);
		std::vector<int> nodeusageMatrix(numnodes);
		std::vector<int> movehistory(numnodes);

		for (int i = 0; i < numnodes; i++)
		{
			AdjacencyMatrix[i] = std::vector<int>(numnodes);
			edgeusageMatrix[i] = std::vector<int>(numnodes);
		}

		for (int i = 0; i < numnodes; i++)
		{
			for (int j = 0; j < numnodes; j++)
			{
				AdjacencyMatrix[i][j] = AdjacencyMatrix[j][i] = 0;
				edgeusageMatrix[i][j] = edgeusageMatrix[j][i] = 0;
			}
		}
		for (int i = 0; i < numnodes; i++)
		{
			nodeusageMatrix[i] = 0;
		}

		//1-Dimensional vectors, might avoid some cache misses???
		std::vector<int> AdjacencyMatrix1D(std::pow(numnodes, 2));
		std::vector<int> edgeusageMatrix1D(std::pow(numnodes, 2));

		for (unsigned i = 0; i < std::pow(numnodes, 2); i++)
		{
			AdjacencyMatrix1D[i] = 0;
			edgeusageMatrix1D[i] = 0;
		}
		//} End of initialization business

		//{Output file stuff
		std::string filepathout = "C:\\Users\\willl\\Desktop\\2021 Summer Research\\Code Output\\";
		if (outputselection == 0)
		{
			std::cout << "What do you want to name the output file?" << std::endl;
			std::cin >> outputfilename;
			std::cin.clear();
			std::cin.ignore(INT_MAX, '\n');
			outputfilename.append(".txt");
			filepathout.append(outputfilename);
		}
		else
		{
			filepathout.append("placeholder.txt");
		}
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

		//Add in 2D vs 1D option here
		if (whichone == 0) //Adjacency Listing File
		{
			if (outputselection == 0) //Full Output (2D vectors)
			{
				readIn2D(selectionpath, numnodes, AdjacencyMatrix); //assigns non-zero values to adjacency matrix
			}
			else if (outputselection == 1) //Quiet Output (1D vectors)
			{
				//readIn2D(selectionpath, numnodes, AdjacencyMatrix); //assigns non-zero values to adjacency matrix
				readIn1D(selectionpath, numnodes, AdjacencyMatrix1D); //assigns non-zero values to adjacency matrix
			}
		}
		else if (whichone == 1) //Adjacency Matrix File
		{
			if (outputselection == 0) //Full Output (2D vectors)
			{
				readinMatrix2D(selectionpath, numnodes, AdjacencyMatrix);
			}
			else if (outputselection == 1) //Quiet Output (1D vectors)
			{
				readinMatrix1D(selectionpath, numnodes, AdjacencyMatrix1D);
			}
		}
		

		std::cout << "Printout of the adjacency matrix for debugging purposes:" << std::endl;
		if (outputselection == 0)
		{
			outputfile << "Printout of the adjacency matrix for debugging purposes:" << std::endl;
		}
		for (int i = 0; i < numnodes; i++)
		{
			for (int j = 0; j < numnodes; j++)
			{
				std::cout << AdjacencyMatrix[i][j] << " ";
				if (outputselection == 0)
				{
					outputfile << AdjacencyMatrix[i][j] << " ";
				}
				
			}
			std::cout << std::endl;
			if (outputselection == 0)
			{
				outputfile << std::endl;
			}
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

		if (outputselection == 0) //full output
		{
			std::cout << "Beginning the game at node " << startingnode << ", on the graph constructed based off of: " << std::endl << selectionpath << std::endl;
			outputfile << "Beginning the game at node " << startingnode << ", on the graph constructed based off of: " << std::endl << selectionpath << std::endl;

			if (gameselection == 0)
			{
				std::cout << "Make-A-Cycle:" << std::endl;
				outputfile << "Make-A-Cycle:" << std::endl;
				std::cout << optimalPlayMACLoud(startingnode, numnodes, AdjacencyMatrix, edgeusageMatrix, nodeusageMatrix, movehistory, indentation, whoseturn, recursiondepth, outputfile) << std::endl;
			}
			else if (gameselection == 1)
			{
				std::cout << "Avoid-A-Cycle:" << std::endl;
				outputfile << "Avoid-A-Cycle:" << std::endl;
				std::cout << optimalPlayAACLoud(startingnode, numnodes, AdjacencyMatrix, edgeusageMatrix, nodeusageMatrix, movehistory, indentation, whoseturn, recursiondepth, outputfile) << std::endl;
			}
		}
		else if (outputselection == 1) //quiet output
		{
			std::cout << "Beginning the game at node " << startingnode << ", on the graph constructed based off of: " << std::endl << selectionpath << std::endl;
			if (gameselection == 0)
			{
				std::cout << "Make-A-Cycle:" << std::endl;
				//std::cout << optimalPlayMACQuiet(startingnode, numnodes, AdjacencyMatrix, edgeusageMatrix, nodeusageMatrix) << std::endl;
				std::cout << optimalPlayMACQuietNew(startingnode, numnodes, AdjacencyMatrix1D, edgeusageMatrix1D, nodeusageMatrix) << std::endl;
			}
			else if (gameselection == 1)
			{
				std::cout << "Avoid-A-Cycle:" << std::endl;
				//std::cout << optimalPlayAACQuiet(startingnode, numnodes, AdjacencyMatrix, edgeusageMatrix, nodeusageMatrix) << std::endl;
				std::cout << optimalPlayAACQuietNew(startingnode, numnodes, AdjacencyMatrix1D, edgeusageMatrix1D, nodeusageMatrix) << std::endl;
			}
		}
		

		outputfile.close();
	}

	//}That's all folks

	Instrumentor::Get().EndSession();
	return 0;
}
