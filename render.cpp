#include <Bela.h>
#include <Midi.h>

Midi midi;
unsigned int midiPort = 0;

bool recording = false;

void midiMessageCallback(MidiChannelMessage message, void* port) {
    if(message.getType() == kmmControlChange) {
        if (message.getDataByte(0) == 64) {
            recording = message.getDataByte(1) == 127;
        }
    }
}

bool setup(BelaContext *context, void *userData)
{
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