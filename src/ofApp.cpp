#include "ofApp.h"

#define globalBri 0xff  // all LEDs at full brightness, wear sunglasses or lower this
#define smoothing 0x00 // no smoothing, we want to send data and display it straight away
#define ringPanelID 0 // if 0, all ring panels will show the same data
#define ringFrontBackMirror 1 // we put the same data on front and back rings
#define idByte_test 0x80
char idByte[1];

int numberOfRingsPairs = 1;
#define numberOfLedsPerRing 5

#define udp_incipit 0x02 // defined by WWL
#define udp_format 0x00 // DMX style with initial break
#define udp_ports 0x00 // 0 for same data to all ports
#define udp_nbyteslo 0xB5 // 437 bytes per port. 437(dec) = 01B5(hex)
#define udp_nbyteshi 0x01 // 437 bytes per port. 437(dec) = 01B5(hex)
#define udp_nports 0x03 // number of ports in use (see hardware diagram)
#define udp_portsinpkt 0x03 // number of ports for which this packet contains data


//--------------------------------------------------------------
void ofApp::setup(){

	udpConnection.Create();
	udpConnection.Connect("192.168.1.78", 16661);
	udpConnection.SetNonBlocking(true);

	
	
}

//--------------------------------------------------------------
void ofApp::buildRingString(int color) {

	// build the string data: 4 colors, times the number of LEDs per ring (108), times the number of rings daisy chaines
	for (int i = 0; i < numberOfRingsPairs*numberOfLedsPerRing * 4; i++) {

		// Assuming the color format is RED,GREEN,BLUE,WHITE, a loop doing everything full red, then green, then blue, then white would be:

		if (color == 1) {
			i % 4 == 0 ? ring_strig[i + 5] = 0xFF : ring_strig[i + 5] = 0x00;
		}
		if (color == 2) {
			i % 4 == 1 ? ring_strig[i + 5] = 0xFF : ring_strig[i + 5] = 0x00;
		}
		if (color == 3) {
			i % 4 == 2 ? ring_strig[i + 5] = 0xFF : ring_strig[i + 5] = 0x00;
		}
		if (color == 4) {
			i % 4 == 3 ? ring_strig[i + 5] = 0xFF : ring_strig[i + 5] = 0x00;
		}
	}
	
	//debug only
	udpConnection.Send(ring_strig, sizeof(ring_strig));

}

char udp_strig[445] = { udp_incipit, udp_format, 0x80, udp_ports, udp_nbyteslo, udp_nbyteshi, udp_nports, udp_portsinpkt };
//--------------------------------------------------------------
void ofApp::sendUDPString() {

	// Simply add the ring led data to the udp string data
	for (int i = 0; i < 445; i++) {
		udp_strig[i + 8] = ring_strig[i];
	}

	udpConnection.Send(udp_strig, sizeof(udp_strig));
}

//--------------------------------------------------------------
void ofApp::testAllLEDs() {
	// Simple routine that turns all LEDs red, then green, then blue, then white, wait a few seconds for a person to inspect each board.
	// This will allow to check that all drivers & LEDs are correctly soldered.
	buildRingString(1);
	sendUDPString();
	ofSleepMillis(5000);

	buildRingString(2);
	sendUDPString();
	ofSleepMillis(5000);

	buildRingString(3);
	sendUDPString();
	ofSleepMillis(5000);

	buildRingString(4);
	sendUDPString();
	ofSleepMillis(5000);
}


//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){

}






//--------------------------------------------------------------
void ofApp::keyPressed(int key){

	//idByte = 

	char ring_strig[20] = { 0x00, globalBri, globalBri, smoothing, idByte_test};

	// All RED: 0xFF every 4 bytes
	for (int i = 0; i < 10; i++) {
		
		ring_strig[i + 5] = 0xFF;

		/*if (i % 4 == 0) {
			ring_strig[i + 5] = 0xFF;
		}
		else {
			ring_strig[i + 5] = 0x00;
		}*/
	}

	//char test_hex[3] = { 0x00, 0x01, 0x02 };
	//string message = "Key pressed, sending UDP message now";
	//string message = "00";
	udpConnection.Send(ring_strig, sizeof(ring_strig));
	//udpConnection.Send(message.hex)
	//udpConnection.Send(message.c_str(), message.length());

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}