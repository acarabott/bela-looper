#include <Bela.h>
#include <stdlib.h>
#include <Midi.h>
#include <Utilities.h>

#include "LoopLayer.h"

#define DEBUG

#define NUM_LAYERS 10
#define NUM_CHANNELS 2

Midi midi;
unsigned int gMidiPort = 0;

uint16_t gCurrentLayer = 0;

LoopLayer layers[NUM_LAYERS];
uint32_t gBufferIdx = 0;
uint16_t gInputChannel = 0;

void midiMessageCallback(MidiChannelMessage message, void* port) {
    if (message.getType() == kmmControlChange) {
        midi_byte_t controlChange = message.getDataByte(0);
        midi_byte_t value = message.getDataByte(1);

        // sustain pedal
        if (controlChange == 64) {
            layers[gCurrentLayer].toggleRecording();
        }
        // volume
        else if (controlChange == 7) {
            layers[gCurrentLayer].setMul(map(value, 0, 127, 0, 1));

            #ifdef DEBUG
                printf("layer %d volume: %f\n", gCurrentLayer,
                    layers[gCurrentLayer].getMul());
            #endif
        }
        // modulation
        else if (controlChange == 1 && value == 127) {
            #ifdef DEBUG
                printf("deleting layer %d\n", gCurrentLayer);
            #endif
            layers[gCurrentLayer].erase();
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

    return true;
}

void render(BelaContext *context, void *userData)
{
    for (uint32_t n = 0; n < context->audioFrames; n++) {
        float inputSignal = audioRead(context, n, gInputChannel);

        float layerSignal = 0;
        // record into layers
        for (uint16_t layer = 0; layer < NUM_LAYERS; layer++) {
            if (layers[layer].recordEnabled()) {
                layers[layer].write(gBufferIdx, inputSignal);
            }

            // sum all layers
            layerSignal += layers[layer].read(gBufferIdx);
        }

        // combine input pass through and recorded layers
        float outputSignal = (inputSignal * layers[gCurrentLayer].getMul()) +
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