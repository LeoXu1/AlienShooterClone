#include <SDL.h>
#include <vector>
#include <iostream>
#include <string>
#include <SDL_ttf.h>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <algorithm>

#include "space_invaders_common.h"
#include "GameObject.h"
#include "Player.h"
#include "Powerup.h"
#include "InvaderManager.h"

class SpaceInvaders {
public:
    SpaceInvaders() : window(nullptr), renderer(nullptr), running(false), font(nullptr), paused(false), gameOver(false), betweenWaves(true),
                      waveClearedTime(0), wave(0), powerLevel(1), powerUpCounter(0), player(SCREEN_WIDTH / 2 - 25, SCREEN_HEIGHT - 60, 50, 40) {}
    
    ~SpaceInvaders() {
        cleanup();
    }
    
    bool initialize() {
        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
            return false;
        }

        if (TTF_Init() == -1) {
            std::cerr << "SDL_ttf could not initialize! TTF_Error: " << TTF_GetError() << std::endl;
            // This is not fatal, we can continue without text.
        }
        
        window = SDL_CreateWindow("Space Invaders",
                                SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        
        if (!window) {
            std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
            return false;
        }
        
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        if (!renderer) {
            std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
            return false;
        }

        // NOTE: You might need to change this path to a font file on your system.
        font = TTF_OpenFont("C:/Windows/Fonts/arial.ttf", 24);
        if (!font) {
            std::cerr << "Warning: Failed to load font! TTF_Error: " << TTF_GetError() << std::endl;
            std::cerr << "Pause text will not be displayed." << std::endl;
        }

        powerFont = TTF_OpenFont("C:/Windows/Fonts/arial.ttf", 60);
        
        running = true;
        srand(time(nullptr));
        return true;
    }
    
    void handleEvents() {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                running = false;
            } 
            else if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_ESCAPE) {
                    paused = !paused;
                    if (paused) {
                        std::cout << "Game Paused" << std::endl;
                    } else {
                        std::cout << "Game Resumed" << std::endl;
                    }
                } 
            }
            else if (!gameOver && paused && e.type == SDL_MOUSEBUTTONDOWN) {
                int x, y;
                SDL_GetMouseState(&x, &y);

                if (x >= resumeButton.x && x <= resumeButton.x + resumeButton.w &&
                    y >= resumeButton.y && y <= resumeButton.y + resumeButton.h) {
                    paused = false;
                    std::cout << "Game Resumed" << std::endl;
                }

                if (x >= quitButton.x && x <= quitButton.x + quitButton.w &&
                    y >= quitButton.y && y <= quitButton.y + quitButton.h) {
                    running = false;
                }
            }
            else if (gameOver && e.type == SDL_MOUSEBUTTONDOWN) {
                int x, y;
                SDL_GetMouseState(&x, &y);

                if (x >= restartButton.x && x <= restartButton.x + restartButton.w &&
                    y >= restartButton.y && y <= restartButton.y + restartButton.h) {
                    gameOver = false;
                    paused = false;
                    wave = 0;
                    powerLevel = 1;
                    bullets.clear();
                    invaderBullets.clear();
                    powerUps.clear();
                    player.reset(SCREEN_WIDTH / 2 - 25, SCREEN_HEIGHT - 60);
                    invaderManager.clearInvaders();
                    powerUpCounter = 0;
                    betweenWaves = true;
                    waveClearedTime = SDL_GetTicks();
                }
                
                if (x >= quitButton.x && x <= quitButton.x + quitButton.w &&
                    y >= quitButton.y && y <= quitButton.y + quitButton.h) {
                    running = false;
                }
            }
        }
        
        if (!paused) {
            player.handleInput();
        }
    }
    
    void update() {
        player.update(bullets);

        // Update bullets
        for (auto& bullet : bullets) {
            bullet.y -= BULLET_SPEED;
            if (bullet.y < 0) bullet.active = false;
        }
        
        // Update invader bullets
        for (auto& bullet : invaderBullets) {
            bullet.y += BULLET_SPEED - 2;
            if (bullet.y > SCREEN_HEIGHT) bullet.active = false;
        }

        for (auto& powerUp : powerUps) {
            powerUp.update();
        }
        
        invaderManager.update(invaderBullets);

        // Check collisions - bullets vs invaders
        invaderManager.checkCollisions(bullets, powerUps);
        
        // Check collisions - invader bullets vs player
        for (auto& bullet : invaderBullets) {
            if (bullet.collidesWith(player)) {
                gameOver = true;
                paused = true;
                break;
            }
        }

        for (auto& invader : invaderManager.getInvaders()) {
            if (invader.collidesWith(player)) {
                gameOver = true;
                paused = true;
                break;
            }
        }

        for (auto& powerUp : powerUps) {
            if (powerUp.collidesWith(player) && !powerUp.isCollected()) {
                powerUpCounter++;
                powerUp.collectTo(powerRect);
                if (powerUpCounter >= 5) {
                    powerLevel++;
                    powerUpCounter = 0;
                }
                break;
            }
        }
        
        // Check win condition
        if (invaderManager.allInvadersDestroyed() && !betweenWaves) {
            betweenWaves = true;
            waveClearedTime = SDL_GetTicks();
        }

        if (betweenWaves && SDL_GetTicks() - waveClearedTime > 6000) {
            wave++;
            invaderManager.initializeInvaders();
            betweenWaves = false;
        }
        
        // Remove inactive objects
        bullets.erase(std::remove_if(bullets.begin(), bullets.end(),
                     [](const GameObject& obj) { return !obj.active; }), bullets.end());
        invaderBullets.erase(std::remove_if(invaderBullets.begin(), invaderBullets.end(),
                            [](const GameObject& obj) { return !obj.active; }), invaderBullets.end());
    }
    
    void render() {
        // Clear screen
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        if (wave > 0)
        {
            if (betweenWaves)
                renderText("Wave Cleared!", SCREEN_WIDTH / 2, 20, { 255, 255, 255, 255 }, font);
            else
                renderText("Wave: " + std::to_string(wave), SCREEN_WIDTH / 2, 20, { 255, 255, 255, 255 }, font);
        }

        renderText(std::to_string(powerLevel), powerRect.x + powerRect.w / 2 + 5, powerRect.y + powerRect.h / 2, { 255, 255, 255, 255 }, powerFont);

        // Render Power Bars
        for (int i = 0; i < 5; ++i) {
            if (i < powerUpCounter) {
                SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // Lit
            } else {
                SDL_SetRenderDrawColor(renderer, 0, 100, 0, 255); // Unlit
            }
            SDL_RenderFillRect(renderer, &powerBars[i]);
        }
        
        // Draw player
        player.render(renderer);
        
        // Draw bullets
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
        for (const auto& bullet : bullets) {
            if (bullet.active) {
                SDL_Rect bulletRect = bullet.getRect();
                SDL_RenderFillRect(renderer, &bulletRect);
            }
        }
        
        // Draw invader bullets
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        for (const auto& bullet : invaderBullets) {
            if (bullet.active) {
                SDL_Rect bulletRect = bullet.getRect();
                SDL_RenderFillRect(renderer, &bulletRect);
            }
        }

        //SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        for (auto& powerUp : powerUps) {
            if (powerUp.active) {
                powerUp.render(renderer);
            }
        }
        
        // Draw invaders
        invaderManager.render(renderer);

        if (gameOver) {
            renderGameOverDialog();
        }
        
        if (!gameOver && paused) {
            renderPauseDialog();
        }

        if (betweenWaves && SDL_GetTicks() - waveClearedTime > 3000 && SDL_GetTicks() - waveClearedTime < 5000) {
            renderText("Wave " + std::to_string(wave+1), SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 - 100, { 0, 255, 0, 255 }, font);
        }

        SDL_RenderPresent(renderer);
    }
    
    void run() {
        while (running) {
            handleEvents();
            if (!paused && !gameOver) {
                update();
            }
            render();
            SDL_Delay(16); // ~60 FPS
        }
    }
    
    void cleanup() {
        if (font) {
            TTF_CloseFont(font);
            font = nullptr;
        }

        if (powerFont) {
            TTF_CloseFont(powerFont);
            powerFont = nullptr;
        }

        if (renderer) {
            SDL_DestroyRenderer(renderer);
            renderer = nullptr;
        }
        if (window) {
            SDL_DestroyWindow(window);
            window = nullptr;
        }
        SDL_Quit();
        TTF_Quit();
    }

private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    bool running;
    TTF_Font* font;
    TTF_Font* powerFont;
    bool paused;
    bool gameOver;
    bool betweenWaves;
    Uint32 waveClearedTime;
    
    Player player;
    InvaderManager invaderManager;
    std::vector<GameObject> bullets;
    std::vector<GameObject> invaderBullets;
    std::vector<Powerup> powerUps;

    SDL_Rect powerRect = { 0, 0, 70, 70 };
    SDL_Rect powerBars[5] = { {5, 80, 10, 10}, {20, 80, 10, 10}, {35, 80, 10, 10}, {50, 80, 10, 10}, {65, 80, 10, 10} };

    SDL_Rect pauseRect = { SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 - 100, 200, 200 };
    SDL_Rect gameOverRect = { SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 - 100, 200, 200 };
    SDL_Rect resumeButton = { SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT / 2 - 40, 100, 30 };
    SDL_Rect quitButton = { SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT / 2 + 20, 100, 30 };
    SDL_Rect restartButton = { SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT / 2 - 40, 100, 30 };
    
    int wave;
    int powerLevel;
    int powerUpCounter;

    void renderText(const std::string& text, int x, int y, SDL_Color color, TTF_Font* font = nullptr) {
        if (!font) return;

        SDL_Surface* textSurface = TTF_RenderText_Solid(font, text.c_str(), color);
        if (!textSurface) {
            std::cerr << "Unable to render text surface! SDL_ttf Error: " << TTF_GetError() << std::endl;
            return;
        }

        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        if (!textTexture) {
            std::cerr << "Unable to create texture from rendered text! SDL Error: " << SDL_GetError() << std::endl;
            SDL_FreeSurface(textSurface);
            return;
        }

        // Center the text on the given x, y coordinates
        SDL_Rect destRect = { x - textSurface->w / 2, y - textSurface->h / 2, textSurface->w, textSurface->h };
        SDL_RenderCopy(renderer, textTexture, nullptr, &destRect);

        SDL_FreeSurface(textSurface);
        SDL_DestroyTexture(textTexture);
    }

    void renderPauseDialog() {
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 100, 100, 100, 150); // Semi-transparent gray
        SDL_RenderFillRect(renderer, &pauseRect);

        SDL_Color textColor = { 255, 255, 255, 255 }; // White color
        renderText("PAUSED", pauseRect.x + pauseRect.w / 2, pauseRect.y + 20, textColor, font);

        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // Green color
        SDL_RenderFillRect(renderer, &resumeButton);
        renderText("Resume", resumeButton.x + resumeButton.w / 2, resumeButton.y + resumeButton.h / 2, textColor, font);

        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Red color
        SDL_RenderFillRect(renderer, &quitButton);
        renderText("Quit", quitButton.x + quitButton.w / 2, quitButton.y + quitButton.h / 2, textColor, font);
    }

    void renderGameOverDialog() {
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 100, 100, 100, 150); // Semi-transparent gray
        SDL_RenderFillRect(renderer, &gameOverRect);

        SDL_Color textColor = { 255, 255, 255, 255 }; // White color
        renderText("GAME OVER", gameOverRect.x + gameOverRect.w / 2, gameOverRect.y + 20, textColor, font);

        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // Green color
        SDL_RenderFillRect(renderer, &restartButton);
        renderText("Restart", restartButton.x + restartButton.w / 2, restartButton.y + restartButton.h / 2, textColor, font);

        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Red color
        SDL_RenderFillRect(renderer, &quitButton);
        renderText("Quit", quitButton.x + quitButton.w / 2, quitButton.y + quitButton.h / 2, textColor, font);
    }
};

int main(int argc, char* argv[]) {
    SpaceInvaders game;
    
    if (!game.initialize()) {
        return -1;
    }
    
    std::cout << "Space Invaders Game Started!" << std::endl;
    
    game.run();
    
    return 0;
}
