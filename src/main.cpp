#include <SDL2/SDL.h>
#include <iostream>
#include <vector>
#include <cmath>

#include "Color.hpp"
#include "Vector3d.hpp"
#include "Ray.hpp"
#include "Objects.hpp"
#include "Camera.hpp"
#include "Scene.hpp"

using namespace std;

using namespace atividades_cg_1::algebra;
using namespace atividades_cg_1::objects;
using namespace atividades_cg_1::camera;
using namespace atividades_cg_1::scene;


int render_picture(int n_rows, int n_cols, int sdl_width, int sdl_height, float window_width, float window_height)
{
    Vector3d eye(0, 0, 0);
    Window *cretos_window = new Window(window_width, window_height, n_cols, n_rows, 0, 0, -.3);
    float width_ratio = sdl_width / (float)n_cols;
    float height_ratio = sdl_height / (float)n_rows;

    IntensityColor source_intensity = IntensityColor(.7, .7, .7);
    IntensityColor sphere_k_d = IntensityColor(.7, .2, .2);
    IntensityColor sphere_k_e = IntensityColor(.7, .2, .2);
    IntensityColor sphere_k_a = IntensityColor(.7, .2, .2);

    SourceOfLight pontual_light(source_intensity, Vector3d(0, .6, -.3));
    IntensityColor environment_light_intensity = IntensityColor(0.3, 0.3, 0.3); // Come from every direction uniformly

    Scene scene(Color(30, 30, 30), pontual_light, environment_light_intensity, eye);

    float sphere_radius = .4;
    IntensityColor floor_plan_k_difuse = IntensityColor(.2, .7, .2);
    IntensityColor floor_plan_k_specular = IntensityColor(0, 0, 0);
    IntensityColor floor_plan_k_environment = IntensityColor(.2, .7, .2);

    IntensityColor back_plan_k_difuse = IntensityColor(.3, .3, .7);
    IntensityColor back_plan_k_specular = IntensityColor(0, 0, 0);
    IntensityColor back_plan_k_environment = IntensityColor(.3, .3, .7);


    Plan *floor_plan = new Plan(Vector3d(0, sphere_radius*(-1), 0), Vector3d(0, 1, 0), floor_plan_k_difuse, floor_plan_k_specular, floor_plan_k_environment, 1, Color(50,25,199));
    Plan *back_plan = new Plan(Vector3d(0, 0, -2), Vector3d(0,0,1), back_plan_k_difuse, back_plan_k_specular, back_plan_k_environment, 1, Color(255,255,255));
    scene.push_object(new Sphere(Vector3d(0, 0, -1), sphere_radius, Color(222, 0, 0), sphere_k_d, sphere_k_e, sphere_k_a, 10));
    scene.push_object(floor_plan);
    scene.push_object(back_plan);


    // Initialize library
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        SDL_Log("Não foi possível inicializar o SDL! SDL_Error: %s", SDL_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow(
        "Computação Gráfica I - Bruno e Guilherme",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        sdl_width,
        sdl_height,
        SDL_WINDOW_SHOWN);

    if (!window)
    {
        SDL_Log("Criação da janela falhou! SDL_Error: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer)
    {
        SDL_Log("Criação do renderer falhou! SDL_Error: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    bool isRunning = true;
    SDL_Event event;

    // using pointers to save memory inside the loop
    Ray *ray = new Ray();
    Vector3d *center_of_small_rectangle = new Vector3d();

    while (isRunning)
    {
        // Event listening
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                isRunning = false;
                cretos_window->should_update = true;
            }

            if (event.type == SDL_KEYUP)
            {
                cretos_window->center.z--;
                // cout << cretos_window->center << endl;
                cretos_window->should_update = true;
            }
        }

        // Good practice
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

        // Just draw the colors saved in cretos_window->windows_colors
        if (!cretos_window->should_update)
        {
            for (int l = 0; l < n_rows; l++)
            {
                for (int c = 0; c < n_cols; c++)
                {
                    Color color = cretos_window->windows_colors[l][c];

                    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);
                    SDL_RenderDrawPointF(renderer, c * width_ratio, l * height_ratio);
                }
            }
            // Lastly, we update the window with the renderer we just painted
            SDL_RenderPresent(renderer);
            continue;
        }

        // By default, we will always use Creto's system to calculate, when we need to draw just transform to SDL system
        for (int l = 0; l < n_rows; l++)
        {
            float y = cretos_window->height / 2 - (cretos_window->dy / 2) - (cretos_window->dy * l); // Creto's system

            for (int c = 0; c < n_cols; c++)
            {
                float x = -cretos_window->width / 2 + (cretos_window->dx / 2) + (cretos_window->dx * c); // Creto's system

                center_of_small_rectangle->x = x;
                center_of_small_rectangle->y = y;
                center_of_small_rectangle->z = cretos_window->center.z;

                ray->p1 = eye;
                ray->p2 = *center_of_small_rectangle;

                cretos_window->windows_colors[l][c] = scene.get_color_to_draw(*ray);

                Color color = cretos_window->windows_colors[l][c];

                SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);
                SDL_RenderDrawPointF(renderer, c * width_ratio, l * height_ratio);
            }
        }

        // Lastly, we update the window with the renderer we just painted
        SDL_RenderPresent(renderer);
        cretos_window->should_update = false;
    }

    // Free the memory
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    scene.dealloc_objects();
    delete ray;
    delete center_of_small_rectangle;
    delete cretos_window;
    return 0;
}


void test_vectorial_product() {
    Vector3d v1(2,0,5);
    Vector3d v2(1, 1, 8);

    Vector3d result(-5, -11, 2);
    if (!result.equals(v1.vectorial_product(v2))) {
        throw logic_error("vectorial_product failed");
    }
}
void run_tests() {
    test_vectorial_product();
}

int main(int argc, char *argv[])
{
    // window width and height will be 1.0 meter. We will render everything in a SDL window with pixes specified.
    run_tests();
    return render_picture(500, 500, 500, 500, .6, .6);
}