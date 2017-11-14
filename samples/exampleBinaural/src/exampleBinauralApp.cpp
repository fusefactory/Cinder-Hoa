#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/audio/audio.h"

#include "HoaNode.h"

using namespace ci;
using namespace ci::app;
using namespace std;

using namespace nono::audio;

class exampleBinauralApp : public App {
  public:
	void setup() override;
    void setupInputs();
    void setupAudioDevice();
    void update() override;
    void draw() override;
    void mouseDrag( MouseEvent event ) override;
    void mouseMove( MouseEvent event ) override;

private:

    nono::audio::HoaNodeRef         mHoaNode;

    audio::GainNodeRef				mGain;

    vector<audio::InputNodeRef>     mPlayers;

    nono::audio::HoaSourceRef       mHoaSourceHover;
    nono::audio::HoaSourceRef       mHoaSourceSelected;

};

void exampleBinauralApp::setup()
{
    setupAudioDevice();

    auto ctx = audio::Context::master();

    setupInputs();

    int numAudioSources = mPlayers.size();

    // creating our HOA Binaural Node.
    mHoaNode = ctx->makeNode( new HoaNodeBinaural( numAudioSources ) );
    mHoaNode->enable();

    // add a Gain to reduce the volume
    mGain = ctx->makeNode( new audio::GainNode( 0.125f ) );

    // connect and enable the Context
    mHoaNode >> mGain >> ctx->getOutput();


    console() << " =========================== ADDING ROUTES =======================" << std::endl;
    // Adding all the sound sources as cinder audio routes
    for( int i=0;i<mPlayers.size();i++ ){
        mHoaNode->addInputRoute(mPlayers[i]);
    }


    ctx->enable();
}

void exampleBinauralApp::setupInputs(){

    auto ctx = audio::Context::master();

    // loading a simple sound player

    string file = "../../../../../samples/data/sound/DrainMagic.ogg";
    audio::SourceFileRef sourceFile = audio::load( loadAsset(file), ctx->getSampleRate() );
    audio::BufferRef buffer = sourceFile->loadBuffer();
    audio::BufferPlayerNodeRef p1 = ctx->makeNode( new audio::BufferPlayerNode( buffer ) );
    p1->setLoopEnabled();
    p1->start();
    p1->setName("Player1");

    // creating some sounds...

    audio::GenNodeRef p2 = ctx->makeNode( new audio::GenOscNode( audio::WaveformType::SINE, 440 ) );
    p2->setName("OSC Sine 220");
    p2->enable();

    audio::GenNodeRef p3 = ctx->makeNode( new audio::GenOscNode( audio::WaveformType::SQUARE, 220 ) );
    p3->setName("OSC Square 220");
    p3->enable();

    audio::InputDeviceNodeRef p4 = ctx->createInputDeviceNode();
    p4->enable();
    p4->setName("Microphone");

    mPlayers.push_back( p1 );
    mPlayers.push_back( p2 );
    mPlayers.push_back( p3 );
    mPlayers.push_back( p4 );

}

void exampleBinauralApp::setupAudioDevice(){

    // debug print all devices to console
    console() << audio::Device::printDevicesToString() << endl;

    audio::DeviceRef deviceWithMaxOutputs;

    for( const auto &dev : audio::Device::getDevices() ) {
        if( ! deviceWithMaxOutputs || deviceWithMaxOutputs->getNumOutputChannels() < dev->getNumOutputChannels() )
            deviceWithMaxOutputs = dev;
    }


    // !! Setting it to the defaul output for now...
    deviceWithMaxOutputs = audio::Device::getDefaultOutput();

    getWindow()->setTitle( "Cinder HOA Example Binaural. Output[" + deviceWithMaxOutputs->getName() +"]" );


    auto ctx = audio::master();
    audio::OutputDeviceNodeRef multichannelOutputDeviceNode = ctx->createOutputDeviceNode( deviceWithMaxOutputs, audio::Node::Format().channels( deviceWithMaxOutputs->getNumOutputChannels() ) );
    ctx->setOutput( multichannelOutputDeviceNode );


}

void exampleBinauralApp::update()
{
}

void exampleBinauralApp::draw()
{
    gl::clear( Color( 0, 0, 0 ) );
    gl::color( Color( 1,1,1 ) );

    gl::pushMatrices();
    gl::translate( getWindowWidth()/2, getWindowHeight()/2 );
    float scale = getWindowWidth()/2;
    auto sources = mHoaNode->getHoaInputs();
    for( const auto& s: sources ){
        vec3 pos = s->mHoaElement->getPosition();
        vec2 pos2D(pos.x*scale,pos.y*scale);
        if( mHoaSourceHover == s ){
            gl::color(1, 0, 0);
        }else{
            gl::color(1, 1, 1);
        }
        gl::drawSolidCircle(pos2D, 10);
    }
    gl::popMatrices();
}

void exampleBinauralApp::mouseDrag( MouseEvent e ){

    vec2 mPos = vec2((float)e.getPos().x,(float)e.getPos().y) - vec2(getWindowWidth()/2, getWindowHeight()/2);
    float scale = getWindowWidth()/2;
    if( mHoaSourceHover != nullptr ){
        vec3 pos( mPos.x/scale, mPos.y/scale, 0 );
        mHoaSourceHover->mHoaElement->setPosition(pos);
        mHoaNode->updatePositions();
    }

}

void exampleBinauralApp::mouseMove( MouseEvent e ){

    if( !mHoaNode ) return;
    vec2 mPos = vec2((float)e.getPos().x,(float)e.getPos().y) - vec2(getWindowWidth()/2, getWindowHeight()/2);
    float scale = getWindowWidth()/2;
    auto sources = mHoaNode->getHoaInputs();
    mHoaSourceHover = nullptr;
    for( auto s: sources ){
        vec3 pos = s->mHoaElement->getPosition();
        vec2 pos2D( pos.x*scale, pos.y*scale );
        if( length(pos2D-mPos) < 10 ){
            mHoaSourceHover = s;
        }
    }
}

CINDER_APP( exampleBinauralApp, RendererGl (RendererGl::Options().stencil().msaa (16)),
           [&] (App::Settings * settings)
{
    settings->setWindowSize (400, 400);
    settings->setFrameRate (60.0f);
    settings->setTitle ("Cinder HOA Wrapper Binaural");
    settings->setHighDensityDisplayEnabled();
})
