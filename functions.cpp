//CONWAY PROJECT 1.1.0 (Stable)
//F.T 2025

#include <SDL2/SDL.h>
#include <iostream>
#include<stdio.h>
#include "functions.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl2.h"
#include "imgui/imgui_impl_sdlrenderer2.h"
#include "SDL2/SDL_image.h"

//============================== Game Class Constructor ==============================

Simulation::Simulation()
    :m_window(nullptr),
     m_renderer(nullptr),
     m_cellSize(20),
     m_width(1600),
     m_height(900),
     m_rows(m_height/m_cellSize),
     m_columns(m_width/m_cellSize),
     m_generation(0),
     m_gameState(0),
     m_lastTickTime(0),
     m_tickInterval(1000),
     m_grid(),
     m_isRunning(),
     m_hudTextures(),
     m_showConfig(),
     m_cameraX(0),
     m_cameraY(0),
     m_cameraVelX(0),
     m_cameraVelY(0),
     m_Wkeydown(false),
     m_Skeydown(false),
     m_Akeydown(false),
     m_Dkeydown(false)

     //SDL Initialization
    {
        std::cout<<"Terminal Initialized"<<std::endl;
        if(SDL_Init(SDL_INIT_VIDEO)<0)
        {
         std::cout<<"Failed to initialize SDL video. \n SDL Error:"<<SDL_GetError()<<std::endl;
        }
        else
        {
            m_window = SDL_CreateWindow("Conway Project", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,1600,900, 0);
            if(m_window == NULL)
            {

             std::cout<<"Failed to initialize SDL window. \n SDL Error:"<<SDL_GetError()<<std::endl;

            }   
            else
            {
                m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED);
                if(m_renderer == NULL)
                {
                 std::cout<<"Failed to initialize SDL renderer. \n SDL Error:"<<SDL_GetError()<<std::endl;
                }
                else
                {
                 std::cout<<"Simulation Initialized Sucessfully!"<<std::endl;
                }
            }

        }

        resetGrid(m_width, m_height);

        
        //ImGui Initialization
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui::StyleColorsDark();
        ImGui_ImplSDL2_InitForSDLRenderer(m_window, m_renderer);
        ImGui_ImplSDLRenderer2_Init(m_renderer);

        //Texture Loads
        SDL_Texture* pauseTexture = IMG_LoadTexture(m_renderer, "images/Pause.png");
        SDL_Texture* play1xTexture = IMG_LoadTexture(m_renderer, "images/Play1x.png");
        SDL_Texture* play2xTexture = IMG_LoadTexture(m_renderer, "images/Play2x.png");
        SDL_Texture* play4xTexture = IMG_LoadTexture(m_renderer, "images/Play4x.png");
        SDL_Texture* optionsTexture = IMG_LoadTexture(m_renderer, "images/Options.png");
        m_hudTextures = {pauseTexture, play1xTexture, play2xTexture, play4xTexture, optionsTexture};
        //Finalization
        m_isRunning = true;
    }
//============================== Simulation Class Deconstructor ==============================
Simulation::~Simulation()
{
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    
    for(SDL_Texture* texture : m_hudTextures)
    {
        SDL_DestroyTexture(texture);
    }
    m_hudTextures.clear();

    SDL_DestroyRenderer(m_renderer);
    SDL_DestroyWindow(m_window);
    m_renderer = nullptr;
    m_window = nullptr;
    SDL_Quit();

    std::cout<<"Simulation Cleaned Up Sucessfully!"<<std::endl;
}
//============================== Renderization ==============================

void Simulation::render()
{
    //Clear
    SDL_SetRenderDrawColor(m_renderer, 35, 35, 35, 255);
    SDL_RenderClear(m_renderer);
    
    for(int i = 0; i<m_grid.size();i++)
    {
        //Get position from the single "i" (since its a 1D vector storing 2D coordinates)
        int row = i/m_columns;
        int column = i % m_columns;
        SDL_Rect cellRect;
        cellRect.x = (column * m_cellSize)+m_cameraX;
        cellRect.y = (row * m_cellSize)+m_cameraY;
        cellRect.h = m_cellSize;
        cellRect.w = m_cellSize;

        if(m_grid[i].state == 0)
        {
            SDL_SetRenderDrawColor(m_renderer, 255, 255, 255, 255);
            SDL_RenderFillRect(m_renderer, &cellRect);
            SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);
            SDL_RenderDrawRect(m_renderer, &cellRect);
        }
        if(m_grid[i].state == 1)
        {
            SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);
            SDL_RenderFillRect(m_renderer, &cellRect);
            SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);
            SDL_RenderDrawRect(m_renderer, &cellRect);
        }

    }
    ImGui_ImplSDL2_NewFrame();
    ImGui_ImplSDLRenderer2_NewFrame();
    ImGui::NewFrame();

    ImGui::SetNextWindowPos(ImVec2(0,0));
    ImGui::SetNextWindowSize(ImVec2(1600,100));

    ImGui::Begin("Container", nullptr, ImGuiWindowFlags_NoTitleBar);
    if(ImGui::ImageButton("Pause", (ImTextureID)m_hudTextures[0], ImVec2(64, 64)))
    {
        m_gameState = 0; 
    }
    ImGui::SameLine();
    if(ImGui::ImageButton("Play1x", (ImTextureID)m_hudTextures[1], ImVec2(64,64)))
    {
        m_gameState = 1;
        m_tickInterval = 1000;
    }
    ImGui::SameLine();
    if(ImGui::ImageButton("Play2x", (ImTextureID)m_hudTextures[2], ImVec2(64,64)))
    {
        m_gameState = 1;
        m_tickInterval = 500;
    }
    ImGui::SameLine();
    if(ImGui::ImageButton("Play4x", (ImTextureID)m_hudTextures[3], ImVec2(64,64)))
    {
        m_gameState = 1;
        m_tickInterval = 250;
        
    }
    ImGui::SameLine();
    if(ImGui::ImageButton(("Options"), (ImTextureID)m_hudTextures[4],ImVec2(64, 64)))
    {
        m_showConfig = true;
    }
    ImGui::Text("Current Generation: %d", m_generation);
    ImGui::End();

    //Config options
    if(m_showConfig == true)
    {
        ImGui::Begin("Configurations", &m_showConfig);

        static int tempWidth = m_width;
        static int tempHeight = m_height;

        ImGui::InputInt("Grid Width", &tempWidth);
        ImGui::InputInt("Grid Height", &tempHeight);
        if(ImGui::Button("Resize"))
        {
            resetGrid(tempWidth, tempHeight);
        }

        ImGui::Text("Note: Resizing will clear the current simulation.");
        ImGui::End();
    }

    ImGui::Render();
    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), m_renderer);
    
    //Present
    SDL_RenderPresent(m_renderer);
}


void Simulation::resetGrid(int newWidth, int newHeight)
{
    //Grid Data building
    m_width = newWidth;
    m_height = newHeight;
    
    m_rows = newHeight/m_cellSize;
    m_columns =  newWidth/m_cellSize;
    
    m_grid.clear();

        for(int i = 0; i<m_rows; i++)
        { 
            for(int j = 0; j<m_columns; j++)
            {
                Cell currentCell;
                currentCell.state = 0;
                m_grid.push_back(currentCell);
            }
        }
    m_generation = 0;
}
//============================== Simulation Main Loop ==============================
void Simulation::run()
{
    while(m_isRunning)
    {
        processInput();
        tick();
        render();
    }

}

//============================== Input Processing ==============================

void Simulation::processInput()
{
    SDL_Event event;
    ImGuiIO&  io = ImGui::GetIO();
    
    while(SDL_PollEvent(&event))
    {
        ImGui_ImplSDL2_ProcessEvent(&event);
    
        if(event.type == SDL_QUIT)
        {
         m_isRunning = false;
        }
        if(event.type == SDL_MOUSEBUTTONDOWN && !io.WantCaptureMouse)
        {    
            
                    
            int xpos = (event.button.x-m_cameraX)/m_cellSize;
            int ypos = (event.button.y-m_cameraY)/m_cellSize;
                        
            int index = ypos * m_columns + xpos;
            if(index > 0 && index < m_grid.size())
            {
                if(m_grid[index].state == 0)
                {
                 m_grid[index].state = 1;
                }
                else if(m_grid[index].state == 1)
                {
                 m_grid[index].state = 0;
                }  
            }
                   
        }
        if(!io.WantCaptureKeyboard)
        {
            if(event.type == SDL_KEYDOWN)
            {
                switch(event.key.keysym.sym)
                {
                 case SDLK_w: m_Wkeydown = true;
                 break;

                 case SDLK_s: m_Skeydown = true;
                 break;

                 case SDLK_a: m_Akeydown = true;
                 break;

                 case SDLK_d: m_Dkeydown = true;
                 break;

                }
            }
            if(event.type == SDL_KEYUP)
            {
                switch(event.key.keysym.sym)
                {
                 case SDLK_w: m_Wkeydown = false;
                 m_cameraVelY = 0;
                 break;
                 case SDLK_s: m_Skeydown = false;
                 m_cameraVelY = 0;
                 break;
                 case SDLK_a: m_Akeydown = false;
                 m_cameraVelX = 0;
                 break;
                 case SDLK_d: m_Dkeydown = false;
                 m_cameraVelX = 0;
                 break;
                }
            }
            
        }
        
        
    }
}

    

//============================== Simulation ==============================

void Simulation::tick()
{
    //Movement
    if(m_Wkeydown == true)
    {
     
        if(abs(m_cameraVelY)<=1)
        {
         m_cameraVelY += 0.1;
        }
        m_cameraY += m_cameraVelY;
    }
    if(m_Skeydown == true)
    {
        if(abs(m_cameraVelY)<=1)
        {
         m_cameraVelY -= 0.1;
        }
        m_cameraY += m_cameraVelY;
    }
    if(m_Akeydown == true)
    {
        if(abs(m_cameraVelX)<=1)
        {
         m_cameraVelX += 0.1;
        }
        m_cameraX += m_cameraVelX;
    }
    if(m_Dkeydown == true)
    {
        if(abs(m_cameraVelX)<=1)
        {
         m_cameraVelX -= 0.1;
        }
        m_cameraX += m_cameraVelX;
    }


    if(m_gameState == 0)
    {
        return;
    }

    Uint32 currentTime = SDL_GetTicks();

    if(currentTime - m_lastTickTime < m_tickInterval)
    {
        return;
    }

    //Generation
    m_lastTickTime = currentTime;
    m_generation++;

    std::vector<Cell>nextGrid = m_grid;
    //Check 8 neighbors
    for(int i=0;i<m_grid.size();i++)
    {
        int xPos = i % m_columns;
        int yPos = i / m_columns;

        int liveNeighbors = 0;
        for(int yOffSet = -1; yOffSet<=1; yOffSet++)
        {
            for(int xOffSet = -1; xOffSet<=1; xOffSet++)
            {
                if(xOffSet == 0 && yOffSet == 0)
                {
                    continue;
                }
                int neighborPosX = xPos + xOffSet;
                int neighborPosY = yPos + yOffSet;
                if(neighborPosX >= 0 && neighborPosX < m_columns && neighborPosY < m_rows && neighborPosY >=0)
                {
                    int neighborIndex = neighborPosY * m_columns + neighborPosX;
                    liveNeighbors += m_grid[neighborIndex].state; 
                }
            }
            
        }
        int originalState = m_grid[i].state;
        //Game Ruleset
        if(originalState == 1 && (liveNeighbors<2 || liveNeighbors>3))
        {
           nextGrid[i].state = 0;
        }
        else if(originalState == 0 && liveNeighbors == 3)
        {
            nextGrid[i].state = 1;
        }
        

    }
    m_grid = nextGrid;
}

