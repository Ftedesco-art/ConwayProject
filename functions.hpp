//CONWAY PROJECT 1.1.0 (Stable)
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
    void resetGrid(int newWidth, int newHeight);

    private:
    void render();
    void processInput();
    void tick();

    SDL_Window *m_window;
    SDL_Renderer *m_renderer;
    bool m_isRunning;
    bool m_showConfig;
    bool m_Wkeydown;
    bool m_Skeydown;
    bool m_Akeydown;
    bool m_Dkeydown;
    int m_cellSize;
    int m_width; 
    int m_height;
    int m_columns;  
    int m_rows; 
    float m_cameraX;
    float m_cameraY;
    float m_cameraVelX;
    float m_cameraVelY;

    int m_generation;
    int m_gameState;
    int m_lastTickTime; 
    int m_tickInterval;
    


    std::vector<Cell>m_grid;
    std::vector<SDL_Texture*>m_hudTextures;

};

#endif