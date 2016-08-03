#include <Bela.h>
#include <Midi.h>

#define NUM_CHANNELS 2
// #define BUFFER_SIZE 2646000 // 60 seconds
#define BUFFER_SIZE 44100 // 1 seconds

Midi midi;
unsigned int gMidiPort = 0;

bool gRecording = false;
uint32_t gRecordIdx = 0;
float gBuffer[NUM_CHANNELS][BUFFER_SIZE] = {{0}, {0}};

void midiMessageCallback(MidiChannelMessage message, void* port) {
    if(message.getType() == kmmControlChange) {
        if (message.getDataByte(0) == 64) {
            gRecording = message.getDataByte(1) == 127;
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
    midi.readFrom(gMidiPort);
    midi.writeTo(gMidiPort);
    midi.enableParser(true);
    midi.setParserCallback(midiMessageCallback, &gMidiPort);

    return true;
}

void render(BelaContext *context, void *userData)
{
    for (uint32_t n = 0; n < context->audioFrames; n++) {
        for (uint32_t ch = 0; ch < context->audioOutChannels; ch++) {
            if (gRecording) {
                gBuffer[ch][gRecordIdx] = audioRead(context, n, ch);
            }
        }
        if (gRecording) {
            gRecordIdx = (gRecordIdx + 1) % BUFFER_SIZE;
        }
    }
}

void cleanup(BelaContext *context, void *userData)
{

}