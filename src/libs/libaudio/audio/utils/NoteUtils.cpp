#include "NoteUtils.hpp"

#include <QDebug>
#include <cmath>

static const float a_hz=440;
static const float a_midi=69;
static const  QString note_names[]{
	"C-", "C#", "D-", "D#", "E-", "F-", "F#", "G-", "G#", "A-", "A#", "B-"
};
static const int note_names_size=12;

float noteToFreq(float note) {
	return a_hz * pow(2.0f, ((note - a_midi) / 12.0f));
}

float freqToNote(float freq){
	return log(freq/a_hz)/log(2.0f) * 12.0 + a_midi;
}


float freqToTone(float freq){
	int note = freqToNote(freq);
	int octave = (note/12);
	return note-(octave*12);
}

QString midiNoteToString(int note){
	if(note<0){
		return "LOW";
	}
	auto name = note_names[note%12];
	auto octave = (note/12);
	return QString("%1%2").arg(name).arg(octave);
}

int midiNoteFromString(QString  note){
	if(note.length()!=3){
		return -1;
	}
	//auto toneChar=QString(QChar(note[0])).toLower();
	const auto part=QString(QChar(note[0]))+QString(QChar(note[1]));
	//qDebug()<<"PART:"<<part;
	int tone=0;
	for(;tone<note_names_size;++tone){
		if(note_names[tone]==part){
			break;
		}
	}
	const auto octave=QString(QChar(note[2])).toInt();
	return tone+(octave*12);
}

/*
qreal NotesWidget::toneForFrequency(qreal tone){
	return (440 / 32) * (pow(2 , ((tone - 9) / 12)));
}

qreal NotesWidget::frequencyForTone(qreal tone){
	return 440 * pow(2, (tone- 49) / 12);
	*/
