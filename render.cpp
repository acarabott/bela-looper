#define DEBUG

#include <Bela.h>
#include <stdlib.h>
#include <Midi.h>
#include <OSCServer.h>
#include <OSCClient.h>
#include <Utilities.h>
#ifdef DEBUG
    #define __STDC_FORMAT_MACROS
    #include <inttypes.h>
#endif


#include "LoopLayer.h"

#define NUM_LAYERS 10
#define NUM_CHANNELS 2

Midi midi;
unsigned int gMidiPort = 0;

LoopLayer layers[NUM_LAYERS];
uint16_t gCurrentLayer = 0;
uint64_t currentFrame = 0;
uint16_t gInputChannel = 0;

OSCServer oscServer;

float tempo = 60;
uint32_t beatCount = 0;

void midiMessageCallback(MidiChannelMessage message, void* port) {
    if (message.getType() == kmmControlChange) {
        midi_byte_t controlChange = message.getDataByte(0);
        midi_byte_t value = message.getDataByte(1);

        // sustain pedal
        if (controlChange == 64) {
            #ifdef DEBUG
                printf("recording on layer %d\n", gCurrentLayer);
            #endif
            layers[gCurrentLayer].toggleRecording(currentFrame);
        }
        // volume
        else if (controlChange == 7) {
            layers[gCurrentLayer].setMul(map(value, 0, 127, 0, 1));
        }
        // modulation
        else if (controlChange == 1 && value == 127) {
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

        // tempo control
        if (noteNum >= 60 && noteNum <= 72) {
            tempo = map(noteNum, 60, 72, 60, 120);
        }
    }

    #ifdef DEBUG
        message.prettyPrint();
    #endif
}

void oscMessageCallback(oscpkt::Message message)
{
    rt_printf("received message to: %s\n", message.addressPattern().c_str());

    // Tempo messages
    int32_t tempoInt;
    float tempoFloat;
    float prevTempo = tempo;
    if (message.match("/tempo").popInt32(tempoInt).isOkNoMoreArgs()) {
        tempo = max(tempoInt, 1);
    }
    else if (message.match("/tempo").popFloat(tempoFloat).isOkNoMoreArgs()) {
        tempo = max(tempoFloat, 1);
    }

    if (tempo != prevTempo) {
        rt_printf("tempo changed to %f\n", tempo);
    }

    // recording messages

    int32_t recordLayer;
    int32_t recordEnableArg;
    int32_t recordQuantiseArg = 0;

    oscpkt::Message::ArgReader recordReader = message.match("/record");
    if (recordReader.popInt32(recordLayer).popInt32(recordEnableArg).isOk()) {
        bool recordEnable = recordEnableArg != 0;
        #ifdef DEBUG
            std::string mode = recordEnable ? "START" : "STOP";
            rt_printf("channel %d recording %s", recordLayer, mode.c_str());
        #endif

        recordReader.popInt32(recordQuantiseArg);
        bool recordQuantise = recordQuantiseArg != 0;

        #ifdef DEBUG
            std::string quantise = recordQuantise != 0 ? "Quantise" : "Free";
            rt_printf(" - %s\n", quantise.c_str());
        #endif

        // schedule / enable recording
        LoopLayer& layer = layers[recordLayer];
        if (recordEnable) {
            if (recordQuantise) {
                layer.scheduleRecordingStart();
            } else {
                layer.startRecording(currentFrame);
            }
        } else {
            if (recordQuantise) {
                layer.scheduleRecordingStop();
            } else {
                layer.stopRecording(currentFrame);
            }
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

    // OSC setup
    oscServer.setup(6666);

    return true;
}

bool checkBeat(uint64_t audioFramesElapsed, float audioSampleRate)
{
    const uint64_t beatFrame = (60.0 / tempo) * audioSampleRate;
    return audioFramesElapsed % beatFrame == 0;
}

void render(BelaContext *context, void *userData)
{
    // listen for OSC
    while (oscServer.messageWaiting()) {
        oscMessageCallback(oscServer.popMessage());
    }

    // audio loop
    for (uint32_t n = 0; n < context->audioFrames; n++) {
        currentFrame = context->audioFramesElapsed + n;
        bool beat = checkBeat(currentFrame, context->audioSampleRate);

        if (beat) {
            for (uint16_t l = 0; l < NUM_LAYERS; l++) {
                LoopLayer& layer = layers[l];

                if (layer.recordingStartScheduled()) {
                    layer.startRecording(currentFrame);
                }
                else if (layer.recordingStopScheduled()) {
                    layer.stopRecording(currentFrame);
                }
            }
            beatCount++;
        }

        const float inputSignal = audioRead(context, n, gInputChannel);

        float layerSignal = 0;
        // record into layers
        for (uint16_t l = 0; l < NUM_LAYERS; l++) {
            LoopLayer& layer = layers[l];
            if (layer.recordEnabled()) {
                layer.write(currentFrame, inputSignal);
            }

            // sum all layers
            layerSignal += layer.read(currentFrame);
        }

        // combine input pass through and recorded layers
        float outputSignal = layerSignal;
        outputSignal += (inputSignal * layers[gCurrentLayer].getMul());

        // output
        for (uint32_t ch = 0; ch < context->audioOutChannels; ch++) {
            audioWrite(context, n, ch, outputSignal);

            if(beat) {
                float noise = 0.01 * (rand() / (float)RAND_MAX * 2 - 1);
                audioWrite(context, n, ch, noise);
            }
        }
    }
}

void cleanup(BelaContext *context, void *userData)
{

}