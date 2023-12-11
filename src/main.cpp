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


SDL_Texture* loadTexture(SDL_Renderer* renderer, const char* filename) {
    SDL_Surface *surface = SDL_LoadBMP(filename);
    if (!surface) {
        SDL_Log("Erro ao carregar a textura: %s", SDL_GetError());
        return nullptr;
    }

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    if (!texture) {
        SDL_Log("Erro ao criar a textura: %s", SDL_GetError());
        return nullptr;
    }

    return texture;
}

int render_picture(int n_rows, int n_cols, int sdl_width, int sdl_height, float window_width, float window_height)
{
    Vector3d eye(0, 0, 0);
    Window *cretos_window = new Window(window_width, window_height, n_cols, n_rows, 0, 0, -.3);
    float width_ratio = sdl_width / (float)n_cols;
    float height_ratio = sdl_height / (float)n_rows;

    IntensityColor source_intensity = IntensityColor(.7, .7, .7);
    IntensityColor sphere_k_d = IntensityColor(.7, .7, .7);
    IntensityColor sphere_k_e = IntensityColor(.9, .9, .9);
    IntensityColor sphere_k_a = IntensityColor(.5, .5, .5);

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
    scene.push_object(new Sphere(Vector3d(0, 0, -1), sphere_radius, Color(222, 0, 0), sphere_k_d, sphere_k_e, sphere_k_a, 100));
    scene.push_object(floor_plan);
    scene.push_object(back_plan);

    //Plan *floor_plan = new Plan(Vector3d(200, 0, 200), Vector3d(0, 1, 0), floor_plan_k_difuse, floor_plan_k_specular, floor_plan_k_environment, 1, Color(50,25,199));
    //Plan *roof_plan = new Plan(Vector3d(200, 400, 200), Vector3d(0, -1, 0), back_plan_k_difuse, back_plan_k_difuse, back_plan_k_difuse, 1, Color(199, 25, 50));
    //Plan *back_plan = new Plan(Vector3d(200, 200, 400), Vector3d(0,0,-1), back_plan_k_difuse, back_plan_k_specular, back_plan_k_environment, 1, Color(0,0,255));
    //Plan *front_plan = new Plan(Vector3d(200, 200, 0), Vector3d(0,0,1), back_plan_k_difuse, back_plan_k_difuse, back_plan_k_difuse, 1, Color(0,0,255));
    //Plan *left_plan = new Plan(Vector3d(0, 200, 200), Vector3d(1,0,0), floor_plan_k_difuse, floor_plan_k_difuse, floor_plan_k_difuse, 1, Color(0,255,0));
    //Plan *right_plan = new Plan(Vector3d(400, 200, 200), Vector3d(-1,0,0), floor_plan_k_difuse, floor_plan_k_difuse, floor_plan_k_difuse, 1, Color(0,255,0));

    //scene.push_object(floor_plan);
    //scene.push_object(roof_plan);
    //scene.push_object(back_plan);
    //scene.push_object(front_plan);
    //scene.push_object(left_plan);
    //scene.push_object(right_plan);

    //scene.push_object(new Cylinder(Vector3d(0, 0, -2), Vector3d(0, 0, -1), Vector3d(0, 0, 1), .4, 1, IntensityColor(.2, .7, .2), IntensityColor(0, 0, 0), IntensityColor(.2, .7, .2), 10, Color(0, 255, 0)));
    //scene.push_object(new Cylinder(Vector3d(0, 0, -1), Vector3d(0, 2, -1), Vector3d(0, 2, 0), .4, 2, IntensityColor(.2, .7, .2), IntensityColor(0, 0, 0), IntensityColor(.2, .7, .2), 10, Color(0, 255, 0)));

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
    bool GUI_visible = false;
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
                cretos_window->center.z += 0.05;
                // cout << cretos_window->center << endl;
                cretos_window->should_update = true;
            }

            if(event.type == SDL_MOUSEBUTTONDOWN)
            {
                if(event.button.button == SDL_BUTTON_RIGHT)
                {
                    if(!GUI_visible)
                    {
                        int mainWinX, mainWinY;
                        SDL_GetWindowPosition(window, &mainWinX, &mainWinY);
                        SDL_Window *window_GUI = SDL_CreateWindow(
                                "Escolher Perspectiva",
                                mainWinX + sdl_width,
                                mainWinY,
                                sdl_width - 150,
                                sdl_width - 150,
                                SDL_WINDOW_SHOWN
                        );

                        if(!window_GUI)
                        {
                            SDL_Log("Criação da janela GUI falhou! SDL_ERRO: %s", SDL_GetError());
                            SDL_Quit();
                            return 1;
                        }

                        SDL_Renderer *renderer_GUI = SDL_CreateRenderer(window_GUI, -1, SDL_RENDERER_ACCELERATED);

                        if(!renderer_GUI)
                        {
                            SDL_Log("Criação do renderer GUI falhou! SDL_Error: %s", SDL_GetError());
                            SDL_DestroyWindow(window);
                            SDL_Quit();
                            return 1;
                        }

                        bool GUI_Running = true;
                        SDL_Event GUI_event;
                        while(GUI_Running)
                        {
                            while(SDL_PollEvent(&GUI_event))
                            {
                                if (GUI_event.type == SDL_WINDOWEVENT && GUI_event.window.event == SDL_WINDOWEVENT_CLOSE)
                                {
                                    GUI_Running = false;
                                }
                                else if (GUI_event.type == SDL_MOUSEBUTTONDOWN)
                                {
                                    int mouseX = GUI_event.button.x, mouseY = GUI_event.button.y;
                                    if(mouseX >= 80 && mouseX <= 280 && mouseY >= 50 && mouseY <= 100)
                                    {
                                        std::printf("Botão Verde clicado!\n");
                                    }
                                    if(mouseX >= 80 && mouseX <= 280 && mouseY >= 120 && mouseY <= 170)
                                    {
                                        std::printf("Botão Azul clicado!\n");
                                    }
                                    if(mouseX >= 80 && mouseX <= 280 && mouseY >= 190 && mouseY <= 240)
                                    {
                                        std::printf("Botão Vermelho clicado!\n");
                                    }
                                }


                            }
                            SDL_RenderClear(renderer_GUI);
                            SDL_SetRenderDrawColor(renderer_GUI, 255, 255, 255, 255);

                            SDL_Rect button1Rect = {80, 50, 200, 50};
                            SDL_SetRenderDrawColor(renderer_GUI, 0, 255, 0, 255);
                            SDL_RenderFillRect(renderer_GUI, &button1Rect);

                            SDL_Rect button2Rect = {80, 50 + 70, 200, 50};
                            SDL_SetRenderDrawColor(renderer_GUI, 0, 0, 255, 255);
                            SDL_RenderFillRect(renderer_GUI, &button2Rect);

                            SDL_Rect button3Rect = {80, 50 + 140, 200, 50};
                            SDL_SetRenderDrawColor(renderer_GUI, 255, 0, 0, 255);
                            SDL_RenderFillRect(renderer_GUI, &button3Rect);

                            SDL_SetRenderDrawColor(renderer_GUI, 255, 255, 255, 255);
                            SDL_RenderPresent(renderer_GUI);
                            GUI_visible = true;
                        }
                        SDL_DestroyRenderer(renderer_GUI);
                        SDL_DestroyWindow(window_GUI);
                        GUI_visible = false;
                    }
                }
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

void test_normal_vector_cylinder()
{
    // Criação de um cilindro para teste
    Vector3d baseCenter(0, 0, -1);    // Centro da base
    Vector3d topCenter(0, 0, 1);      // Centro do topo
    Vector3d cylinderDirection(0, 1, 0); // Vetor da base para o topo (paralelo ao eixo y)
    double radius = 0.5;
    double height = 2.0;

    Cylinder myCylinder(baseCenter, topCenter, cylinderDirection, radius, height, IntensityColor(.2, .7, .2), IntensityColor(0, 0, 0), IntensityColor(.2, .7, .2), 10, Color(0, 255, 0));

    // Pontos de teste para calcular o vetor normal
    Vector3d testPoint1(0, 0, -1);    // Ponto na base
    Vector3d testPoint2(0, 0, 1);      // Ponto no topo
    Vector3d testPoint3(0, 0.5, 0);    // Ponto na lateral do cilindro

    // Cálculo dos vetores normais
    Vector3d normal1 = myCylinder.get_normal_vector(testPoint1);
    Vector3d normal2 = myCylinder.get_normal_vector(testPoint2);
    Vector3d normal3 = myCylinder.get_normal_vector(testPoint3);

    Vector3d result1(0, 0, -1);
    Vector3d result2(0, 0, 1);
    Vector3d result3(0, 1, 0);

    if (!result1.equals(normal1)) {
        normal1.print();
        std::cout << "1";
        throw logic_error("vectorial_normal failed");
    }
    if (!result2.equals(normal2)) {
        normal2.print();
        std::cout << "2";
        throw logic_error("vectorial_normal failed");
    }
    if (!result3.equals(normal3)) {
        normal3.print();
        std::cout << "3";
        throw logic_error("vectorial_normal failed");
    }
}

void run_tests() {
    test_normal_vector_cylinder();
}



int main(int argc, char *argv[])
{
    // window width and height will be 1.0 meter. We will render everything in a SDL window with pixes specified.
    //run_tests();
    return render_picture(500, 500, 500, 500, .6, .6);
}
