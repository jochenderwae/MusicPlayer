#include <Arduino.h>
#include <avr/pgmspace.h>
#include "MusicPlayer.h"


MusicPlayer::MusicPlayer(int buzzerPin) : _buzzerPin(buzzerPin), _playing(false), 
                                          _melody(NULL), _currentNoteIndex(0),
                                          _nextNoteTime(0), _notePlayed(false),
                                          _note(NOTE_END) {
}



void MusicPlayer::play(const int *melody) {
  // ignore if something is playing
  if(_playing) {
    return;
  }
  
  _melody = melody;
  _currentNoteIndex = 0;
  _playing = true;
  _nextNoteTime = 0;
  _note = NOTE_END;

  // the first value is the tempo
  int tempo = pgm_read_word_near(_melody + _currentNoteIndex);
  _currentNoteIndex++;
  _wholenote = (60 * 1000 * 4) / tempo;
}



void MusicPlayer::tick() {
  // if nothing is playing, skip
  if(!_playing) {
    return;
  }

  // if it's not yet time to play the next note, skip
  if(_nextNoteTime > millis()) {
    _notePlayed = false;
    return;
  }

  // read the next note and duration
  _note          = (short)pgm_read_word_near(_melody + _currentNoteIndex);
  int noteLength = (short)pgm_read_word_near(_melody + _currentNoteIndex + 1);

  // if we reached the end of the song, stop playing and skip
  if(_note == NOTE_END) {
    _playing = false;
    return;
  }
  
  int noteDuration = 0;
  int noteDividers = noteLength & 0xff
  bool pause = (noteLength & NOPAUSE) == 0;
  
  // calculates the duration of each note
  for(byte i < 0; i < 8; i++) {
	  if(divider & 2 << i) {
		  noteDuration += _wholenote / (divider & 2 << i);
	  }
  }

  // calculate the time to start the next note
  _nextNoteTime = millis() + noteDuration;
  

  // don't play anything if it's a rest note
  if(_note != REST) {
    // we only play the note for 90% of the duration, leaving 10% as a pause
    _notePlayed = true;
    tone(_buzzerPin, _note, pause ? noteDuration * 0.9 : noteDuration);
  }

  // move pointer to the next note
  _currentNoteIndex += 2;
}


bool MusicPlayer::isNotePlayed() {
  return _notePlayed;
}

bool MusicPlayer::isPlaying() {
  return _playing;
}

int MusicPlayer::getLastNote() {
  return _note;
}
