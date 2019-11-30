#pragma once

#include <ctime>
#include <string>
#include <iostream>

const int rows = 50; // x
const int cols = 50; // y

struct Node
{
	bool IsAlive = false;
	int generation = 0;

	Node()
	{
		IsAlive = false;
		generation = 0;
	}

	Node(bool alive, int gen)
	{
		IsAlive = alive;
		generation = gen;
	}
};

Node Grid[cols][rows];
Node GridCopy[cols][rows];

void RandomizeGrid();
void ResetGrid();
void CalculateGrid();
int CountNeighbours(int x, int y);

void RandomizeGrid()
{
	srand(time(NULL));
	for (int i = 0; i < cols; i++)
	{
		for (int j = 0; j < rows; j++)
		{
		    //Grid[i][j].IsAlive = true;
            //Grid[i][j].generation = 1;
			if (rand() % 2 == 0)
			{
				Grid[i][j].IsAlive = true;
				Grid[i][j].generation = 1;
			}
			else if (rand() % 2 == 1)
			{
				Grid[i][j].IsAlive = false;
			}
		}
	}
}

void ResetGrid()
{
	for (int i = 0; i < cols; i++)
	{
		for (int j = 0; j < rows; j++)
		{
			Grid[i][j].generation = 0;
			Grid[i][j].IsAlive = false;
		}
	}
}

void CalculateGrid()
{
	//Copy Grid
	for (int i = 0; i < cols; i++)
	{
		for (int j = 0; j < rows; j++)
		{
			GridCopy[i][j] = Grid[i][j];
		}
	}

	//Compute Next Based on Grid
	for (int i = 0; i < cols; i++)
	{
		for (int j = 0; j < rows; j++)
		{

			//If not wrap-around
			if (i == 0 || i == cols - 1 || j == 0 || j == rows - 1)
			{
				//Nothing Happens
				Grid[i][j] = GridCopy[i][j];
			}
			else
            {
                //Count Live Neighbours
                int neighbours = 0; // = CountNeighbours(i, j);
                // Check neighbor cells for life
                if (GridCopy[i-1][j+1].IsAlive	==	true)	neighbours++;
                if (GridCopy[i][j+1].IsAlive	==	true)	neighbours++;
                if (GridCopy[i+1][j+1].IsAlive	==	true)	neighbours++;
                if (GridCopy[i-1][j].IsAlive	==	true)	neighbours++;
                if (GridCopy[i+1][j].IsAlive	==	true)	neighbours++;
                if (GridCopy[i-1][j-1].IsAlive	==	true)	neighbours++;
                if (GridCopy[i][j-1].IsAlive	==	true)	neighbours++;
                if (GridCopy[i+1][j-1].IsAlive	==	true)	neighbours++;

                // Rules
                //Loneliness
                if ((GridCopy[i][j].IsAlive == true) && (neighbours <= 1))
                {
                    Grid[i][j].IsAlive = false;
                    Grid[i][j].generation = 0;
                }

                //Statis
                else if ((GridCopy[i][j].IsAlive == true) && ( neighbours == 2 || neighbours == 3))
                {
                    Grid[i][j].IsAlive = true;
                    Grid[i][j].generation += 1;
                }

                //Overcrowding
                else if ((GridCopy[i][j].IsAlive == true) && (neighbours >= 4))
                {
                    Grid[i][j].IsAlive = false;
                    Grid[i][j].generation = 0;
                }

                //Reproduction
                else if ((GridCopy[i][j].IsAlive == false) && (neighbours == 3))
                {
                    Grid[i][j].IsAlive = true;
                    Grid[i][j].generation += 1;
                }
            }
        }
	}
}

int CountNeighbours(int x, int y)
{
	int sum = 0;
	for (int i = -1; i < 2; i++)
	{
		for (int j = -1; j < 2; j++)
		{
			int col = (x + i + cols) % cols;
			int row = (y + j + rows) % rows;

			if (GridCopy[col][row].IsAlive == true)
			{
				sum++;
			}
		}
	}
	return sum-1;
}

void DrawGrid(int width, int height, int xMinCoord, int xMaxCoord, int yMinCoord, int yMaxCoord)
{
    int xMin = width - height;
	int xMax = width;
	int yMin = 0;
	int yMax = height;

	int colOffset = xMaxCoord - xMinCoord;
	int rowOffset = yMaxCoord - yMinCoord;

	int xOffset = (xMax - xMin) / colOffset;
	int yOffset = (yMax - yMin) / rowOffset;
	float offset = 1.0;

	glBegin(GL_QUADS);

	for(int i = 0; i<colOffset; i++)
    {
        int x = i + xMinCoord;
        float xPos =  xMin + i * xOffset;

        for(int j = 0; j<rowOffset; j++)
        {
            int y = j + yMinCoord;
            float yPos =  yMin + j * yOffset;

            if(Grid[x][y].IsAlive == true)
            {
                glColor3f(1.0f, 1.0f, 0.0f);
            }
            else if(Grid[x][y].IsAlive == false)
            {
                glColor3f(1.0f, 0.0f, 1.0f);
            }

            glVertex2f(xPos + offset, yPos + offset);
			glVertex2f(xPos + xOffset - offset, yPos + offset);
			glVertex2f(xPos + xOffset - offset, yPos + yOffset - offset);
			glVertex2f(xPos + offset, yPos + yOffset - offset);

			/*
			glColor3f(1.0f, 0.0f, 1.0f); glVertex2f(xPos + offset, yPos + offset);
			glColor3f(1.0f, 0.5f, 1.0f); glVertex2f(xPos + xOffset - offset, yPos + offset);
			glColor3f(1.0f, 1.0f, 1.0f); glVertex2f(xPos + xOffset - offset, yPos + yOffset - offset);
			glColor3f(1.0f, 0.0f, 1.0f); glVertex2f(xPos + offset, yPos + yOffset - offset);
			*/
        }
    }

	glEnd();
}

void GridNodeSelection(int button, int x, int y)
{
	static int state = -1;

	if(x >= 0 && x <cols  && y >= 0 && y <rows)
    {
        if(button == GLFW_PRESS)
        {
            if(state != -1)
            {
                Grid[x][y].IsAlive = state;
                GridCopy[x][y].IsAlive = state;
            }
            else
            {
                if(Grid[x][y].IsAlive == true)
                {
                    state = 0;
                }
                else if(Grid[x][y].IsAlive == false)
                {
                    state = 1;
                }
            }
        }

        if(button == GLFW_RELEASE)
        {
            state = -1;
        }
    }
}

