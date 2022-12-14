#pragma once

#include "ofMain.h"
#include "ofxNetwork.h"
#include "ofxGui.h"


class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void testAllLEDs();

		void buildRingString(int color);
		void sendUDPString();
		void updatePort(int &circleResolution);

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

	ofParameter<ofColor> color;
	void updateModel(ofColor&);
	ofEventListener colorListener;

	// Setup UDP COMM
	ofxUDPManager udpConnection;

	
	ofxPanel gui;

	ofxButton CycleTest;
	ofxToggle CycleTestToggle;
	ofxIntSlider numberOfPorts;
	
		
};
