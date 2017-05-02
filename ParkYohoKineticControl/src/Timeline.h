#ifndef __ParkYohoTimeline__
#define __ParkYohoTimeline__

#include <stdio.h>
#include "ofMain.h"
#include "ofxGui.h"
#include "Keyframe.h"
#include "ofEvents.h"

class Timeline
{
public:
	ofEvent<Keyframe> keyframeSelectedEvent; //a event for other class to listen to keyframe selected
	ofEvent<int> keyframeDeselectedEvent; //a event for other class to listen to keyframe deselected

	int NULL_FRAME;
	vector <Keyframe> frames;
	ofRectangle rect;
	Keyframe selectedKeyframe;

	float xPos;
	float yPos;
	float width;
	float scroll;
	bool doAddKeyframeOnClick;
	bool doRemoveKeyframeOnClick;	


	void setPos(float x, float y, float w, float scroll); //set the position of timeline, scroll is the scroll amount of the graph
	void addKeyframeOnClick(); //enable add a keyframe on next mouse Press
	void removeKeyframeOnClick(); //enable remove a clicked keyframe on next mouse Press
	void selectKeyframeOnClick(); //disable add or remove keyframe on click
	float getValueAtPos(float posX); //get the value at X position
	void deselectKeyframes(); //deselect keyframes
								   
	//-----------  To do  --------------
	void loadKeyframes();//load keyframes data to init the frames array
	void saveKeyframes();//save keyframes data to file
	void reset();//remove all keyframes and ready for load keyframes to add data

	void setup();
	void update();
	void draw();
	void mousePressed(int x, int y, int button);

private:
	void sortKeyframes();
	Keyframe nullKeyframe;
};

#endif