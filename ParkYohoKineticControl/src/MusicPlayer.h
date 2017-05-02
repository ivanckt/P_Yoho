#ifndef __ParkYohoMusicPlayer__
#define __ParkYohoMusicPlayer__

#include <stdio.h>
#include "ofMain.h"
#include "ofxGui.h"
#include "Timeline.h"

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


			//-----------------------     Timeline      -------------------
			Timeline timeline;


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
			ofxButton addKeyButton;
			ofxButton removeKeyButton;
			ofxButton selectKeyButton;
			ofParameter<float> keyframeSlider;

			ofxPanel gui;


			void playButtonPressed();
			void nextButtonPressed();
			void pauseButtonPressed();
			void addKeyButtonPressed();
			void removeKeyButtonPressed();
			void selectKeyButtonPressed();
			void volumeChanged(float &setVolume);
			void graphScrollChanged(float &setScroll);
			void keyframeSliderChanged(float &val);
			void keyFrameSelected(Keyframe &kf);
			void keyFrameDeselected(int &i);

			//Use this function to get the tween value of timeline
			vector<float> getTimelineTweenValues();

			void mousePressed(int x, int y, int button);

		private:
			int lastMaxSoundTime; //record the last max sound time, used for prevent redraw the graph when song looped
			int lastPauseSoundTime;
			void computeSoundDuration();
			void resetGraph();
			Keyframe *selectedKeyframe;
			Keyframe nullKeyframe;
			const float KEYFRAME_MIN_VALUE = 0.0f;
			const float KEYFRAME_MAX_VALUE = 20.0f;

	};


#endif
