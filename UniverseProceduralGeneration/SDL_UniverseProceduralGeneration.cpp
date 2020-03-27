#include "SDL.h"


#include "SDL_Framework.h"

constexpr uint32_t g_starColours[8] =
{
    0xFFFFFFFF, 0xFFD9FFFF, 0xFFA3FFFF, 0xFFFFC8C8,
    0xFFFFCB9D, 0xFF9F9FFF, 0xFF415EFF, 0xFF28199D
};

struct sPlanet
{
    double distance = 0.0;
    double diameter = 0.0;
    double foliage = 0.0;
    double minerals = 0.0;
    double water = 0.0;
    double gases = 0.0;
    double temperature = 0.0;
    double population = 0.0;
    bool ring = false;
    std::vector<double> vMoons;
};

class cStarSystem
{
public:
    cStarSystem(uint32_t x, uint32_t y, bool bGenerateFullSystem = false)
    {
        nProcGen = (x & 0xFFFF) << 16 | (y & 0xFFFF);

        starExists = (rndInt(0, 20) == 1);
        if (!starExists) return;

        starDiameter = rndDouble(10.0, 40.0);
        starColour = g_starColours[rndInt(0, 8)];

        if (!bGenerateFullSystem) return;

        double dDistanceFromStar = rndDouble(60.0, 200.0);
        int nPlanets = rndInt(0, 10);
        for (int i = 0; i < nPlanets; i++)
        {
            sPlanet p;
            p.distance = dDistanceFromStar;
            dDistanceFromStar += rndDouble(20.0, 200.0);
            p.diameter = rndDouble(4.0, 20.0);

            p.temperature = rndDouble(-200.0, 300.0);

            p.foliage = rndDouble(0.0, 1.0);
            p.minerals = rndDouble(0.0, 1.0);
            p.gases = rndDouble(0.0, 1.0);
            p.water = rndDouble(0.0, 1.0);

            double dSum = 1.0 / (p.foliage + p.minerals + p.gases + p.water);
            p.foliage *= dSum;
            p.minerals *= dSum;
            p.gases *= dSum;
            p.water *= dSum;

            p.population = std::max(rndInt(-5000000, 20000000), 0);

            p.ring = rndInt(0, 10) == 1;

            int nMoons = std::max(rndInt(-5, 5), 0);
            for (int n = 0; n < nMoons; n++)
            {
                p.vMoons.push_back(rndDouble(1.0, 5.0));
            }

            vPlanets.push_back(p);
        }
    }

    ~cStarSystem()
    {

    }

public:
    std::vector<sPlanet> vPlanets;

public:
    bool		starExists = false;
    double		starDiameter = 0.0f;
    uint32_t	starColour = 0xFFFFFFFF;

private:
    uint32_t nProcGen = 0;

    double rndDouble(double min, double max)
    {
        return ((double)rnd() / (double)(0x7FFFFFFF)) * (max - min) + min;
    }

    int rndInt(int min, int max)
    {
        return (rnd() % (max - min)) + min;
    }

    uint32_t rnd()
    {
        nProcGen += 0xe120fc15;
        uint64_t tmp;
        tmp = (uint64_t)nProcGen * 0x4a39b70d;
        uint32_t m1 = (uint32_t)((tmp >> 32) ^ tmp);
        tmp = (uint64_t)m1 * 0x12fad5c9;
        uint32_t m2 = (uint32_t)((tmp >> 32) ^ tmp);
        return m2;
    }
};

SDL_Point mGalaxyOffset = { 0,0 };

bool mStarSelected = false;

uint32_t nSelectedStarSeed1 = 0;
uint32_t nSelectedStarSeed2 = 0;

class SDL_UniverseProceduralGeneration : public SDL_Framework
{
public:
    bool user_init() override
    {
        return true;
    }

    bool user_render(int elapsed_time) override
    {
        SDL_SetRenderDrawColor(renderer(), 0, 0, 0, 0); // black color
        SDL_RenderClear(renderer());

        if (pressed_key() == SDLK_w) mGalaxyOffset.y -= 50;
        if (pressed_key() == SDLK_s) mGalaxyOffset.y += 50;
        if (pressed_key() == SDLK_a) mGalaxyOffset.x -= 50;
        if (pressed_key() == SDLK_d) mGalaxyOffset.x += 50;

        //std::cout << "(" << mGalaxyOffset.x << "," << mGalaxyOffset.y << ")\n";

        int nSectorsX = window_width() / 16;
        int nSectorsY = window_height() / 16;

        SDL_Point vMouse = { mouse_position().x / 16 , mouse_position().y / 16 };

        for (int vScreenSectorX = 0; vScreenSectorX < nSectorsX; vScreenSectorX++) {

            for (int vScreenSectorY = 0; vScreenSectorY < nSectorsY; vScreenSectorY++) {

                uint32_t seed1 = mGalaxyOffset.x + vScreenSectorX;
                uint32_t seed2 = mGalaxyOffset.y + vScreenSectorY;

                cStarSystem star(seed1, seed2);
                if (star.starExists)
                {
                    SDL_Point p = { vScreenSectorX * 16 + 8, vScreenSectorY * 16 + 8 };
                    SDL_Color c;
                    c.a = 0;
                    c.r = (star.starColour & 0xFF000000) >> 24;
                    c.g = (star.starColour & 0x00FF0000) >> 16;
                    c.b = (star.starColour & 0x0000FF00) >> 8;
                    draw_circle(p, (int)star.starDiameter / 8, c, true);

                    if (vMouse.x == vScreenSectorX && vMouse.y == vScreenSectorY) {
                        p.x = vScreenSectorX * 16 + 8;
                        p.y = vScreenSectorY * 16 + 8;
                        draw_circle(p, 12, { 255,255,255,255 }, false);
                    }
                }
            }
        }

        if (is_mouse_button_pressed(LEFT))
        {
            uint32_t seed1 = mGalaxyOffset.x + vMouse.x;
            uint32_t seed2 = mGalaxyOffset.y + vMouse.y;

            cStarSystem star(seed1, seed2);
            if (star.starExists)
            {
                mStarSelected = true;
                nSelectedStarSeed1 = seed1;
                nSelectedStarSeed2 = seed2;
            }
            else
                mStarSelected = false;
        }

        if (mStarSelected)
        {
            cStarSystem star(nSelectedStarSeed1, nSelectedStarSeed2, true);

            SDL_SetRenderDrawColor(renderer(), 0, 0, 128, 0); // dark blue color
            SDL_Rect r = { 8, window_height() - 232, 550, 232 };
            SDL_RenderFillRect(renderer(), &r);
            SDL_SetRenderDrawColor(renderer(), 255, 255, 255, 0); // white color
            SDL_RenderDrawRect(renderer(), &r);

            SDL_Point vBody = { 14, window_height() - 232 / 2 };

            vBody.x += (int)(star.starDiameter * 1.375);
            SDL_Color c;
            c.a = 0;
            c.r = (star.starColour & 0xFF000000) >> 24;
            c.g = (star.starColour & 0x00FF0000) >> 16;
            c.b = (star.starColour & 0x0000FF00) >> 8;
            draw_circle(vBody, (int)(star.starDiameter * 1.375), c, true);
            vBody.x += (int)((star.starDiameter * 1.375) + 8);

            for (auto& planet : star.vPlanets)
            {
                if (vBody.x + planet.diameter >= 496) break;

                vBody.x += (int)planet.diameter;
                draw_circle(vBody, (int)(planet.diameter * 1.0), { 255, 0, 0, 0 }, true); // red

                if (planet.ring) {
                    draw_circle(vBody, (int)(planet.diameter * 1.0 + 5), { 255, 255, 255, 255 }, false);// white
                }

                SDL_Point vMoon = { vBody.x, vBody.y };
                vMoon.y += (int)(planet.diameter + 10);

                for (auto& moon : planet.vMoons)
                {
                    vMoon.y += (int)moon;
                    draw_circle(vMoon, (int)moon, { 192, 192, 192, 0 }, true); // gray
                    vMoon.y += (int)(moon + 10);
                }

                vBody.x += (int)(planet.diameter + 8);
            }
        }

        SDL_RenderPresent(renderer());

        return true;
    }

    void user_clean() override
    {

    }
};

int main(int argc, char* argv[])
{
    SDL_UniverseProceduralGeneration universe;

    if (universe.init("Universe Procedural Generation", 400, 100, 1200, 800, 0)) {
        universe.run();
    }

    return 0;
}



