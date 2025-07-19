//CONWAY PROJECT 1.0.0 (stable)
//F.T 2025

#ifndef FUNCTIONS_HPP
#define FUNCTIONS_HPP
#include <vector>
#include <SDL2/SDL.h>
struct Cell
{
    int state;
};

class Simulation
{  
    public:
    Simulation();
    ~Simulation();
    void run();

    private:
    void render();
    void processInput();
    void tick();

    SDL_Window *m_window;
    SDL_Renderer *m_renderer;
    bool m_isRunning;

    int m_cellSize;
    int m_width; 
    int m_height;
    int m_columns;  
    int m_rows; 

    int m_generation;
    int m_gameState;
    int m_lastTickTime; 
    int m_tickInterval;

    std::vector<Cell>m_grid;
    std::vector<SDL_Texture*>m_hudTextures;

};



void printVector(std::vector<Cell> &alive);
#endif
