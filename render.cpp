#include <Bela.h>
#include <Midi.h>

#define NUM_CHANNELS 2
// #define BUFFER_SIZE 2646000 // 60 seconds
#define BUFFER_SIZE 44100 // 1 seconds

Midi midi;
unsigned int midiPort = 0;
bool recording = false;

float gBuffer[NUM_CHANNELS][BUFFER_SIZE] = {{0}, {0}};

void midiMessageCallback(MidiChannelMessage message, void* port) {
    if(message.getType() == kmmControlChange) {
        if (message.getDataByte(0) == 64) {
            recording = message.getDataByte(1) == 127;
        }
    }
}

bool setup(BelaContext *context, void *userData)
{
    // ensure audio channels
    if (context->audioInChannels != context ->audioOutChannels) {
        printf("Error: for this project we need matching number of audio I/O");
        return false;
    }

    // midi setup
    midi.readFrom(midiPort);
    midi.writeTo(midiPort);
    midi.enableParser(true);
    midi.setParserCallback(midiMessageCallback, &midiPort);

    return true;
}

void render(BelaContext *context, void *userData)
{

}

void cleanup(BelaContext *context, void *userData)
{

}