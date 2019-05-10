#ifndef TTS_SAMPLE_H_
#define TTS_SAMPLE_H_

#include "cae_lib.h"

class TTSProcess
{
public:
    static bool textToSpeech(const char* text, void* args);
	static void play_feedback_pcm(const char* text, void* args);
};

#endif // TTS_SAMPLE_H_
