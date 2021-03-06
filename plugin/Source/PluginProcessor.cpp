/*
  ==============================================================================

    This file was auto-generated by the Introjucer!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

const char* TIAAudioProcessor::paramPulse1Level      = "level1";
const char* TIAAudioProcessor::paramPulse1Shape      = "shape1";
const char* TIAAudioProcessor::paramPulse1Tune       = "tune1";
const char* TIAAudioProcessor::paramPulse1Fine       = "fine1";

const char* TIAAudioProcessor::paramPulse2Level      = "level2";
const char* TIAAudioProcessor::paramPulse2Shape      = "shape2";
const char* TIAAudioProcessor::paramPulse2Tune       = "tune2";
const char* TIAAudioProcessor::paramPulse2Fine       = "fine2";

//==============================================================================
String percentTextFunction (const slParameter& p, float v)
{
    return String::formatted("%.0f%%", v / p.getUserRangeEnd() * 100);
}

String enableTextFunction (const slParameter& p, float v)
{
    return v > 0.0f ? "On" : "Off";
}

String dutyTextFunction (const slParameter& p, float v)
{
    const int duty = int (v);
    switch (duty)
    {
        case 0: return "12.5%";
        case 1: return "25%";
        case 2: return "50%";
        case 3: return "75%";
    }
    return "";
}

String arTextFunction (const slParameter& p, float v)
{
    return String::formatted("%.1f s", v * 1.0/64.0 * 16);
}

String stTextFunction (const slParameter& p, float v)
{
    String str;
    switch (abs (int (v)))
    {
        case 0: str = "Off"; break;
        case 1: str = "7.8 ms"; break;
        case 2: str = "15.6 ms"; break;
        case 3: str = "23.4 ms"; break;
        case 4: str = "31.3 ms"; break;
        case 5: str = "39.1 ms"; break;
        case 6: str = "46.9 ms"; break;
        case 7: str = "54.7 ms"; break;
    }
    
    if (v < 0)
        str = "-" + str;
    
    return str;
}

String stepTextFunction (const slParameter& p, float v)
{
    return v > 0.0f ? "15" : "7";
}

String intTextFunction (const slParameter& p, float v)
{
    return String (int (v));
}

//==============================================================================
TIAAudioProcessor::TIAAudioProcessor()
{
    addPluginParameter (new slParameter (paramPulse1Tune,      "Pulse 1 Tune",       "Tune",        "", -48.0f, 48.0f, 1.0f, 0.0f, 1.0f, intTextFunction));
    addPluginParameter (new slParameter (paramPulse1Fine,      "Pulse 1 Tune Fine",  "Fine",        "", -100.0f, 100.0f, 1.0f, 0.0f, 1.0f, intTextFunction));
    addPluginParameter (new slParameter (paramPulse1Level,     "Pulse 1 Level",      "Level",       "",  0.0f,  15.0f, 1.0f, 0.0f, 1.0f));
    addPluginParameter (new slParameter (paramPulse1Shape,     "Pulse 1 Shape",      "Shape",       "",  0.0f, 15.0f, 1.0f, 0.0f, 1.0f));

    addPluginParameter (new slParameter (paramPulse2Tune,      "Pulse 2 Tune",       "Tune",        "", -48.0f, 48.0f, 1.0f, 0.0f, 1.0f, intTextFunction));
    addPluginParameter (new slParameter (paramPulse2Fine,      "Pulse 2 Tune Fine",  "Fine",        "", -100.0f, 100.0f, 1.0f, 0.0f, 1.0f, intTextFunction));
    addPluginParameter (new slParameter (paramPulse2Level,     "Pulse 2 Level",      "Level",       "",  0.0f,  15.0f, 1.0f, 0.0f, 1.0f));
    addPluginParameter (new slParameter (paramPulse2Shape,     "Pulse 2 Shape",      "Shape",       "",  0.0f, 15.0f, 1.0f, 0.0f, 1.0f));
}

TIAAudioProcessor::~TIAAudioProcessor()
{
}

//==============================================================================
void TIAAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    outputSmoothed.reset (sampleRate, 0.05);
    
    Tia_sound_init (31399, sampleRate);
}

void TIAAudioProcessor::releaseResources()
{
}

void TIAAudioProcessor::runUntil (int& done, AudioSampleBuffer& buffer, int pos)
{
    int todo = jmin (pos, buffer.getNumSamples()) - done;

    while (todo > 0)
    {
        int count = jmin (todo, 1024);
        
        unsigned char out[1024];
        Tia_process (out, count);
        
        float* data = buffer.getWritePointer (0, done);
        
        for (int i = 0; i < count; i++)
        {
            if (out[i] < 30)
                out[i] = out[i];
            
            data[i] = (out[i] / 255.0f) * 2.0f - 1.0f;
        }
        
        done += count;
        todo -= count;
    }
}

void TIAAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midi)
{
    int done = 0;
    runUntil (done, buffer, 0);
    
    int pos = 0;
    MidiMessage msg;
    MidiBuffer::Iterator itr (midi);
    while (itr.getNextEvent (msg, pos))
    {
        runUntil (done, buffer, pos);
        
        if (msg.isNoteOn())
        {
            noteQueue.add (msg.getNoteNumber());
            velocity = msg.getVelocity();
        }
        else if (msg.isNoteOff())
        {
            noteQueue.removeFirstMatchingValue (msg.getNoteNumber());
        }
        else if (msg.isAllNotesOff())
        {
            noteQueue.clear();
        }
        
        const int curNote = noteQueue.size() > 0 ? noteQueue.getLast() : -1;
        
        if (curNote != lastNote)
        {
            if (curNote != -1)
            {
                float freq0 = getMidiNoteInHertz (curNote + parameterIntValue (paramPulse1Tune) + parameterIntValue (paramPulse1Fine) / 100.0f);
                float freq1 = getMidiNoteInHertz (curNote + parameterIntValue (paramPulse2Tune) + parameterIntValue (paramPulse2Fine) / 100.0f);

                float base = 31399.5f;
                int div0 = 0;
                int div1 = 0;

                // ch0
                div0 = base / freq0 / 2;
                if (div0 <= 15)
                {
                    Update_tia_sound (AUDF0, div0);
                    Update_tia_sound (AUDC0, 4);
                }
                else
                {
                    div0 = base / freq0 / 6;
                    if (div0 <= 15)
                    {
                        Update_tia_sound (AUDF0, div0);
                        Update_tia_sound (AUDC0, 0xC);
                    }
                    else
                    {
                        div0 = base / freq0 / 31;
                        if (div0 <= 15)
                        {
                            Update_tia_sound (AUDF0, div0);
                            Update_tia_sound (AUDC0, 6);
                        }
                        else
                        {
                            div0 = base / freq0 / 93;
                            Update_tia_sound (AUDF0, div0);
                            Update_tia_sound (AUDC0, 0xE);
                        }
                    }
                }
                
                // ch1
                div1 = base / freq1 / 2;
                if (div1 <= 15)
                {
                    Update_tia_sound (AUDF1, div1);
                    Update_tia_sound (AUDC1, 4);
                }
                else
                {
                    div1 = base / freq1 / 6;
                    if (div1 <= 15)
                    {
                        Update_tia_sound (AUDF1, div1);
                        Update_tia_sound (AUDC1, 0xC);
                    }
                    else
                    {
                        div1 = base / freq1 / 31;
                        if (div1 <= 15)
                        {
                            Update_tia_sound (AUDF1, div1);
                            Update_tia_sound (AUDC1, 6);
                        }
                        else
                        {
                            div1 = base / freq1 / 93;
                            Update_tia_sound (AUDF1, div1);
                            Update_tia_sound (AUDC1, 0xE);
                        }
                    }
                }
                
                Update_tia_sound (AUDV0, velocity * parameterIntValue (paramPulse1Level));
                Update_tia_sound (AUDV1, velocity * parameterIntValue (paramPulse2Level));
            }
            else
            {
                Update_tia_sound (AUDV0, 0);
                Update_tia_sound (AUDV1, 0);
            }
            
            lastNote = curNote;
        }
    }
    
    runUntil (done, buffer, buffer.getNumSamples());
    
    if (editor)
        editor->scope.addSamples (buffer.getReadPointer (0), buffer.getNumSamples());
}

//==============================================================================
bool TIAAudioProcessor::hasEditor() const
{
    return true;
}

AudioProcessorEditor* TIAAudioProcessor::createEditor()
{
    editor = new TIAAudioProcessorEditor (*this);
    return editor;
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new TIAAudioProcessor();
}
