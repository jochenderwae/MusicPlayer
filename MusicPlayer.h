#ifndef MusicPlayer_h
#define MusicPlayer_h

/*
 * This code is adapted from https://github.com/robsoncouto/arduino-songs/
 */

#define NOTE_B0  31
#define NOTE_C1  33
#define NOTE_CS1 35
#define NOTE_D1  37
#define NOTE_DS1 39
#define NOTE_E1  41
#define NOTE_F1  44
#define NOTE_FS1 46
#define NOTE_G1  49
#define NOTE_GS1 52
#define NOTE_A1  55
#define NOTE_AS1 58
#define NOTE_B1  62
#define NOTE_C2  65
#define NOTE_CS2 69
#define NOTE_D2  73
#define NOTE_DS2 78
#define NOTE_E2  82
#define NOTE_F2  87
#define NOTE_FS2 93
#define NOTE_G2  98
#define NOTE_GS2 104
#define NOTE_A2  110
#define NOTE_AS2 117
#define NOTE_B2  123
#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_CS8 4435
#define NOTE_D8  4699
#define NOTE_DS8 4978
#define NOTE_REST 0

/*
timing bit pattern
Flags    Duration
00000000 00000000
0000000P 00ITSQHF
F = full note
H = half note
Q = quarter note
S = sixteenth note
T = thirtysecond note
I = sixtyfourth note

P = no pause (between this and next note)

*/

#define FULL 1
#define FULL_DOT 3
#define HALF 2
#define HALF_DOT 6
#define QART 4
#define QART_DOT 12
#define EIGT 8
#define EIGT_DOT 24
#define SIXT 16
#define SIXT_DOT 48
#define THRT 32
#define THRT_DOT 96
#define SFOR 64

#define NOPAUSE 0x0100


#define NOTE_END 0xFFFF

template <typename PlayNotePolicy>
class MusicPlayer : private PlayNotePolicy {
private:
  const uint16_t *_melody;
  uint16_t _wholenote;
  
  uint16_t _currentNoteIndex;
  uint16_t _note;
  uint32_t _nextNoteTime;
  bool _playing;
  bool _notePlayed;
  
  using PlayNotePolicy::playNote;

public:
  MusicPlayer() : _playing(false), _melody(NULL), _currentNoteIndex(0),
                  _nextNoteTime(0), _notePlayed(false), _note(NOTE_END) {}


  void play(const uint16_t *melody) {
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

  void tick() {
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
    _note               = (uint16_t)pgm_read_word_near(_melody + _currentNoteIndex);
    uint16_t noteLength = (uint16_t)pgm_read_word_near(_melody + _currentNoteIndex + 1);
  
    // if we reached the end of the song, stop playing and skip
    if(_note == NOTE_END) {
      _playing = false;
      return;
    }
    
    uint32_t noteDuration = 0;
    uint16_t noteDividers = noteLength & 0xff;
    bool pause = (noteLength & NOPAUSE) == 0;
    
    // calculates the duration of each note
    for(uint8_t i = 0; i < 8; i++) {
     if(noteDividers & 2 << i) {
        noteDuration += _wholenote / (noteDividers & 2 << i);
      }
    }
  
    // calculate the time to start the next note
    _nextNoteTime = millis() + noteDuration;
    
  
    // don't play anything if it's a rest note
    if(_note != NOTE_REST) {
      // we only play the note for 90% of the duration, leaving 10% as a pause
      _notePlayed = true;
    playNote(_note, pause ? noteDuration * 0.9 : noteDuration);
    }
  
    // move pointer to the next note
    _currentNoteIndex += 2;
  }
  
  bool isNotePlayed() {
    return _notePlayed;
  }
  
  bool isPlaying() {
    return _playing;
  }
  
  uint16_t getLastNote() {
    return _note;
  }
};

template <int pin>
class ArduinoToneNotePolicy {
protected:
	void playNote(uint16_t note, uint32_t duration) {
		tone(pin, note, duration);
	}
};

#endif
