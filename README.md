# MusicPlayer
## Introduction
MusicPlayer is an Arduino library to play music scores on any number of Arduino compatible devices using only a piezo buzzer.
This repository also includes a web page that can turn music scores in ABC notation into a data array that the MusicPlayer can play.
The convertor is very basic and will skip over characters or constructions that it doesn't recognize.
## Arduino code
Robson Couto https://github.com/robsoncouto/arduino-songs/
Start with including the MusicPlayer header:
```C++
#include "MusicPlayer.h"
```
Next, create a variable to import the tune:
```C++
extern const PROGMEM int odeToJoy[];
```
Create the music player itself (using a piezo buzzer connected to pin 9):
```C++
MusicPlayer musicPlayer = MusicPlayer(9);
```
And finally the setup and loop functions.
```C++
void setup()
{
	musicPlayer.play(odeToJoy);
}

void loop()
{
	musicPlayer.tick();
}
```
## MusicPlayer API
### Constructor
```C++
MusicPlayer(int buzzerPin);
```
The constructor takes one parameter: the pin to which the piezo element is connected.
### play
```C++
void MusicPlayer::play(const int *melody);
```
Start playing a tune defined in `melody`. This is an array in which the content is formatted as follows:
- Tempo - the first integer indigates the tempo (or beats per minute) of the tune
- Notes - two integers for each note:
  - Tone - the tone or frequency of the note
  - Duration - the duration of the note
- NOTE_END - termination code of the tune. The player will stop when it encounters this value.
### tick
```C++
void MusicPlayer::tick();
```
Call the `tick` method in the `loop()` function. In this call the MusicPlayer will:
- check how much time has passed since last call
- determine if a new note should be played
- load the next note and timing
- play the note for the specified duration  
### isNotePlayed
```C++
  bool MusicPlayer::isNotePlayed();
```
Call this after `tick`. Will return `true` if a new note was just started.
### isPlaying
```C++
  bool MusicPlayer::isPlaying();
```
Will return `true` if there's currently a tune playing (after a call to `play` and before reaching the end of the tune.
### getLastNote
```C++
  int MusicPlayer::getLastNote();
```
Returns the numeric value (frequency) of the last played note. Use this to e.g. blink leds based on the note played.
## Convertor
You can use the supplied HTML page to convert musical scores in ABC fromat (see: http://abcnotation.com/) to integer arrays containing all note information.
Paste the ABC notation score in the top input box, click convert and copy the contents of the bottom two text boxes to a .cpp file and an .ino file.
Or mix them together into one file.