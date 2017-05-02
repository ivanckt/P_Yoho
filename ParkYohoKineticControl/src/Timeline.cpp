#include "Timeline.h"

struct keyframe_comparer
{
	bool operator()(const Keyframe& lhs, const Keyframe& rhs)
	{
		return lhs.x < rhs.x;
	}
};

void Timeline::setup() {
	selectedKeyframe = nullKeyframe;
	xPos = 0.0f;
	yPos = 0.0f;
	width = 0.0f;
	scroll = 0.0f;
	NULL_FRAME = 0;
}

void Timeline::update() {

}

void Timeline::draw() {
	ofPushStyle();
	ofPushMatrix();

	//draw background
	ofFill();
	if (doAddKeyframeOnClick) {
		ofSetColor(200, 225, 200);
	}
	else if (doRemoveKeyframeOnClick) {
		ofSetColor(225, 200, 200);
	}
	else {
		ofSetColor(200, 200, 200);
	}
	ofDrawRectangle(xPos, yPos, width, 20);

	//draw keyframes 
	ofSetColor(225, 100, 100);
	for (int i = 0; i < frames.size(); i++) {
		Keyframe kf = frames[i];
		if (kf.selected) {
			ofSetColor(100, 225, 100);
		}
		else {
			ofSetColor(225, 100, 100);
		}
		ofDrawCircle(kf.x, yPos+10.0f, 10.0f);
	}
	
	ofPopMatrix();
	ofPopStyle();
}

void Timeline::mousePressed(int x, int y, int button) {
	//check if the click is inside the timeline
	if (y > yPos && y < yPos + 20 && x > xPos && x < xPos + width) {
		//deselect all keyframes
		deselectKeyframes();

		int i;
		Keyframe kf;
		//check if any keyframes is selected and choose the nearest to mousePos
		bool isSelected = false;
		int selectedKeyframeIndex;
		float minDist = 10.0f;
		for (i = 0; i < frames.size(); i++) {
			kf = frames[i];
			float dist = (kf.x - scroll) - x;
			dist = std::abs(dist);
			if (dist < 10.0f && dist<minDist) {
				minDist = dist;
				isSelected = true;
				selectedKeyframeIndex = i;
			}
		}

		if (isSelected) {
			//if a keyframe is clicked, select it or remove it
			if (doRemoveKeyframeOnClick) {
				//check if the frame is currently selected
				if (frames[selectedKeyframeIndex].x == selectedKeyframe.x) {
					//deselect it if the same
					selectedKeyframe = nullKeyframe;
					keyframeDeselectedEvent.notify(this, NULL_FRAME);
				}

				frames.erase(frames.begin() + selectedKeyframeIndex);
				//doRemoveKeyframeOnClick = false;
			}else {
				frames[selectedKeyframeIndex].selected = true;
				selectedKeyframe = frames[selectedKeyframeIndex];
				keyframeSelectedEvent.notify(this, frames[selectedKeyframeIndex]);
			}
		}else if (doAddKeyframeOnClick) {
			//deselect the keyframe if there is a selected
			if (nullKeyframe.x != selectedKeyframe.x) {
				selectedKeyframe = nullKeyframe;
				keyframeDeselectedEvent.notify(this, NULL_FRAME);
			}
			//if not clicking on any keyframe, add a new keyframe
			Keyframe newKf;
			newKf.x = scroll + x;
			//newKf.selected = true;
			frames.push_back(newKf);
			sortKeyframes();
			//selectedKeyframe = newKf;
			//keyframeSelectedEvent.notify(this, newKf);
			//doAddKeyframeOnClick = false;
		}else {
			//deselect the keyframe if there is a selected
			if (nullKeyframe.x != selectedKeyframe.x) {
				selectedKeyframe = nullKeyframe;
				keyframeDeselectedEvent.notify(this, NULL_FRAME);
			}
			
		}
	}
}
void Timeline::deselectKeyframes() {
	int i;
	Keyframe kf;
	for (i = 0; i < frames.size(); i++) {
		frames[i].selected = false;
	}
}
void Timeline::setPos(float x, float y, float w, float s) {
	xPos = x;
	yPos = y;
	width = w;
	scroll = s;
}

void Timeline::addKeyframeOnClick() {
	doAddKeyframeOnClick = true;
	doRemoveKeyframeOnClick = false;
}

void Timeline::removeKeyframeOnClick() {
	doRemoveKeyframeOnClick = true;
	doAddKeyframeOnClick = false;
}

void Timeline::selectKeyframeOnClick() {
	doRemoveKeyframeOnClick = false;
	doAddKeyframeOnClick = false;
}


float Timeline::getValueAtPos(float posX) {
	
	int size = frames.size();
	//handle special cases of keyframes
	if (size == 0) {
		//if no keyframes
		return 0.0f;
	} else {
		//values before the first keyframe is set to same as first keyframe
		if (posX <= frames[0].x) {
			return frames[0].val;
		}
		//values after the last keyframe is set to same as last keyframe
		if (posX >= frames[size-1].x) {
			return frames[size-1].val;
		}
	}

	//values is in-between the keyframes
	float val = 0.0f;
	Keyframe k1; //nearest smaller keyframe
	Keyframe k2; //nearset larger keyframe
	float maxX = 1000000.0f;
	float minX = 0.0f;
	for (int i = 0; i < size; i++) {
		if (frames[i].x <= posX && frames[i].x>=minX) {
			k1 = frames[i];
			minX = k1.x;
		}
		if (frames[i].x >= posX && frames[i].x<=maxX) {
			k2 = frames[i];
			maxX = k1.x;
		}
	}

	val = (k2.val - k1.val) / (k2.x - k1.x)  * (posX - k1.x) + k1.val;
	return val;

}

//TO DO: write a loop to read the saved data and populate the frames array
void Timeline::loadKeyframes() {
	
	Keyframe newKf;
	//newKf.x = XXXX;
	frames.push_back(newKf);
	
	sortKeyframes();
}

//TO DO: save keyframes data to file
void Timeline::saveKeyframes() {

}

void Timeline::reset() {
	frames.clear();
	selectKeyframeOnClick();
	if (nullKeyframe.x != selectedKeyframe.x) {
		selectedKeyframe = nullKeyframe;
		keyframeDeselectedEvent.notify(this, NULL_FRAME);
	}
}

void Timeline::sortKeyframes() {
	//sort the keyframes
	std::sort(frames.begin(), frames.end(), keyframe_comparer());
}
