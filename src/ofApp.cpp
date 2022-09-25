#include "ofApp.h"

#define globalBri 0xff  // all LEDs at full brightness, wear sunglasses or lower this
#define smoothing 0x00 // no smoothing, we want to send data and display it straight away
#define ringPanelID 0 // if 0, all ring panels will show the same data
#define ringFrontBackMirror 1 // we put the same data on front and back rings
#define idByte_test 0x80
char idByte[1];

#define numberOfRingsPairs 1 // We only have to send the data for 1 ring pair, because the same data will be broadcasted to all panels, as per bits 0..3 of the ID byte of the RS485 string protocol
#define numberOfLedsPerRing 108 // 108 LEDs per ring as per the documentation



char ring_strig[437] = { 0x00, globalBri, globalBri, smoothing, idByte_test };

//--------------------------------------------------------------
void ofApp::setup(){

	// Setup the UDP
	udpConnection.Create();
	udpConnection.Connect("192.168.1.78", 16661);
	udpConnection.SetNonBlocking(true);

	// Setup the GUI
	gui.setup();

	// Add the button
	gui.add(CycleTest.setup("RGBW Auto Cycle Test (20 seconds)"));

	//gui.add(CycleTestToggle.setup("RGBW Auto Cycle Test", false));

	//gui.add(numberOfPorts.setup("Number of splitter ports with connected rings", 3, 1, 16));
	//numberOfPorts.addListener(this, &ofApp::updateModel);
	
	// Add the colorhweel, and the function it sends the selected colors to
	gui.add(color.set("color", ofColor(255, 0, 0, 255), ofColor(0, 0, 0, 0), ofColor(255, 255, 255, 255)));
	colorListener = color.newListener(this, &ofApp::updateModel);
	
	std::cout << "setup complete" << endl;
}

//Function that takes the colors from the color wheel and generate the RS485 string to control the actual LEDs.
void ofApp::updateModel(ofColor& color) {

	std::cout << "R,G,B,W: " << color << endl;

	char colorRGBW[4];
	colorRGBW[0] = color.r;
	colorRGBW[1] = color.g;
	colorRGBW[2] = color.b;
	colorRGBW[3] = color.a; // I know that the alpha channel is not the White in RGBW, but I prefered using the nice RGB colorwheel rather than 4 regular sliders. So for this purpose I am using the alpha as White.

	// 4 LEDs (RBGW) times 108 LEDs per ring, times 1 ring. the same data will be copied to the secondary ring and to all rings in the same chain.
	for (int i = 0; i < numberOfRingsPairs*numberOfLedsPerRing * 4; i++) {
		i % 4 == 0 ? ring_strig[i + 5] = colorRGBW[0] : ring_strig[i + 5] = ring_strig[i + 5];
		i % 4 == 1 ? ring_strig[i + 5] = colorRGBW[1] : ring_strig[i + 5] = ring_strig[i + 5];
		i % 4 == 2 ? ring_strig[i + 5] = colorRGBW[2] : ring_strig[i + 5] = ring_strig[i + 5];
		i % 4 == 3 ? ring_strig[i + 5] = colorRGBW[3] : ring_strig[i + 5] = ring_strig[i + 5];
	}

	//debug only
	//udpConnection.Send(ring_strig, sizeof(ring_strig));

	// Now integrate the led string data into the udp string and sent it.
	sendUDPString();

}


//--------------------------------------------------------------
// Function that builds a test RS485 ring with all R,G,B,W colors
void ofApp::buildRingString(int color) {

	// build the string data: 4 colors, times the number of LEDs per ring (108), times the number of rings daisy chaines
	for (int i = 0; i < numberOfRingsPairs*numberOfLedsPerRing * 4; i++) {

		// Assuming the color format is RED,GREEN,BLUE,WHITE, a loop doing everything full red, then green, then blue, then white would be:
		// we start at 5 because the first 5 bytes are for settings, as per Mike's protocol.

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

#define udp_incipit 0x02 // defined by WWL
#define udp_format 0x00 // DMX style with initial break
#define udp_flags 0x80 // value is 128 to start transmission immediately
#define udp_ports 0x00 // 0 for same data to all ports
#define udp_nbyteslo 0xB5 // 437 bytes per port. 437(dec) = 01B5(hex)
#define udp_nbyteshi 0x01 // 437 bytes per port. 437(dec) = 01B5(hex)
#define udp_nports 0x03 // number of ports in use (see hardware diagram)
#define udp_portsinpkt 0x03 // number of ports for which this packet contains data
char udp_strig[445] = { udp_incipit, udp_format, udp_flags, udp_ports, udp_nbyteslo, udp_nbyteshi, udp_nports, udp_portsinpkt };
//--------------------------------------------------------------
// Function that puts the RS485 LED string into a UDP string for the splitter.
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
void ofApp::chooseColor() {

}

//--------------------------------------------------------------
void ofApp::chooseRings() {

}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
	//ofDrawCircle(ofGetWidth() / 2, ofGetHeight() / 2, radius);
	gui.draw();

	if (CycleTest) {
		std::cout << "Button Pressed, triggering RGBW test now. Duration 15 seconds" << endl;
		testAllLEDs();
	}

	if (CycleTestToggle) {
		//std::cout << "Toggled ON" << endl;
	}
	else {
	//	std::cout << "Toggled OFF" << endl;
	}
}






//--------------------------------------------------------------
void ofApp::keyPressed(int key){

	////idByte = 

	//char ring_strig[20] = { 0x00, globalBri, globalBri, smoothing, idByte_test};

	//// All RED: 0xFF every 4 bytes
	//for (int i = 0; i < 10; i++) {
	//	
	//	ring_strig[i + 5] = 0xFF;

	//	/*if (i % 4 == 0) {
	//		ring_strig[i + 5] = 0xFF;
	//	}
	//	else {
	//		ring_strig[i + 5] = 0x00;
	//	}*/
	//}

	////char test_hex[3] = { 0x00, 0x01, 0x02 };
	////string message = "Key pressed, sending UDP message now";
	////string message = "00";
	//udpConnection.Send(ring_strig, sizeof(ring_strig));
	////udpConnection.Send(message.hex)
	////udpConnection.Send(message.c_str(), message.length());

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
