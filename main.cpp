#include <SDL.h>
#include <iostream>
#include <vector>
#include <cmath>

#include "Color.hpp"

using namespace std;

/*  Creto's system
    Window center is at (0, 0, -d)
    Observer is at (0, 0, 0)
*/

// A vector that works on Creto's coordinate system
class Vector3d
{
public:
    float x;
    float y;
    float z;

    Vector3d() {}
    Vector3d(float x, float y, float z)
    {
        this->x = x;
        this->y = y;
        this->z = z;
    }

    Vector3d multiply(float value);
    Vector3d divide(float value);
    Vector3d sum(Vector3d v);
    Vector3d minus(Vector3d v);

    float size()
    {
        return sqrt(std::pow(this->x, 2) + std::pow(this->y, 2) + std::pow(this->z, 2));
    }

    float scalar_product(Vector3d v)
    {
        return (this->x * v.x + this->y * v.y + this->z * v.z);
    }

    Vector3d get_vector_normalized()
    {
        return this->divide(this->size());
    }

    bool equals(Vector3d other) {
        return this->x == other.x && this->y == other.y && this->z == other.z;
    }
};

class SourceOfLight
{
public:
    IntensityColor intensity;
    Vector3d center;
    SourceOfLight() {}
    SourceOfLight(IntensityColor intensity, Vector3d center)
    {
        this->intensity = intensity;
        this->center = center;
    }
    SourceOfLight(Color color, Vector3d center)
    {
        this->intensity = color.to_color_normalized();
        this->center = center;
    }
};

std::ostream &operator<<(std::ostream &os, Vector3d const &v)
{
    return os << "Vector(" << v.x << "," << v.y << "," << v.z << ")";
}

Vector3d Vector3d::multiply(float value)
{
    float x = this->x * value;
    float y = this->y * value;
    float z = this->z * value;
    return Vector3d(x, y, z);
}

Vector3d Vector3d::divide(float value)
{
    return this->multiply(1 / value);
}

Vector3d Vector3d::sum(Vector3d v)
{
    float x = this->x + v.x;
    float y = this->y + v.y;
    float z = this->z + v.z;
    return Vector3d(x, y, z);
}

Vector3d Vector3d::minus(Vector3d v)
{
    Vector3d new_v = v.multiply(-1);
    return this->sum(new_v);
}

class Ray
{
public:
    Vector3d p1;
    Vector3d p2;

    Ray() {}

    Ray(Vector3d p1, Vector3d p2)
    {
        this->p1 = p1;
        this->p2 = p2;
    }

    float size()
    {
        return (p2.minus(p1)).size();
    }

    // Unitary direction vector
    Vector3d get_dr()
    {
        return (p2.minus(p1)).divide(this->size());
    }
};

std::ostream &operator<<(std::ostream &os, Ray const &r)
{
    return os << "Ray(P1: " << r.p1 << " | P2: " << r.p2 << ")";
}

class Intersection
{
public:
    float time;
    bool is_valid;

    Intersection() {}
    Intersection(float t, bool valid)
    {
        this->time = t;
        this->is_valid = valid;
    }
};

class Object
{
public:
    // bool intersected_by(Ray ray) {}
    Color color;
    IntensityColor difuse_reflectivity;   // K_d
    IntensityColor specular_reflectivity; // K_e
    IntensityColor environment_reflectivity; // K_a
    float shininess;
    virtual Intersection get_intersection(Ray ray) { return Intersection(0.0, false); }
    virtual Vector3d get_normal_vector(Vector3d intersection_point) {}
    virtual Vector3d get_light_vector(Vector3d intersection_point, SourceOfLight source_of_light) {}

    IntensityColor get_difuse_contribution(Vector3d intersection_point, SourceOfLight source_of_light)
    {

        Vector3d l = this->get_light_vector(intersection_point, source_of_light);
        Vector3d n = this->get_normal_vector(intersection_point);
        float scalar_product_l_n = l.scalar_product(n);

        // if scalar_product is negative, the angle is greater than 180 degrees and we can't see the object. The color in result will be black.
        if (scalar_product_l_n < 0)
        {
            scalar_product_l_n = 0;
        }
        float f_difuse = scalar_product_l_n;
        IntensityColor contribution = source_of_light.intensity.arroba_multiply(this->difuse_reflectivity).multiply(f_difuse);

        return contribution;
    }

    IntensityColor get_specular_contribution(Vector3d intersection_point, Vector3d eye_point, SourceOfLight source_of_light)
    {
        Vector3d l = this->get_light_vector(intersection_point, source_of_light);
        Vector3d n = this->get_normal_vector(intersection_point);

        Vector3d r = n.multiply(2).minus(l).get_vector_normalized();
        Vector3d v = eye_point.minus(intersection_point).get_vector_normalized();

        float f_specular = std::pow(r.scalar_product(v), this->shininess);

        IntensityColor contribution = source_of_light.intensity.arroba_multiply(this->specular_reflectivity).multiply(f_specular);
        return contribution;
    }
};

class Sphere : public Object
{
public:
    Vector3d center;
    float radius;

    Sphere(Vector3d center, float radius, Color color, IntensityColor difuse_reflectivity, IntensityColor specular_reflectivity, IntensityColor environment_reflectivity, float shininess)
    {
        this->center = center;
        this->radius = radius;
        this->color = color;
        this->difuse_reflectivity = difuse_reflectivity;
        this->specular_reflectivity = specular_reflectivity;
        this->environment_reflectivity = environment_reflectivity;
        this->shininess = shininess;
    }

    // n unitary vector (normal vector).
    Vector3d get_normal_vector(Vector3d intersection_point) override
    {
        return (intersection_point.minus(this->center)).divide(this->radius);
    }

    // l unitary vector in direction to light.
    Vector3d get_light_vector(Vector3d intersection_point, SourceOfLight source) override
    {
        return (source.center.minus(intersection_point)).get_vector_normalized();
    }

    Intersection get_intersection(Ray ray) override
    {

        Vector3d initial_point = ray.p1;
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

        float delta = std::pow(b, 2) - (4 * a * c);
        float t1 = (-b + std::sqrt(delta)) / (2 * a);
        float t2 = (-b - std::sqrt(delta)) / (2 * a);

        if (delta < 0)
        {
            return Intersection(0.0, false);
        }

        Vector3d intersection_point;
        if (t1 < t2)
            return Intersection(t1, true);
        return Intersection(t2, true);
    }
};

class Plan: public Object {
public:
    Vector3d known_point;
    Vector3d normal;

    Plan(Vector3d known_point, Vector3d normal, IntensityColor difuse_reflectivity, IntensityColor specular_reflectivity, IntensityColor environment_reflectivity, float shininess, Color color) {
        this->known_point = known_point;
        this->normal = normal;
        this->difuse_reflectivity = difuse_reflectivity;
        this->specular_reflectivity = specular_reflectivity;
        this->environment_reflectivity = environment_reflectivity;
        this->shininess = shininess;
        this->color = color;
    }

    Vector3d get_normal_vector(Vector3d intersection_point) override {
        return this->normal.get_vector_normalized();
    }
    Vector3d get_light_vector(Vector3d intersection_point, SourceOfLight source_of_light) override {
        return source_of_light.center.minus(intersection_point).get_vector_normalized();
    }

    Intersection get_intersection(Ray ray) override {
        Vector3d w = ray.p1.minus(this->known_point);
//        Vector3d w = this->known_point.minus(ray.p2);
        Vector3d dr = ray.get_dr();

        float t_int = -(this->normal.scalar_product(w))/(this->normal.scalar_product(dr));

        return Intersection(t_int, t_int > 0);
    }
};

class Scene
{
public:
    std::vector<Object *> objects;
    Color background_color;
    SourceOfLight source_of_light;
    IntensityColor environment_light;
    Vector3d eye;

    Scene(Color bg_color, SourceOfLight source, IntensityColor environment_light, Vector3d eye)
    {
        this->background_color = bg_color;
        this->source_of_light = source;
        this->environment_light = environment_light;
        this->eye = eye;
    }

    void push_object(Object *obj)
    {
        objects.push_back(obj);
    }

    Color get_color_to_draw(Ray ray)
    {
        float min_time_intersection = 10000000.0;
        int min_index = -1;
        for (int i = 0; i < objects.size(); i++)
        {
            Intersection intersection = objects[i]->get_intersection(ray);
            if (intersection.is_valid)
            {
                if (intersection.time < min_time_intersection)
                {
                    min_time_intersection = intersection.time;
                    min_index = i;
                }
            }
        }

        if (min_index == -1)
            return this->background_color;

        Object *obj = objects[min_index];
        Color color = obj->color;

        // Pin + t*dr
        Vector3d intersection_point = ray.p1.sum(ray.get_dr().multiply(min_time_intersection));

        // Check if the object is seen by pontual light
        Ray ray_light(this->source_of_light.center, intersection_point);

        float min_t2 = 10000000.0;
        Object *min_obj = objects[0];

        for (auto & object : objects)
        {
            Intersection intersection2 = object->get_intersection(ray_light);
            if (intersection2.is_valid)
            {
                if (intersection2.time < min_t2)
                {
                    min_t2 = intersection2.time;
                    min_obj = object;
                }
            }
        }

        Vector3d intersection_point2 = ray_light.p1.sum(ray_light.get_dr().multiply(min_t2));

        // If intersection times are not equal, there is something interrupting light to get into that point, so we discard difuse and specular contributions.
        if (min_obj != obj) {
            IntensityColor environment_contrib = this->environment_light.arroba_multiply(obj->environment_reflectivity);

            return color.multiply(environment_contrib);
        }

        IntensityColor difuse_contrib = obj->get_difuse_contribution(intersection_point, source_of_light);
        IntensityColor specular_contrib = obj->get_specular_contribution(intersection_point, this->eye, source_of_light);
        IntensityColor environment_contrib = this->environment_light.arroba_multiply(obj->environment_reflectivity);

        IntensityColor result = environment_contrib.sum(difuse_contrib).sum(specular_contrib);


        return color.multiply(result);
    }

    void dealloc_objects()
    {
        for (auto &obj : objects)
        {
            delete obj;
        }
    }
};

class Window
{
public:
    Vector3d center;
    float width;
    float height;

    int cols;
    int rows;

    // dx and dy of rectangles
    float dx;
    float dy;

    std::vector<std::vector<Color>> windows_colors;
    bool should_update = true;

    Window() {}
    Window(float width, float height, int cols, int rows, float x, float y, float z)
    {
        this->width = width;
        this->height = height;
        this->cols = cols;
        this->rows = rows;
        this->center = Vector3d(x, y, z);

        this->dx = width / (float)cols;
        this->dy = height / (float)rows;

        for (int i = 0; i < rows; i++)
        {
            std::vector<Color> row;
            for (int j = 0; j < cols; j++)
            {
                row.push_back(Color(100, 100, 100));
            }
            windows_colors.push_back(row);
        }
    }
};


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
        "Computação Gráfica I - Bruno e Vitor",
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
                cout << cretos_window->center << endl;
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

int main(int argc, char *argv[])
{
    // window width and height will be 1.0 meter. We will render everything in a SDL window with pixes specified.
    return render_picture(500, 500, 500, 500, .6, .6);
}
