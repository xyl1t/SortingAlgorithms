#ifndef BEEPER_HPP
#define BEEPER_HPP

#if __has_include("SDL2/SDL.h")
#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>
#else
#include <SDL.h>
#include <SDL_audio.h>
#endif

#include <queue>
#include <cmath>

const int AMPLITUDE = 28000;
const int FREQUENCY = 44100;

struct BeepObject
{
    double freq;
    int samplesLeft;
};

class Beeper
{
private:
    double v;
    std::queue<BeepObject> beeps;
public:
    Beeper();
    ~Beeper();
    void beep(double freq, int duration);
    void generateSamples(Sint16 *stream, int length);
    void wait();
};

void audio_callback(void*, Uint8*, int);

#endif