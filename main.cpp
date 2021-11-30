#include "SDL2/SDL.h"

#include "SDL2/SDL2_gfxPrimitives.h"
#include <list>
#include <iostream>
#include <random>
#include "mat2.h"
#define G 6.6743e-11
#define NUM_OBJS 200

//////////////////////////////////////////////////////////////
// basic math and structs
//////////////////////////////////////////////////////////////


inline const int& max(const int& i, const int& j)
{
  if (i > j)
    return i;
  return j;
}

struct vec
{
  double x,y;

  vec& operator +=(const vec& rhs)
  {
    x += rhs.x;
    y += rhs.y;
    return *this;
  }

  vec& operator -=(const vec& rhs)
  {
    x -= rhs.x;
    y -= rhs.y;
    return *this;
  }

  vec& operator *= (const double& fac)
  {
    x *= fac;
    y *= fac;
    return *this;
  }

  vec& operator /= (const double& fac)
  {
    x /= fac;
    y /= fac;
    return *this;
  }
  
  friend vec operator+ (vec vec1, const vec& vec2)
  {
    vec1.x += vec2.x;
    vec1.y += vec2.y;
    return vec1;
  }

  friend vec operator- (vec vec1, const vec& vec2)
  {
    vec1.x -= vec2.x;
    vec1.y -= vec2.y;
    return vec1;
  }

  friend vec operator* (vec vec1, double fac)
  {
    vec1.x *= fac;
    vec1.y *= fac;
    return vec1;
  }
  
  friend vec operator* (double fac, vec vec1)
  {
    vec1.x *= fac;
    vec1.y *= fac;
    return vec1;
  }

  friend vec operator/ (vec vec1, double fac)
  {
    vec1.x /= fac;
    vec1.y /= fac;

    return vec1;
  }
  
  friend std::ostream& operator<<(std::ostream& os, const vec& vec1)
  {
    os << "(" << vec1.x << ", " << vec1.y << ")";
    return os;
  }

  friend vec operator* (mat2 mat, vec vec1)
  {
    vec res;
    res.x = vec1.x * mat.a + vec1.y * mat.b;
    res.y = vec1.x * mat.c + vec1.y * mat.d;
    return res;
  }

};

struct object
{
  double    mass, radius;
  vec       pos, v;
  SDL_Color color;
};

inline double len(vec vector)
{
  return sqrtl(vector.x * vector.x + vector.y * vector.y);
}

inline double squarelen(vec vector)
{
  return vector.x * vector.x + vector.y * vector.y;
}

inline double dotprod(vec v1, vec v2)
{
  return v1.x * v2.x + v1.y * v2.y;
}

enum tracetype
  {
    none,
    circletrace,
    linetrace
  };

/////////////////////////////////////////////////
// physics code
/////////////////////////////////////////////////

void update_velocities(object* obj1, object* obj2, double dt = 0.01)
{
  // gravity code
  // Acceleration = Force over mass = G * prod(masses) / mass -> 
  // Use force without masses
  vec direction = obj1->pos - obj2->pos;
  double dist = len(direction);
  vec force = -G / (dist * dist * dist) * direction;
  
  obj1->v += dt * force * obj2->mass;
  obj2->v -= dt * force * obj1->mass;
  
}

void update_pos(object* obj, double dt)
{
  obj->pos += obj->v * dt;

}

bool objects_collide(object* obj1, object* obj2, double epsilon = 0.0)
{
  double dist = len(obj1->pos - obj2->pos);
  
  if (dist <= obj1->radius + obj2->radius + epsilon)
    return true;
  return false;
}

void handle_collision(object* obj1, object* obj2, double dt)
{
  // physics basics
  vec direction = obj1->pos - obj2->pos;
  double d = len(direction);
  direction /= d; // Unit vector in direction of distance
  double cold = obj1->radius + obj2->radius + 0.01;
  
  // collision code
  if (d < cold)
  {
    vec dv = obj1->v - obj2->v;
    double t = (d - cold) / dotprod(dv, direction); // time since collision

    // go back to moment of collision
    obj1->pos -= obj1->v * t;
    obj2->pos -= obj2->v * t;
    
    // Velocity parallel to distance vector, scaled by masses
    vec vpar = 2.0 * direction * dotprod(dv, direction) / (obj1->mass + obj2->mass);

    obj1->v -= vpar * obj2->mass;
    obj2->v += vpar * obj1->mass;

    // we progress by the amount of time since the collision
    obj1->pos += obj1->v * t;
    obj2->pos += obj2->v * t;
  }
}

//////////////////////////////////////////////////
// main
//////////////////////////////////////////////////

int main()
{
  SDL_Init(SDL_INIT_EVERYTHING);
  SDL_Window* win;
  SDL_Renderer* ren;

  
  win = SDL_CreateWindow("Gravity", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, 0);
  ren = SDL_CreateRenderer(win, 0, 0);
  
  if (win == NULL || ren == NULL)
    SDL_Log("%s", SDL_GetError());

  SDL_SetRenderDrawColor(ren, 20, 20, 40, 255);
  SDL_RenderClear(ren);
  SDL_RenderPresent(ren);

  object objects[NUM_OBJS];
  // Parameters: Mass,  Radius,     Position, Velocity, Color (RGBA)
  objects[0] = {1e18,     10.0, {   0,    0}, {0,   0}, {255,   0,   0, 0}}; // Red 'Sun'
  objects[1] = {100e12,    4.0, { 100,    0}, {0, 100}, {  0, 100,   0, 0}}; // Green 'Earth' 
  #if NUM_OBJS > 2
  objects[2] = {  20e9,    1.0, {-100, -100}, {2,   0}, {  0, 255, 255, 0}}; // Cyan 'Comet'
  
  std::default_random_engine generator;
  std::normal_distribution<double> radius_gen(300.0,20.0);
  std::normal_distribution<double> scale_gen(1.0,0.1);
  for (int i = 3; i < NUM_OBJS; i++)
  {
    mat2 rot = {0, -1, 1, 0};
    double radius, angle, scale;
    bool collision = true;
    int count = 0;
    while (collision){
      radius = radius_gen(generator);
      angle  = rand() * 2 * M_PI / RAND_MAX;
      scale  = scale_gen(generator);
      objects[i] = {
        100e12 * scale / 2.0, 4 * scale,              // Mass and size
        {radius * sin(angle), radius * cos(angle)},  // Position
        {0,0}, {255, 255, 255, 0}};                   // Velocity and color
      // Detect if there is already a star too close by
      collision = false;
      for (int k = 0; k < i && !collision; ++k)
        collision |= objects_collide(objects + i, objects + k, 0.0);
      if (count++ > 10)
      {
        std::cerr << "Tried more than 10 times to find a suitable spot for new Star. No success...\n";
        break;
      }
    }
    // Give all stars a slight rotation
    objects[i].v = rot * objects[i].pos / sqrt(len(objects[i].pos)) * 3.0e1;
  }
  #endif
  
  // Main loop
  SDL_Event e;
  bool running = true;
  bool pause = true;
  double dt = 0.001;
  const uint8_t* keys = SDL_GetKeyboardState(NULL);
  
  while (running)
  {
    while (SDL_PollEvent(&e))
    {
      if (e.type == SDL_QUIT)
        running = false;
      if (e.type == SDL_KEYUP)
      {
        if (e.key.keysym.sym == SDLK_SPACE) pause = !pause;
      }
    }
    
    // Update screen
    SDL_SetRenderDrawColor(ren, 20, 20, 40, 255);
    SDL_RenderClear(ren);
    for (int i = 0; i < NUM_OBJS; i++)
      filledCircleRGBA(ren, objects[i].pos.x + 640, 360 - objects[i].pos.y, objects[i].radius,
        objects[i].color.r, objects[i].color.g, objects[i].color.b, 255);
    SDL_RenderPresent(ren);
    
    // Restrict to 60 FPS
    SDL_Delay(1000 / 60);
    sim_time += dt;

    if (pause) continue;

    // update physics
    for (int i = 0; i < NUM_OBJS; i++)
      update_pos(objects + i, dt);
    
    for (int i = 1; i < NUM_OBJS; i++)
      for (int j = 0; j < i; j++)
        handle_collision(objects + i, objects + j, dt);
    
    for (int i = 1; i < NUM_OBJS; i++)
      for (int j = 0; j < i; j++)
        update_velocities(objects + i, objects + j, dt);
    
  }
  
  return 0;
  
  
}






