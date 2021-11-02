#include "SDL2/SDL.h"

#include "SDL2/SDL2_gfxPrimitives.h"
#include <list>

#define G 0.000000000066743
#define NUM_OBJS 2

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
  long double mass, radius;
  vec pos,v;
};

inline long double len(vec vector)
{
  return sqrtl(powl(vector.x, 2) + powl(vector.y, 2));
}

inline long double squarelen(vec vector)
{
  return vector.x * vector.x + vector.y * vector.y;
}

typedef enum tracetype
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
  // physics basics
  vec direction = obj1->pos - obj2->pos;
  long double d = len(direction);
  long double cold = obj1->radius + obj2->radius + 0.001;
  // collision code
  
  if (d < cold)
  {
    vec dv = obj1->v - obj2->v;
    long double t = (cold - d) / len(dv); // time since collision

    // go back to moment of collision
    obj1->pos -= obj1->v * t;
    obj2->pos -= obj2->v * t;

    // velocity parallel to distance vector
    vec vpar = direction * ((direction.x * dv.x + direction.y * dv.y) / (cold * cold));
    // velocity perpendicular to distance vector
    vec vprp =  dv - vpar; 

    // new vpar for circle 1 & 2
    vec vp1 = vpar * (obj1->mass -  obj2->mass) / (obj1->mass + obj2->mass); 
    vec vp2 = vpar * (obj1->mass + obj1->mass) / (obj1->mass + obj2->mass);

    obj1->v = vp1 + vprp + obj2->v;
    obj2->v = vp2 + vprp + obj2->v;

    // we progress by the amount of time since the collision
    obj1->pos += obj1->v * t;
    obj2->pos += obj2->v * t;

  }
  
  // gravity code
  double force = G / (powl(obj1->pos.x - obj2->pos.x ,2) + powl(obj1->pos.y - obj2->pos.y, 2) + 0.01);

  obj2->v += direction * dt * force * obj1->mass;
  obj1->v -= direction * dt * force * obj2->mass;

  
  
}

void update_pos(object* obj, double dt = 0.01)
{
  obj->pos += obj->v * dt;
}

//////////////////////////////////////////////////
// main
//////////////////////////////////////////////////

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


  long double tot_mass = 0;
  vec center_of_mass = {0,0};
  
  std::list<vec> trace[NUM_OBJS];
  object objects[NUM_OBJS];
  objects[0] = {10000000000000000,40 , {0,0}, {0,.1}};
  objects[1] = {10, 4, {500, 0}, {0,200}};
  //objects[2] = {2000000000000, 1.5, {-100, -100}, {2, 0}};

  for (int i = 3; i < NUM_OBJS; i++)
    objects[i] = {100000.*rand(), 4, {rand() % 1000 -500., rand() % 500 - 250.}, {0,0}};

  for (int i = 0; i < NUM_OBJS; i++) tot_mass += objects[i].mass;
  
  
  SDL_Color color[NUM_OBJS] = {{255, 0, 0, 0},{0, 255, 0, 0}/*, {0,0,255,0}*/};

  for (int i = 0; i < NUM_OBJS; i++)
    trace[i].push_front(objects[i].pos);

  SDL_Event e;
  bool running = true;
  bool pause = true;
  tracetype tracing = none;
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
	if (e.key.keysym.sym == SDLK_t) if (tracing == none) tracing = circletrace; else tracing = none;
      }
    }

    if (pause) continue;

    // update physics
    for (int i = 0; i < NUM_OBJS; i++)
      for (int j = 0; j < i; j++)
	update_velocities(objects + i, objects + j);

    for (int i = 0; i < NUM_OBJS; i++)  update_pos(objects + i);

    // calculate center of mass
    for (int i = 0; i < NUM_OBJS; i++) center_of_mass += objects[i].pos * objects[i].mass;
    center_of_mass /= tot_mass;
    
    // do drawing
    SDL_SetRenderDrawColor(ren, 20, 20, 40, 255);
    SDL_RenderClear(ren);
    for (int i = 0; i < NUM_OBJS; i++)
    {
      SDL_SetRenderDrawColor(ren, color[i].r, color[i].g, color[i].b, color[i].a);
      if (len(trace[i].front() - objects[i].pos) >objects[i].radius)
      {
        trace[i].push_front(objects[i].pos);
        if (trace[i].size() > 128) trace[i].pop_back();
      }
      int j = 128;
      switch(tracing)
      {
      case none:
	break;
	
      case circletrace:
	for (vec iter : trace[i])
	  filledCircleColor(ren, iter.x + 640, 360 - iter.y, objects[i].radius, *(unsigned int*)(color + i) + (max(j--, 0) <<  24));
	break;

      case linetrace:
	SDL_FPoint* points = new SDL_FPoint[trace[i].size()];
	int k = 0;
	for (vec iter : trace[i])
	  points[k++] = {float(iter.x + 640), float(360 - iter.y)};
	
	SDL_RenderDrawLinesF(ren, points, k);
		    
		       

      }
	  
      
      filledCircleColor(ren, objects[i].pos.x + 640, 360 - objects[i].pos.y,
			objects[i].radius,
			*(unsigned int*)(color + i) + (0xff <<  24));
    }

    objects[0].pos += vec({1. * (keys[SDL_SCANCODE_D] - keys[SDL_SCANCODE_A]),
	1. * (keys[SDL_SCANCODE_W] - keys[SDL_SCANCODE_S])}) * 0.01;
    

    SDL_RenderPresent(ren);
  }
  
  return 0;
  
  
}






