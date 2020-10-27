#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {

	ofSetFrameRate(60);
	ofSetWindowTitle("openFrameworks");

	ofBackground(239);
	ofSetColor(0);
	ofEnableDepthTest();

	this->fbo1.allocate(ofGetWidth(), ofGetHeight());
	this->fbo2.allocate(ofGetWidth(), ofGetHeight());
	this->shader.load("shader/shader.vert", "shader/shader.frag");

	this->noise_param = glm::vec3(ofRandom(1000), ofRandom(1000), ofRandom(1000));
}

//--------------------------------------------------------------
void ofApp::update() {

	ofSeedRandom(39);

	this->fbo1.begin();
	ofClear(0);

	ofTranslate(ofGetWidth() * 0.5, ofGetHeight() * 0.5);
	ofRotateY(90);

	int span = 15;
	ofMesh face, frame;
	frame.setMode(ofPrimitiveMode::OF_PRIMITIVE_LINES);
	float frame_step = 0.02;
	float len_step = 0.002;
	for (float len = ofGetWidth() * 0.8; len > 0; len -= span) {

		int index = face.getNumVertices();

		auto loc = glm::vec3(
			len * ofMap(ofNoise(this->noise_param.x, ofGetFrameNum() * frame_step + len * len_step), 0, 1, 0, 0.5),
			len * ofMap(ofNoise(this->noise_param.y, ofGetFrameNum() * frame_step + len * len_step), 0, 1, 0, 0.5),
			len * ofMap(ofNoise(this->noise_param.z, ofGetFrameNum() * frame_step + len * len_step), 0, 1, 0, 0.5)
		);

		auto next = glm::vec3(
			(len - span) * ofMap(ofNoise(this->noise_param.x, ofGetFrameNum() * frame_step + (len - span) * len_step), 0, 1, 0, 0.5),
			(len - span) * ofMap(ofNoise(this->noise_param.y, ofGetFrameNum() * frame_step + (len - span) * len_step), 0, 1, 0, 0.5),
			(len - span) * ofMap(ofNoise(this->noise_param.z, ofGetFrameNum() * frame_step + (len - span) * len_step), 0, 1, 0, 0.5)
		);

		face.addVertex(glm::vec3(loc.x, loc.y, loc.z));
		face.addVertex(glm::vec3(-loc.x, loc.y, loc.z));
		face.addVertex(glm::vec3(-next.x, next.y, next.z));
		face.addVertex(glm::vec3(next.x, next.y, next.z));

		frame.addVertex(glm::vec3(loc.x, loc.y, loc.z));
		frame.addVertex(glm::vec3(-loc.x, loc.y, loc.z));
		frame.addVertex(glm::vec3(-next.x, next.y, next.z));
		frame.addVertex(glm::vec3(next.x, next.y, next.z));

		face.addIndex(index + 0); face.addIndex(index + 1); face.addIndex(index + 2);
		face.addIndex(index + 0); face.addIndex(index + 2); face.addIndex(index + 3);

		frame.addIndex(index + 0); frame.addIndex(index + 3);
		frame.addIndex(index + 1); frame.addIndex(index + 2);
	}

	frame.addIndex(0); frame.addIndex(1);
	frame.addIndex(frame.getNumVertices() - 1); frame.addIndex(frame.getNumVertices() - 2);


	for (int i = 0; i < 6; i++) {

		ofRotateX(60);

		ofSetColor(0);
		face.draw();

		ofSetColor(255);
		frame.drawWireframe();
	}

	this->fbo1.end();

	this->fbo2.begin();
	ofClear(0);

	span = 72;
	auto flag = true;
	for (int x = 0; x < ofGetWidth(); x += span) {

		for (int y = 0; y < ofGetHeight(); y += span) {

			if (flag) { ofDrawRectangle(x, y, span, span); }
			flag = !flag;
		}
		flag = !flag;
	}

	this->fbo2.end();
}

//--------------------------------------------------------------
void ofApp::draw() {

	ofFill();
	this->shader.begin();
	this->shader.setUniform1f("time", ofGetElapsedTimef());
	this->shader.setUniform2f("resolution", ofGetWidth(), ofGetHeight());
	this->shader.setUniformTexture("tex1", this->fbo1.getTexture(), 1);
	this->shader.setUniformTexture("tex2", this->fbo2.getTexture(), 2);

	ofDrawRectangle(glm::vec2(0, 0), ofGetWidth(), ofGetHeight());

	this->shader.end();
}

//--------------------------------------------------------------
int main() {

	ofGLWindowSettings settings;
	settings.setGLVersion(3, 2);
	settings.setSize(720, 720);
	ofCreateWindow(settings);
	ofRunApp(new ofApp());
}