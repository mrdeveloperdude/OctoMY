#ifndef NOTEUTILS_H
#define NOTEUTILS_H

#include <QString>

float noteToFreq(float note);
float freqToNote(float freq);
float freqToTone(float freq);
QString midiNoteToString(int note);
int midiNoteFromString(QString  note);



#endif // NOTEUTILS_H
