#include "SDL2/SDL.h"

#include "SDL2/SDL2_gfxPrimitives.h"
#include <list>

#define G 0.000000000066743
#define NUM_OBJS 3

inline const int& max(const int& i, const int& j)
{
  if (i > j)
    return i;
  return j;
}

struct vec
{
  long double x,y;


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

  vec& operator *= (const long double& fac)
  {
    x *= fac;
    y *= fac;
    return *this;
  }

  vec& operator /= (const long double& fac)
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

  friend vec operator* (vec vec1, long double fac)
  {
    vec1.x *= fac;
    vec1.y *= fac;
    return vec1;
  }

  friend vec operator/ (vec vec1, long double fac)
  {
    vec1.x /= fac;
    vec1.y /= fac;

    return vec1;
  }
};

struct object
{
  long double mass;
  vec pos,v;
};

inline long double len(vec vector)
{
  return sqrtl(powl(vector.x, 2) + powl(vector.y, 2));
}

void update_velocities(object* obj1, object* obj2, double dt = 0.01)
{
  double force = G / (powl(obj1->pos.x - obj2->pos.x ,2) + powl(obj1->pos.y - obj2->pos.y, 2));
  vec direction = obj1->pos - obj2->pos;
  direction /= len(direction);

  obj2->v += direction * dt * force * obj1->mass;
  obj1->v -= direction * dt * force * obj2->mass;
}

void update_pos(object* obj, double dt = 0.01)
{
  obj->pos += obj->v * dt;
}

int main()
{
  SDL_Init(SDL_INIT_EVERYTHING);
  SDL_Window* win;
  SDL_Renderer* ren;

  
  win = SDL_CreateWindow("space", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, 0);
  ren = SDL_CreateRenderer(win, 0, 0);

  if (win == NULL || ren == NULL)
    SDL_Log("%s", SDL_GetError());

  SDL_SetRenderDrawColor(ren, 20, 20, 40, 255);
  SDL_RenderClear(ren);
  SDL_RenderPresent(ren);

  std::list<vec> trace[NUM_OBJS];
  object objects[NUM_OBJS];
  objects[0] = {10000000000000, {0,0}, {0,.1}};
  objects[1] = {1000000000000, {100, 100}, {0,-2}};
  objects[2] = {2000000000000, {-100, -100}, {2, 0}};
  SDL_Color color[NUM_OBJS] = {{255, 0, 0, 0},{0, 255, 0, 0}, {0,0,255,0}};

  for (int i = 0; i < NUM_OBJS; i++)
    trace[i].push_front(objects[i].pos);

  SDL_Event e;
  bool running = true;
  bool pause = true;
  while (running)
  {
    while (SDL_PollEvent(&e))
    {
      if (e.type == SDL_QUIT)
	running = false;
      if (e.type == SDL_KEYUP)
	if (e.key.keysym.sym == SDLK_SPACE)
	  pause = !pause;
    }

    if (pause) continue;
      
    for (int i = 0; i < NUM_OBJS; i++)
      for (int j = 0; j < i; j++)
      {
	update_velocities(objects + i, objects + j);
      }

    SDL_SetRenderDrawColor(ren, 20, 20, 40, 255);
    SDL_RenderClear(ren);
    for (int i = 0; i < NUM_OBJS; i++)
    {
      update_pos(objects + i);
      if (len(trace[i].front() - objects[i].pos) > 1)
      {
        trace[i].push_front(objects[i].pos);
        if (trace[i].size() > 128) trace[i].pop_back();
      }
      SDL_SetRenderDrawColor(ren, color[i].r, color[i].g, color[i].b, color[i].a);
      int j = 128;
      for (vec iter : trace[i])
	filledCircleColor(ren, iter.x + 640, 360 - iter.y, powl(objects[i].mass, 1./3.) / 10000, *(unsigned int*)(color + i) + (max(j--, 0) <<  24));

      filledCircleColor(ren, objects[i].pos.x + 640, 360 - objects[i].pos.y, powl(objects[i].mass, 1./3.) / 10000, *(unsigned int*)(color + i) + (0xff <<  24));
    }

    SDL_RenderPresent(ren);
  }
  
  return 0;
  
  
}






