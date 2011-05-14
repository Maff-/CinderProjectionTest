#include "Resources.h"
#include "cinder/app/AppBasic.h"
#include "cinder/params/Params.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/gl/Texture.h"
#include "cinder/ImageIo.h"
#include "cinder/Camera.h"
#include "cinder/Arcball.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class CinderProjectionTestApp : public AppBasic {
  public:
	void setup();
	void keyDown( KeyEvent event );
	void mouseMove( MouseEvent event );
	void mouseDown( MouseEvent event );
	void mouseDrag( MouseEvent event );
	void mouseWheel (MouseEvent event);
	void resize( ResizeEvent event );
	void update();
	void draw();
	
	// Camera to look around in the scene
	CameraPersp mLookCam;
	Vec3f       mLookEye;
	Vec3f       mLookCenter;
	Vec3f       mLookUp;
	float       mLookDistance;
	
	// Camera that is located at the 'sweet spot',
	// from this point the texture should be projected
	CameraPersp mSweetCam;
	Vec3f       mSweetEye;
	Vec3f       mSweetCenter;
	Vec3f       mSweetUp;
	
	// Camera at the position of the Projector
	// this is the view that the real life projector should project
	CameraPersp mProjCam;
	Vec3f       mProjEye;
	Vec3f       mProjCenter;
	Vec3f       mProjUp;
	
	int         mActiveCam;
	
	Quatf       mCubeRotation;
	float       mCubeSize;
	
	gl::Texture	mTexture;
    gl::GlslProg mProjShader;
	float       mTextureScaling;
	
	Vec2f			mMousePos;
	
	Arcball     mArcball;
	
	params::InterfaceGl mParams;
	
	float mFPS;
	
	enum {
		CAM_LOOK = 0,
		CAM_SWEET = 1,
		CAM_PROJ = 2
	};
};

void CinderProjectionTestApp::setup()
{
	mLookEye = Vec3f( 5.0f, 10.0f, 30.0f );
	mSweetEye = Vec3f( 0.0f, 0.0f, 4.0f );
	mProjEye = Vec3f( 2.0f, 3.0f, 3.0f );
	mLookCenter = mSweetCenter = mProjCenter = Vec3f::zero();
	mLookUp = mSweetUp = mProjUp = Vec3f::yAxis();
	
	mLookDistance = 30.0f;
	// setPerspective of cams moved to ::resize()
	
	mActiveCam = CAM_LOOK;
	
	mCubeSize = 1.0f;
	
	mTexture = gl::Texture( loadImage( loadResource( RES_IMAGE ) ) );
    mProjShader = gl::GlslProg( loadResource( RES_TEXTUREPROJECTION_VERT ), loadResource( RES_TEXTUREPROJECTION_FRAG ) );
	mTextureScaling = 1.0f;
	
	vector<string> camEnum;
	camEnum.push_back("Look around (free)");
	camEnum.push_back("'Sweet spot' (fixed)");
	camEnum.push_back("Projector (fixed)");
	
	mParams = params::InterfaceGl( "Texture Projection", Vec2i( 160, 100 ) );
	//mParams.addParam( "Active Cam", camEnum, &mActiveCam ); // ::addParam(..enum..) is not available in Cinder 0.8.2
	//mParams.setOptions("", "iconified=true" ); // ::setOptions is not available in Cinder 0.8.2
	//mParams.setOptions("", "help='Keys [1], [2] and [3] can be used to switch camrea positions'" );
	mParams.addParam( "Avg. FPS", &mFPS, "precision=1", true );
	mParams.addParam( "Tex Scale", &mTextureScaling, "step=0.1" );
	mParams.addParam( "Cube Rot.", &mCubeRotation );
	mParams.addParam( "Cube Size", &mCubeSize, "keyincr=+ keydecr=-" );
	mParams.addSeparator();
	mParams.addParam( "Sweet Eye", &mSweetEye );
}

void CinderProjectionTestApp::resize( ResizeEvent event )
{
	mArcball.setWindowSize( getWindowSize() );
	mArcball.setCenter( getWindowCenter() );
	mArcball.setRadius( 150 );
	
	mLookCam.setPerspective(60, getWindowAspectRatio(), 1.0f, 100.0f);
	mSweetCam.setPerspective(60, getWindowAspectRatio(), 1.0f, 30.0f);
	mProjCam.setPerspective(60, getWindowAspectRatio(), 1.0f, 30.0f);
}

void CinderProjectionTestApp::keyDown( KeyEvent event )
{
	switch (event.getCode()) {
		case KeyEvent::KEY_1:
		case KeyEvent::KEY_2:
		case KeyEvent::KEY_3:
			mActiveCam = event.getCode() - KeyEvent::KEY_1;
			break;
		default:
			break;
	}
}

void CinderProjectionTestApp::mouseDown( MouseEvent event )
{
	mArcball.mouseDown( event.getPos() );
}

void CinderProjectionTestApp::mouseMove( MouseEvent event )
{	
	mMousePos.x = event.getX() - getWindowWidth() * 0.5f;
	mMousePos.y = getWindowHeight() * 0.5f - event.getY();
}

void CinderProjectionTestApp::mouseDrag( MouseEvent event )
{
	mArcball.mouseDrag( event.getPos() );
}
void CinderProjectionTestApp::mouseWheel (MouseEvent event)
{
	if ( mActiveCam == CAM_LOOK )
	{
		mLookDistance -= event.getWheelIncrement() ;
	}
}

void CinderProjectionTestApp::update()
{
	mLookEye.normalize();
	mLookEye *= abs(mLookDistance);
	mLookCam.lookAt(mLookEye, mLookCenter);
	//mLookCam.setOrientation( mArcball.getQuat() );
	mSweetCam.lookAt(mSweetEye, mSweetCenter);
	mProjCam.lookAt(mProjEye, mProjCenter);
	
	mFPS = getAverageFps();
}

void CinderProjectionTestApp::draw()
{
	
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ), true );
	
	
	gl::enableDepthRead();
	gl::enableDepthWrite();
	
	//gl::enableAlphaBlending();
	
	
	if ( mActiveCam == CAM_LOOK )
	{
		gl::setMatrices( mLookCam );
		gl::rotate( mArcball.getQuat() );
	}
	else if ( mActiveCam == CAM_SWEET )
		gl::setMatrices( mSweetCam );
	else if ( mActiveCam == CAM_PROJ )
		gl::setMatrices( mProjCam );
	
	// Green = Cam from 'sweet spot'
	gl::color( Color( 0, 1, 0 ) );
	gl::drawFrustum( mSweetCam );
	
	// Cyan = Cam from projector
	gl::color( Color( 0, 1, 1 ) );
	gl::drawFrustum( mProjCam );
	
	// Draw a grid in the yz plane
	gl::color( Color( 0.2f, 0.2f, 0.2f ) );
	gl::pushMatrices();
	gl::scale(Vec3f::one() /5 );
	for (int i=0; i<=20; ++i)
	{
		gl::drawLine(Vec3f(-100.0f+(i*10.0f), 0.0f, -100.0f),Vec3f(-100.0f+(i*10.0f), 0.0f, 100.0f));
		gl::drawLine(Vec3f(-100.0f, 0.0f, -100.0f+(i*10.0f)),Vec3f(100.0f, 0.0f, -100.0f+(i*10.0f)));
	}
	gl::popMatrices();
	
	gl::color( Color::white() );
	
	//glEnable( GL_LIGHTING );
	//glEnable( GL_LIGHT0 );
	//GLfloat light_position[] = { mMousePos.x, mMousePos.y, 75.0f, 1.0f };
	//glLightfv( GL_LIGHT0, GL_POSITION, light_position );
	
	
	// Bind texture, so it will be avaible to the fragment shader.
	mTexture.bind();
    // Bind the texture projection shader, let the magic happen!
    mProjShader.bind();
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    mProjShader.uniform("projMap", 0);
    mProjShader.uniform("TexGenMat", mSweetCam.getProjectionMatrix() );
    mProjShader.uniform("InvViewMat", mProjCam.getInverseModelViewMatrix() );
	mProjShader.uniform("texScaling", mTextureScaling );
	//mProjShader.uniform("alpha", 0.6f );
    //mProjShader.uniform("scaling", 10.0f );
	
	gl::pushModelView();
	//gl::rotate( Vec3f( 0.0f, 45.0f, 0.0f ) );
	gl::rotate( mCubeRotation );
	gl::translate( Vec3f(0.0f, 0.5f * mCubeSize, 0.0f) );
	gl::drawColorCube( Vec3f::zero(), Vec3f::one() * mCubeSize );
	gl::popModelView();
	
	
    
    mProjShader.unbind();
	mTexture.unbind();
	
	glDisable( GL_LIGHTING );
	
	gl::color( Color::white() );
	
	
	// Draw the interface
	params::InterfaceGl::draw();
}


CINDER_APP_BASIC( CinderProjectionTestApp, RendererGl )
