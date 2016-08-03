#include <Bela.h>
#include <stdlib.h>
#include <Midi.h>
#include <Utilities.h>

#define DEBUG

#define NUM_LAYERS 10
#define NUM_CHANNELS 2
#define BUFFER_SIZE 132300 // 3 seconds
// #define BUFFER_SIZE 2646000 // 60 seconds

Midi midi;
unsigned int gMidiPort = 0;

uint16_t gCurrentLayer = 0;
bool gRecording = false;
float gBuffer[NUM_LAYERS][BUFFER_SIZE] = {{0}};
float gLayerMuls[NUM_LAYERS];
uint32_t gBufferIdx = 0;
uint16_t gInputChannel = 0;


void deleteLayer(uint16_t layerNum) {
    std::fill(gBuffer[layerNum], gBuffer[layerNum] + BUFFER_SIZE, 0);
}

void midiMessageCallback(MidiChannelMessage message, void* port) {
    if (message.getType() == kmmControlChange) {
        midi_byte_t controlChange = message.getDataByte(0);
        midi_byte_t value = message.getDataByte(1);

        // sustain pedal
        if (controlChange == 64) {
            gRecording = value == 127;

            #ifdef DEBUG
                if (gRecording) {
                    printf("recording on layer %d\n", gCurrentLayer);
                } else {
                    printf("stopped recording\n");
                }
            #endif
        }
        // volume
        else if (controlChange == 7) {
            gLayerMuls[gCurrentLayer] = map(value, 0, 127, 0, 1);

            #ifdef DEBUG
                printf("layer %d volume: %f\n", gCurrentLayer,
                    gLayerMuls[gCurrentLayer]);
            #endif
        }
        // modulation
        else if (controlChange == 1 && value == 127) {
            #ifdef DEBUG
                printf("deleting layer %d\n", gCurrentLayer);
            #endif
            deleteLayer(gCurrentLayer);
        }

    }
    else if (message.getType() == kmmNoteOn) {
        midi_byte_t noteNum = message.getDataByte(0);
        midi_byte_t velocity = message.getDataByte(1);

        if (noteNum >= 0 && noteNum < NUM_LAYERS && velocity > 0) {
            gCurrentLayer = noteNum;

            #ifdef DEBUG
                printf("current layer set to %d\n", gCurrentLayer);
            #endif
        }
    } else {
        #ifdef DEBUG
            message.prettyPrint();
        #endif
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

    // fill muls array
    std::fill(gLayerMuls, gLayerMuls + NUM_LAYERS, 1);

    return true;
}

void render(BelaContext *context, void *userData)
{
    for (uint32_t n = 0; n < context->audioFrames; n++) {
        float inputSignal = audioRead(context, n, gInputChannel);
        // record into current layer
        if (gRecording) {
            gBuffer[gCurrentLayer][gBufferIdx] += inputSignal;
            // float noise = 0.01 * (rand() / (float)RAND_MAX * 2 - 1);
            // gBuffer[gCurrentLayer][gBufferIdx] += noise;
        }

        // sum all layers
        float layerSignal = 0;
        for (uint16_t layer = 0; layer < NUM_LAYERS; layer++) {
            layerSignal += gBuffer[layer][gBufferIdx] * gLayerMuls[layer];
        }

        // combine input pass through and recorded layers
        float outputSignal = (inputSignal * gLayerMuls[gCurrentLayer]) +
                             layerSignal;

        // output
        for (uint32_t ch = 0; ch < context->audioOutChannels; ch++) {
            audioWrite(context, n, ch, outputSignal);
        }

        // move buffer playhead
        gBufferIdx = (gBufferIdx + 1) % BUFFER_SIZE;
    }
}

void cleanup(BelaContext *context, void *userData)
{

}