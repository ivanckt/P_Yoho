#include "MusicPlayer.h"

//--------------------------------------------------------------
void MusicPlayer::setup() {

	//-----------------------     MP3 Player      -------------------

	string path = "presetMusic/";
	dir.open(path);
	dir.allowExt("mp3");
	dir.listDir();


	currentTrack = 0;
	//go through and print out all the paths
	for (int i = 0; i < dir.size(); i++) {
		ofLogNotice(dir.getPath(i));
		if (i == currentTrack) {
			mp3.load(dir.getPath(currentTrack));
		}
	}
	lastPauseSoundTime = 0;
	//-----------------------     FFT      -------------------

	// the fft needs to be smoothed out, so we create an array of floats
	// for that purpose:
	fftSmoothed = new float[8192];
	for (int i = 0; i < 8192; i++) {
		fftSmoothed[i] = 0;
	}

	nBandsToGet = 128;
	fftSum = 0.0f;
	fftB1 = 0.0f;
	fftB2 = 0.0f;
	fftB3 = 0.0f;
	fftB4 = 0.0f;

	//-----------------------     Graph      -------------------
	graphScale = 0.05f; // scale = 1/20
	fftHistory.assign(0, 0.0);
	timeHistory.assign(0, 0.0);
	lastMaxSoundTime = 0;
	graphScrollX = 0;
	//-----------------------     GUI      -------------------

	

	playButton.addListener(this, &MusicPlayer::playButtonPressed);
	nextButton.addListener(this, &MusicPlayer::nextButtonPressed);
	pauseButton.addListener(this, &MusicPlayer::pauseButtonPressed);
	volume.addListener(this, &MusicPlayer::volumeChanged);
	graphSlide.addListener(this, &MusicPlayer::graphScrollChanged);

	gui.setup("panel");
	gui.setDefaultWidth(400);
	gui.add(playButton.setup("Play"));
	gui.add(nextButton.setup("Next"));
	gui.add(pauseButton.setup("Pause/Resume"));
	gui.add(snapButtonToggle.setup("Snap graph to center", false));
	gui.add(repeatThisToggle.setup("Repeat Current Track", false));
	gui.add(volume.setup("Volume", 0.8, 0, 1));
	gui.add(graphSlide.setup("Graph Slide", 0, 0, 1));
	gui.add(trackFileName.setup("Now Playing", ""));

	gui.loadFromFile("musicPlayerSettings.xml");
}

//--------------------------------------------------------------
void MusicPlayer::update() {

	ofBackground(80, 80, 20);

	//-----------------------     MP3 Player      -------------------

	// update the sound playing system:
	ofSoundUpdate();



	//-----------------------     FFT      -------------------

	// (5) grab the fft, and put in into a "smoothed" array,
	//		by taking maximums, as peaks and then smoothing downward
	float * val = ofSoundGetSpectrum(nBandsToGet);		// request 128 values for fft
	fftSum = 0.0f;
	fftB1 = 0.0f;
	fftB2 = 0.0f;
	fftB3 = 0.0f;
	fftB4 = 0.0f;
	for (int i = 0; i < nBandsToGet; i++) {

		// let the smoothed calue sink to zero:
		fftSmoothed[i] *= 0.96f;

		// take the max, either the smoothed or the incoming:
		if (fftSmoothed[i] < val[i]) fftSmoothed[i] = val[i];

		//sum of all bands
		fftSum += fftSmoothed[i];

		//sum for 4-sub bands
		if (i < nBandsToGet / 4) {
			fftB1 += fftSmoothed[i];
		}else if (i < 2*nBandsToGet / 4) {
			fftB2 += fftSmoothed[i];
		}else if (i < 3*nBandsToGet / 4) {
			fftB3 += fftSmoothed[i];
		}else{
			fftB4 += fftSmoothed[i];
		}
	}

	//record the history if sound is playig. Skip drawing if the sound is looped ( where the currnetTime < lastMaxSoundTime)
	if (mp3.getIsPlaying() && mp3.getPositionMS()>= lastMaxSoundTime) {
		fftHistory.push_back(fftSum);
		timeHistory.push_back(mp3.getPositionMS());
		if (mp3.getPositionMS() > lastMaxSoundTime)
			lastMaxSoundTime = mp3.getPositionMS();
	}
	
	//-----------------------     GUI      -------------------

	mp3.setLoop(repeatThisToggle);

}

//--------------------------------------------------------------
void MusicPlayer::draw() {

	//-----------------------     FFT      -------------------
	ofPushStyle();
	ofEnableAlphaBlending();
	ofSetColor(255, 255, 255, 100);
	ofDrawRectangle(100, ofGetHeight() - 300, 5 * 128, 200);
	ofDisableAlphaBlending();

	// draw the fft resutls:
	ofSetColor(255, 255, 255, 255);

	float width = (float)(5 * 128) / nBandsToGet;
	for (int i = 0; i < nBandsToGet; i++) {
		// (we use negative height here, because we want to flip them
		// because the top corner is 0,0)
		ofDrawRectangle(100 + i*width, ofGetHeight() - 100, width, -(fftSmoothed[i] * 200));
	}

	string reportString = "fftSum: " + ofToString(fftSum) + "\nfftB1: " + ofToString(fftB1) + "\nfftB2: " + ofToString(fftB2) + "\nfftB3: " + ofToString(fftB3) + "\nfftB4: " + ofToString(fftB4) + "\ntime: " + ofToString(mp3.getPositionMS() / 1000) + " / " + ofToString(duration / 1000);
	ofDrawBitmapString(reportString, 400, ofGetHeight() - 400);

	//draw the fft history as a graph
	float graphWidth = duration * 1.0f * graphScale;
	if (snapButtonToggle && duration >0) {
		graphScrollX = mp3.getPosition() - (0.5f * ofGetWidth() / graphWidth);
	}
	ofPushMatrix();
	ofTranslate(-graphScrollX * graphWidth, 0, 0);//translate graph to current page
	ofSetColor(225);
	ofSetLineWidth(1);
	ofFill();
	ofSetColor(125);
	ofDrawRectangle(0, 400, graphWidth, 20);
	ofSetColor(225);
	if (fftHistory.size() > 0) {
		ofBeginShape();

		for (unsigned int i = 0; i < fftHistory.size(); i++) {
			if (i == 0) ofVertex(i, 400);
			float t = timeHistory[i];

			ofVertex(t*graphScale, 400 - fftHistory[i] * 10);

			if (i == fftHistory.size() - 1) ofVertex(t*graphScale, 400);
		}
		ofEndShape(false);
	}
	ofSetColor(100, 255, 100);
	if (mp3.getIsPlaying()) {
		ofDrawRectangle(graphWidth*mp3.getPosition()-1, 320, 2, 100);
	}
	else if(duration >0){
		ofDrawRectangle(graphWidth*(lastPauseSoundTime*1.0f/duration)-1, 320, 2, 100);
	}
	
	ofPopMatrix();

	ofPopStyle();
	//-----------------------     GUI      -------------------

	gui.draw();

}

//--------------------------------------------------------------
//-----------------------     GUI EVENTS     -------------------
//--------------------------------------------------------------

//--------------------------------------------------------------
void MusicPlayer::playButtonPressed() {
	computeSoundDuration();
	resetGraph();
	mp3.play();
	trackFileName.setup("Now Playing", dir.getName(currentTrack));
}

//--------------------------------------------------------------
void MusicPlayer::nextButtonPressed() {

	if (currentTrack < dir.size() - 1)
	{
		currentTrack++;
	}
	else {
		currentTrack = 0;
	}
	mp3.load(dir.getPath(currentTrack));
	computeSoundDuration();
	resetGraph();
	mp3.play();
	trackFileName.setup("Now Playing", dir.getName(currentTrack));

	ofLog() << "CurrentTrack : " << currentTrack;
}

void MusicPlayer::pauseButtonPressed() {
	if (mp3.isPlaying()) {
		lastPauseSoundTime = mp3.getPositionMS();
		mp3.stop();
	} else {
		mp3.play();
		mp3.setPositionMS(lastPauseSoundTime);
	}
}

void MusicPlayer::mousePressed(int x, int y, int button) {
	if (y > 400 && y < 420) {
		float graphWidth = duration * 1.0f * graphScale;
		float time = duration * (((x*1.0 )/graphWidth)+graphScrollX);
		if (time > duration)
			time = duration;
		if (time < 0.0f)
			time = 0.0f;
		ofLog() << "time : " << time;
		mp3.setPositionMS(time);
		if (!mp3.getIsPlaying()) {
			lastPauseSoundTime = time;
		}
	}
}

//--------------------------------------------------------------
void MusicPlayer::volumeChanged(float &setVolume) {
	mp3.setVolume(setVolume);
}

void MusicPlayer::graphScrollChanged(float &setScroll) {
	graphScrollX = setScroll;
}

void MusicPlayer::computeSoundDuration() {
	//set to half position and get the millisecond there
	mp3.play();
	mp3.setPosition(0.5f);
	duration = mp3.getPositionMS()*2 -1;
	mp3.setPosition(0);
	mp3.stop();
	printf("SOUND LENGTH: %i\n\n", duration);
}

void MusicPlayer::resetGraph() {
	fftHistory.clear();
	timeHistory.clear();
	lastMaxSoundTime = 0;
	lastPauseSoundTime = 0;
}