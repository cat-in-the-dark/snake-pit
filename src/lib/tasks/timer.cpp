#include "timer.h"

Timer::Timer(float time) : time(time), elapsed(0) {}

bool Timer::IsPassed() { return elapsed >= time; }

void Timer::Update(float dt) { elapsed += dt; }

void Timer::Reset() { elapsed = 0; }

float Timer::Elapsed() { return elapsed; }
