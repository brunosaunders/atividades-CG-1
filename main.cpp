#include <SDL.h>
#include <iostream>
#include <vector>
#include <cmath>

using namespace std;

float max_in_vector(std::vector<float> v) {
    float max = v[0];

    for (auto& value : v) {
        if (value > max) max = value; 
    }
    return max;
}

float min_in_vector(std::vector<float> v) {
    float min = v[0];

    for (auto& value : v) {
        if (min > value) min = value; 
    }
    return min;
}

void print_vector(std::vector<float> v) {
    std::cout << "[";
    for (auto& value : v) {
        std::cout << value << " ";
    }
    std::cout << "]\n";
}

class Color {
    public:
        int r; int g; int b;
        Color() {}
        Color(int red, int green, int blue) {
            this->r = red;
            this->g = green;
            this->b = blue;
        }
};

class Vector3d;

// A vector that works on SDL system
class Vector3dSDL {
    public:
        float x; float y; float z;
        Vector3dSDL() {}

        Vector3dSDL(float x, float y, float z) {
            this->x = x; this->y = y; this->z = z;
        }
        Vector3d to_creto_coord_system(int w_width, int w_height);
};

/*  Creto's system
    Window center is at (0, 0, -d)
    Observer is at (0, 0, 0)

    SDL's system
    Window center is at (window_width/2, window_height/2, -d)
    observer is at (window_width/2, window_height/2, 0)
*/

// A vector that works on Creto's coordinate system
class Vector3d {
    public:
        float x; float y; float z;

        Vector3d() {}
        Vector3d(float x, float y, float z) {
            this->x = x; this->y = y; this->z = z;
        }

        Vector3dSDL to_sdl_coord_system(int w_width, int w_height);
        Vector3d multiply(float value);
        Vector3d divide(float value);
        Vector3d sum(Vector3d v);
        Vector3d minus(Vector3d v);

        float size() {
            return sqrt(std::pow(this->x, 2) + std::pow(this->y, 2) + std::pow(this->z, 2));
        }

        float scalar_product(Vector3d v) {
            return (this->x * v.x + this->y * v.y + this->z * v.z);
        }
};

Vector3d Vector3dSDL::to_creto_coord_system(int w_width, int w_height) {
    float new_x = this->x - w_width/2;
    float new_y = this->y - w_height/2;

    return Vector3d(new_x, new_y, this->z);
}

Vector3dSDL Vector3d::to_sdl_coord_system(int w_width, int w_height) {
    float new_x = this->x + w_width/2;
    float new_y = this->y + w_height/2;

    return Vector3dSDL(new_x, new_y, this->z);
}

Vector3d Vector3d::multiply(float value) {
    float x = this->x * value;
    float y = this->y * value;
    float z = this->z * value;
    return Vector3d(x,y,z);
}

Vector3d Vector3d::divide(float value) {
    return this->multiply(1/value);
}

Vector3d Vector3d::sum(Vector3d v) {
    float x = this->x + v.x;
    float y = this->y + v.y;
    float z = this->z + v.z;
    return Vector3d(x, y, z);
}

Vector3d Vector3d::minus(Vector3d v) {
    Vector3d new_v = v.multiply(-1);
    return this->sum(new_v);
}

class Ray {
    public:
        Vector3d p1;
        Vector3d p2;

        Ray(){}

        Ray(Vector3d p1, Vector3d p2) {
            this->p1 = p1; this->p2 = p2;
        }

        float size() {
            return (p2.minus(p1)).size();
        }

        // Unitary direction vector
        Vector3d get_dr() {
            return (p2.minus(p1)).divide(this->size());
        }
};

class Sphere {
    public:
        Vector3d center;
        float radius;
        Color color;

        Sphere(float x, float y, float z, float r) {
            Vector3d center(x,y,z);
            this->center = center;
            this->radius = r;
            this->color = Color(255, 0, 0);
        }

        bool intersected_by(Ray ray) {
            Vector3d initial_point = ray.p2;
            Vector3d dr = ray.get_dr();

            Vector3d w = initial_point.minus(this->center);

            // We want to check if ||Pint - C||² = R²
            // (Pin + tint*dr - C).(Pin + tint*dr - C) - R² = 0
            // (w + tint*dr).(w + tint*dr) - R² = 0
            // w² + 2w*tint*dr + tint²*dr² - R² = 0
            // dr²*tint² + 2w*dr*tint + w² -     R² = 0
            float a = dr.scalar_product(dr);
            float b = w.multiply(2).scalar_product(dr);
            float c = w.scalar_product(w) - std::pow(this->radius, 2);

            float delta = std::pow(b, 2) - (4*a*c);

            return delta >= 0; // The ray intersect the sphere in 2 points or in 1 point and we see sphere's color.
        }
};

class Window {
    public:
        Vector3d center;
        int width;
        int height;

        int cols;
        int rows;

        // dx and dy of rectangles
        float dx; float dy;

        Window(){}
        Window(int width, int height, int cols, int rows, float x, float y, float z) {
            this->width = width; this->height = height;
            this->cols = cols; this->rows = rows;
            this->center = Vector3d(x, y, z);
            
            this->dx = width/(float) cols;
            this->dy = height/(float) rows;
        }
};


int render_picture(int n_rows, int n_cols, int window_width, int window_height) {

    Window cretos_window(window_width, window_height, n_cols, n_rows, 0, 0, -30);
    Vector3d origin(0, 0, 0);

    // Sphere sphere(0, 0, -1000, 960);
    std::vector<Sphere> objects;
    // objects.push_back(sphere);
    objects.push_back(Sphere(810, 0, -950, 900));
    objects.push_back(Sphere(-810, 0, -950, 800));


    // Initialize library
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_Log("Não foi possível inicializar o SDL! SDL_Error: %s", SDL_GetError());
        return 1;
    }
    

    SDL_Window* window = SDL_CreateWindow(
        "Computação Gráfica I - Bruno e Vitor",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        window_width,
        window_height,
        SDL_WINDOW_SHOWN
    );

    if (!window) {
        SDL_Log("Criação da janela falhou! SDL_Error: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        SDL_Log("Criação do renderer falhou! SDL_Error: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    bool isRunning = true;
    SDL_Event event;
    
    while (isRunning) {
        // Event listening
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                isRunning = false;
            }
        }

        // Good practice
        SDL_RenderClear(renderer);

        int counter = 0;
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

        // By default, we will always use Creto's system to calculate, when we need to draw just transform to SDL system
        for (int l = 0; l < n_rows; l++) {
            float y = cretos_window.height/2 - (cretos_window.dy/2) - (cretos_window.dy*l); // Creto's system

            for (int c = 0; c < n_cols; c++) {
                float x = - cretos_window.width/2 + (cretos_window.dx/2) + (cretos_window.dx*c); // Creto's system
                Vector3d center_of_small_rectangle(x,y, cretos_window.center.z);
                Ray ray(origin, center_of_small_rectangle);
                Color color = Color(100, 100, 100);

                if (objects[0].intersected_by(ray) || objects[1].intersected_by(ray)) {
                    color = objects[0].color;
                }


                SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);

                Vector3dSDL vecSDL = center_of_small_rectangle.to_sdl_coord_system(cretos_window.width, cretos_window.height);
                SDL_RenderDrawPointF(renderer, vecSDL.x, vecSDL.y);
            }
        }

        // Lastly, we update the window with the renderer we just painted
        SDL_RenderPresent(renderer);
    }

    // Free the memory
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

int main(int argc, char* argv[]) {
    return render_picture(1200, 600, 1200, 600);
}