#ifndef __ParkYohoMusicPlayer__
#define __ParkYohoMusicPlayer__

#include <stdio.h>
#include "ofMain.h"
#include "ofxGui.h"

class
	MusicPlayer {
		public:
			void setup();
			void update();
			void draw();


			//-----------------------     MP3 Player      -------------------

			ofDirectory dir;

			ofSoundPlayer 		mp3;
			int currentTrack;
			int duration;

			//-----------------------     FFT      -------------------
			float 				* fftSmoothed;
			int nBandsToGet;

			float fftSum;
			float fftB1;
			float fftB2;
			float fftB3;
			float fftB4;

			//-----------------------     Graph      -------------------
			float graphScrollX; //the current page of the graph
			vector <float> fftHistory; //history for fftSum
			vector <float> timeHistory; //history for the time of sound
			float graphScale; //draw scale for the graph 


			//-----------------------     GUI      -------------------
			ofxButton playButton;
			ofxButton nextButton;
			ofxButton pauseButton;

			ofxLabel trackFileName;
			//ofxToggle repeatAllToggle; //TODO
			ofxToggle repeatThisToggle; //TODO
			ofxToggle snapButtonToggle;

			ofxFloatSlider volume;
			ofxFloatSlider graphSlide;

			ofxPanel gui;


			void playButtonPressed();
			void nextButtonPressed();
			void pauseButtonPressed();
			void snapButtonPressed();
			void volumeChanged(float &setVolume);
			void graphScrollChanged(float &setScroll);

			void mousePressed(int x, int y, int button);

		private:
			int lastMaxSoundTime; //record the last max sound time, used for prevent redraw the graph when song looped
			int lastPauseSoundTime;
			void computeSoundDuration();
			void resetGraph();


	};


#endif
