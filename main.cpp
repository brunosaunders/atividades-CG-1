#include <SDL.h>
#include <iostream>
#include <vector>
#include <tuple>
#include <cmath>
#include <sstream>

using namespace std;

// All sources of light will have this class with intensity colors in range of 0-1 (white will be -> 1.0, 1.0, 1.0)
class IntensityColor {
    public:
        float r; float g; float b;
        IntensityColor() {}
        IntensityColor(uint8_t r, uint8_t g, uint8_t b) {
            this->r = r/256.0;
            this->g = g/256.0;
            this->b = b/256.0;
        }

};

class Color {
    public:
        uint8_t r; uint8_t g; uint8_t b;
        Color() {}
        Color(uint8_t red, uint8_t green, uint8_t blue) {
            this->r = red;
            this->g = green;
            this->b = blue;
        }
        IntensityColor to_color_normalized() {
            return IntensityColor(this->r, this->g, this->b);
        }

        // Combines source intensity color with true color from object.
        void arroba_multiply(IntensityColor intensity) {
            this->r = (uint8_t) this->r * intensity.r;
            this->g = (uint8_t) this->g * intensity.g;
            this->b = (uint8_t) this->b * intensity.b;
        }
};

/*  Creto's system
    Window center is at (0, 0, -d)
    Observer is at (0, 0, 0)
*/

// A vector that works on Creto's coordinate system
class Vector3d {
    public:
        float x; float y; float z;

        Vector3d() {}
        Vector3d(float x, float y, float z) {
            this->x = x; this->y = y; this->z = z;
        }

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

        Vector3d get_vector_normalized() {
            return this->divide(this->size());
        }
};


class SourceOfLight {
    public:
        IntensityColor intensity;
        Vector3d center;
        SourceOfLight(){}
        SourceOfLight(Color color, Vector3d center) {
            this->intensity = color.to_color_normalized();
            this->center = center;
        }
};


std::ostream &operator<<(std::ostream &os, Vector3d const &v) {
    return os << "Vector(" << v.x << "," << v.y << "," << v.z << ")";
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

std::ostream &operator<<(std::ostream &os, Ray const &r) {
    return os << "Ray(P1: " << r.p1 << " | P2: " << r.p2 << ")";
}

class Intersection {
    public:
        float time;
        bool is_valid;

        Intersection(){}
        Intersection(float t, bool valid) {
            this->time = t; this->is_valid = valid;
        }
};

class Object {
    public:
        // bool intersected_by(Ray ray) {}
        Color color;
        virtual Intersection get_intersection(Ray ray) {return Intersection(0.0, false);}
};

class Sphere : public Object{
    public:
        Vector3d center;
        float radius;

        Sphere(float x, float y, float z, float r) {
            Vector3d center(x,y,z);
            this->center = center;
            this->radius = r;
            this->color = Color(255, 0, 0);
        }

        // n unitary vector (normal vector).
        Vector3d get_normal_vector(Vector3d intersection_point) {
            return (intersection_point.minus(this->center)).divide(this->radius);
        }

        // l unitary vector in direction to light.
        Vector3d get_light_vector(Vector3d intersection_point, SourceOfLight source) {
            return (source.center.minus(intersection_point)).get_vector_normalized();
        }
       
        Intersection get_intersection(Ray ray) {

            Vector3d initial_point = ray.p2;
            Vector3d dr = ray.get_dr();

            Vector3d w = initial_point.minus(this->center);

            // We want to check if ||Pint - C||² = R²
            // (Pin + tint*dr - C).(Pin + tint*dr - C) - R² = 0
            // (w + tint*dr).(w + tint*dr) - R² = 0
            // w² + 2w*tint*dr + tint²*dr² - R² = 0
            // dr²*tint² + 2w*dr*tint + w² -     R² = 0
            float a = dr.scalar_product(dr);
            float b = (w.multiply(2)).scalar_product(dr);
            float c = w.scalar_product(w) - std::pow(this->radius, 2);

            float delta = std::pow(b, 2) - (4*a*c);
            float t1 = (-b + std::sqrt(delta))/(2*a);
            float t2 = (-b - std::sqrt(delta))/(2*a);

            if (delta < 0) {
                return Intersection(0.0, false);
            }

            Vector3d intersection_point;
            if (t1 < t2) return Intersection(t1, true);
            return Intersection(t2, true);
        }
};

class Scene {
    public:
        std::vector<Object*> objects;
        Color background_color;
        SourceOfLight source_of_light;

        Scene(Color bg_color, SourceOfLight source) {
            this->background_color = bg_color;
            this->source_of_light = source;
        }

        void push_object(Object *obj) {
            objects.push_back(obj);
        }

        Color get_color_to_draw(Ray ray) {
            float min_t = 10000000.0;
            int min_index = -1;
            for(int i=0; i < objects.size(); i++) {
                Intersection intersection = objects[i]->get_intersection(ray);
                if (intersection.is_valid) {
                    if (intersection.time < min_t) {
                        min_t = intersection.time;
                        min_index = i;
                    }
                }
            }

            if (min_index == -1) return this->background_color;
            return objects[min_index]->color;
        }

        void dealloc_objects() {
            for (auto &obj : objects) {
                delete obj;
            }
        }
};

class Window {
    public:
        Vector3d center;
        float width;
        float height;

        int cols;
        int rows;

        // dx and dy of rectangles
        float dx; float dy;

        std::vector<std::vector<Color>> windows_colors;
        bool should_update = true;

        Window(){}
        Window(float width, float height, int cols, int rows, float x, float y, float z) {
            this->width = width; this->height = height;
            this->cols = cols; this->rows = rows;
            this->center = Vector3d(x, y, z);
            
            this->dx = width/(float) cols;
            this->dy = height/(float) rows;

            for (int i=0; i < rows; i++) {
                std::vector<Color> row;
                for (int j=0; j < cols; j++) {
                    row.push_back(Color(100,100,100));
                }
                windows_colors.push_back(row);
            } 
        }
};


int render_picture(int n_rows, int n_cols, int sdl_width, int sdl_height, float window_width, float window_height) {

    Vector3d origin(0, 0, 0);
    Window *cretos_window = new Window(window_width, window_height, n_cols, n_rows, 0, 0, -1);

    SourceOfLight source_of_light(Color(255,255,255), Vector3d(0, 4, -1));
    Scene scene(Color(100,100,100), source_of_light);

    scene.push_object(new Sphere(0, 0, -5, 0.5));
    scene.push_object(new Sphere(0, 1, -5, 0.5));
    scene.push_object(new Sphere(1, 0, -5, 0.5));


    // Initialize library
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_Log("Não foi possível inicializar o SDL! SDL_Error: %s", SDL_GetError());
        return 1;
    }
    

    SDL_Window* window = SDL_CreateWindow(
        "Computação Gráfica I - Bruno e Vitor",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        sdl_width,
        sdl_height,
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

    // using pointers to save memory inside the loop
    Ray* ray = new Ray();
    Vector3d* center_of_small_rectangle = new Vector3d();
    
    while (isRunning) {
        // Event listening
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                isRunning = false;
                cretos_window->should_update = true;
            }

            if (event.type == SDL_KEYUP) {
                cretos_window->center.z--; 
                cout << cretos_window->center << endl;
                cretos_window->should_update = true;
            }
        }

        // Good practice
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

        // Just draw the colors saved in cretos_window->windows_colors
        if (!cretos_window->should_update) {
            for (int l = 0; l < n_rows; l++) {             
                for (int c = 0; c < n_cols; c++) {
                    Color color = cretos_window->windows_colors[l][c];
               
                    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);
                    SDL_RenderDrawPoint(renderer, c, l);
                }
            }
            // Lastly, we update the window with the renderer we just painted
            SDL_RenderPresent(renderer);
            continue;
        }

        // By default, we will always use Creto's system to calculate, when we need to draw just transform to SDL system
        for (int l = 0; l < n_rows; l++) {
            float y = cretos_window->height/2 - (cretos_window->dy/2) - (cretos_window->dy*l); // Creto's system

            for (int c = 0; c < n_cols; c++) {
                float x = - cretos_window->width/2 + (cretos_window->dx/2) + (cretos_window->dx*c); // Creto's system

                center_of_small_rectangle->x = x;
                center_of_small_rectangle->y = y;
                center_of_small_rectangle->z = cretos_window->center.z;

                ray->p1 = origin;
                ray->p2 = *center_of_small_rectangle;


                cretos_window->windows_colors[l][c] = scene.get_color_to_draw(*ray);

                Color color = cretos_window->windows_colors[l][c];
               
                SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);
                SDL_RenderDrawPoint(renderer, c, l);
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


void tests() {
    Vector3d v(16,28,10);
    Vector3d v2(22,36,10);

    Ray r(v,v2);
    cout << r << endl;
    cout << r.get_dr() << endl;
}

int main(int argc, char* argv[]) {
    // window width and height will be 1.0 meter. We will render everything in a SDL window with pixes specified.
    return render_picture(700, 700, 700, 700, 1.0, 1.0);
}
