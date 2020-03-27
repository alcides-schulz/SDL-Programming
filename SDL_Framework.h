#pragma once

#include "SDL.h"

#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <random>

class SDL_Framework
{
private:
    bool mIsRunning = false;
    SDL_Window* mWindow;
    const char *mWindowTitle;
    SDL_Renderer* mRenderer;
    SDL_Keycode mPressedKey = 0;
    std::vector<bool> mMouseButtonStates;
    SDL_Point mMousePosition = { 0,0 };
    int mWindowWidth = 0;
    int mWindowHeight = 0;

public:
    enum mouse_buttons
    {
        LEFT = 0,
        MIDDLE = 1,
        RIGHT = 2
    };

    SDL_Framework()
    {
        mMouseButtonStates.push_back(false);
        mMouseButtonStates.push_back(false);
        mMouseButtonStates.push_back(false);
    }

    virtual bool user_render(int elapsedTime) { if (elapsedTime) return true; return true; };
    virtual bool user_init() { return true; };
    virtual void user_clean() {};

    int SDL_Framework::window_width() { return mWindowWidth; }
    int SDL_Framework::window_height() { return mWindowHeight; }
    SDL_Renderer* SDL_Framework::renderer() { return mRenderer; }

    SDL_Keycode pressed_key()
    {
        return mPressedKey;
    }

    SDL_Point mouse_position()
    {
        return mMousePosition;
    }

    bool is_mouse_button_pressed(enum mouse_buttons mouse_button)
    {
        return mMouseButtonStates[mouse_button];
    };

    bool init(const char* title, int xpos, int ypos, int width, int height, int flags)
    {
        int result = SDL_Init(SDL_INIT_EVERYTHING);
        if (result != 0)
        {
            std::cout << "SDL_Init failed, result: " << result << ".\n";
            std::getchar();
            return false;
        }

        mWindowTitle = title;
        mWindowWidth = width;
        mWindowHeight = height;

        mWindow = SDL_CreateWindow(title, xpos, ypos, width, height, flags);
        if (mWindow == 0) {
            std::cout << "SDL_CreateWindow failed.\n";
            std::getchar();
            return false;
        }

        mRenderer = SDL_CreateRenderer(mWindow, -1, 0);
        if (mRenderer == 0) {
            std::cout << "SDL_CreateRenderer failed.\n";
            std::getchar();
            return false;
        }

        mIsRunning = user_init();

        return mIsRunning;
    }

    void run()
    {
        Uint32 startTime = SDL_GetTicks();
        Uint32 timer = startTime;
        int frame_count = 0;

        while (mIsRunning) {
            Uint32 frameStart = SDL_GetTicks();
            Uint32 elapsedTime = frameStart - startTime;
            startTime = frameStart;

            handle_events();
            user_render(elapsedTime);

            frame_count++;
            if (SDL_GetTicks() - timer > 1000) {
                std::string t = mWindowTitle;
                t.append(" ");
                t.append(std::to_string(frame_count));
                t.append(" FPS");
                SDL_SetWindowTitle(mWindow, t.c_str());
                timer += 1000;
                frame_count = 0;
            }
        }

        user_clean();

        SDL_DestroyWindow(mWindow);
        SDL_DestroyRenderer(mRenderer);
        SDL_Quit();
    }

    void draw_circle(SDL_Point center, int radius, SDL_Color color, bool fill)
    {
        int radius2 = radius * radius;

        SDL_SetRenderDrawColor(mRenderer, color.r, color.g, color.b, color.a);
        for (int w = 0; w <= radius * 2; w++)
        {
            for (int h = 0; h <= radius * 2; h++)
            {
                int dx = radius - w;
                int dy = radius - h;
                int pos = dx * dx + dy * dy;
                if (fill && pos < radius2)
                {
                    SDL_RenderDrawPoint(mRenderer, center.x + dx, center.y + dy);
                }
                int diff = pos - radius2;
                if (!fill && abs(diff) <= 10)
                {
                    SDL_RenderDrawPoint(mRenderer, center.x + dx, center.y + dy);
                }
            }
        }
    }

private:
    void handle_events()
    {
        mPressedKey = 0;

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_KEYDOWN:
                mPressedKey = event.key.keysym.sym;
                break;
            case SDL_QUIT:
                mIsRunning = false;
                break;
            case SDL_MOUSEMOTION:
                mMousePosition.x = event.motion.x;
                mMousePosition.y = event.motion.y;
                break;
            case SDL_MOUSEBUTTONDOWN:
                if (event.button.button == SDL_BUTTON_LEFT)
                {
                    mMouseButtonStates[LEFT] = true;
                }
                if (event.button.button == SDL_BUTTON_MIDDLE)
                {
                    mMouseButtonStates[MIDDLE] = true;
                }
                if (event.button.button == SDL_BUTTON_RIGHT)
                {
                    mMouseButtonStates[RIGHT] = true;
                }
                break;
            case SDL_MOUSEBUTTONUP:
                if (event.button.button == SDL_BUTTON_LEFT)
                {
                    mMouseButtonStates[LEFT] = false;
                }

                if (event.button.button == SDL_BUTTON_MIDDLE)
                {
                    mMouseButtonStates[MIDDLE] = false;
                }

                if (event.button.button == SDL_BUTTON_RIGHT)
                {
                    mMouseButtonStates[RIGHT] = false;
                }
                break;
            default:
                break;
            }
        }
    }

};

//END
