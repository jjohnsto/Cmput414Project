#include <iostream>
#include <string>


#include <Ogre.h>
#include <OgreApplicationContext.h>
#include <OgreFrameListener.h>
#include <OgreRenderTarget.h>
#include <OgreProfiler.h>
#include <stdio.h>

std::string usageText = "<appname> <-o or -d>\n-o is octree -d is default.\n";

class MyTestApp : public OgreBites::ApplicationContext, public OgreBites::InputListener, Ogre::FrameListener
{
public:
    MyTestApp(bool useBSP,int runFrames,int rotationFrequency, float rotationConstant);
    void setup();
	Ogre::SceneManager* create_scene();
    bool keyPressed(const OgreBites::KeyboardEvent& evt);
    bool frameEnded(const Ogre::FrameEvent& evt);
    void printFrameRateInfo(char* prefix);
    void quit();
private:
	bool useOctree; // enable Octree culling optimization
    Ogre::Camera* cam; 
    Ogre::SceneNode *sn;
    Ogre::Vector3 camMoveDelta;
    int camTimesMoved;
    int runFrames;
    float rotationConstant;
    int rotationFrequency;
};

//! [constructor]
MyTestApp::MyTestApp(bool useOctree,int runFrames,int rotationFrequency,float rotationConstant) : OgreBites::ApplicationContext("OgreTutorialApp")
{
	this->useOctree = useOctree;
    this->runFrames = runFrames;
    this->rotationFrequency = rotationFrequency;
    this->rotationConstant = rotationConstant;
}
//! [constructor]

//! [key_handler]
bool MyTestApp::keyPressed(const OgreBites::KeyboardEvent& evt)
{
    if (evt.keysym.sym == SDLK_ESCAPE)
    {
        this->quit();
    }
    return true;
}
//! [key_handler]

bool MyTestApp::frameEnded(const Ogre::FrameEvent& evt)
{
    this->cam->setAutoTracking(true,sn);
    this->cam->setPosition(cam->getPosition() + this->camMoveDelta);

    if(this->camTimesMoved == this->rotationFrequency){        
        this->camMoveDelta = Ogre::Vector3(0,0,-1) * this->rotationConstant;
    }
    else if(this->camTimesMoved == this->rotationFrequency * 2){        
        this->camMoveDelta = Ogre::Vector3(-1,0,0)* this->rotationConstant;
    }
    else if(this->camTimesMoved == this->rotationFrequency * 3){        
        this->camMoveDelta = Ogre::Vector3(0,0,1)* this->rotationConstant;
    }
    else if(this->camTimesMoved == this->rotationFrequency * 4){        
        this->camMoveDelta = Ogre::Vector3(1,0,0)* this->rotationConstant;
        this->camTimesMoved = 0;
    }
    this->camTimesMoved++;
    if(runFrames != -1 && --runFrames == 0){
        quit();
    }
}

void MyTestApp::quit(){
    this->printFrameRateInfo("==FrameInfo==");
    getRoot()->queueEndRendering();
}

//! [setup]
void MyTestApp::setup(void)
{
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("/usr/share/OGRE/Media/414", "FileSystem");
    // do not forget to call the base first
    OgreBites::ApplicationContext::setup();
    this->camTimesMoved = 0;
    // register for input events
    addInputListener(this);

    // get a pointer to the already created root
    Ogre::Root* root = getRoot();
	// ST_GENERIC is octree, ST_INTERIOR is BSP
    Ogre::SceneManager* scnMgr = 0;
	if(this->useOctree)
	{
        std::cout << "Rendering using Octree Scene Manager.\n";
    	scnMgr = root->createSceneManager("OctreeSceneManager");
	}
	else
	{
        std::cout << "Rendering using Default Scene Manager.\n";
    	scnMgr = root->createSceneManager("DefaultSceneManager");
	}

    // register our scene with the RTSS
    Ogre::RTShader::ShaderGenerator* shadergen = Ogre::RTShader::ShaderGenerator::getSingletonPtr();
    shadergen->addSceneManager(scnMgr);

    // without light we would just get a black screen    
	scnMgr->setAmbientLight(Ogre::ColourValue(0.2, 0.2, 0.2));

	// probably get rid of this...
	Ogre::Light* light = scnMgr->createLight("MainLight");
	Ogre::SceneNode* lightNode = scnMgr->getRootSceneNode()->createChildSceneNode();
	lightNode->setPosition(0, 10, 7500);
	//lightNode->attachObject(light);

    // also need to tell where we are
    Ogre::SceneNode* camNode = scnMgr->getRootSceneNode()->createChildSceneNode();
    camNode->setPosition(0, 10, 27);
    camNode->lookAt(Ogre::Vector3(-10, 0, 0), Ogre::Node::TS_PARENT);
    camMoveDelta = Ogre::Vector3(1,0,0) * this->rotationConstant;
    // create the camera
    cam = scnMgr->createCamera("myCam");
    cam->setNearClipDistance(5); // specific to this sample
    cam->setAutoAspectRatio(true);
    camNode->attachObject(cam);

    // and tell it to render into the main window
    getRenderWindow()->addViewport(cam);

    Ogre::Entity *e = scnMgr->createEntity("terrain", "tutoriallevel.mesh");
	sn = scnMgr->getRootSceneNode()->createChildSceneNode("MySceneNode");
    cam->setAutoTracking(true,sn);
	sn->attachObject(e);
}
//! [setup]
void MyTestApp::printFrameRateInfo(char* prefix){
    const Ogre::RenderTarget::FrameStats& stats = getRenderWindow()->getStatistics();
    std::cout << prefix << "Average: " << stats.avgFPS << "\n";
    std::cout << prefix << "Best FPS: " << stats.bestFPS << "\n";
    std::cout << prefix << "Worst FPS: " << stats.worstFPS << "\n";
    std::cout << prefix << "Best Frame Time: " << stats.bestFrameTime << " ms" << "\n";
    std::cout << prefix << "Worst Frame Time: " << stats.worstFrameTime << " ms" << "\n";
    
}

//! [main]
int main(int argc, char *argv[])
{
    bool useOctree = false;
    FILE *options = fopen("./options","r");
    int runFrames,rotationFrequency;
    float rotationConstant;
    if(options != NULL){
        fscanf(options,"numFrames=%i\n",&runFrames);
        fscanf(options,"rotationFrequency=%i\n",&rotationFrequency);
        fscanf(options,"rotationConstant=%f\n",&rotationConstant);
        std::cout << "Options:\n" << runFrames <<" , " << rotationFrequency << " , " << rotationConstant << "\n";
    }
    else{
        std::cout << "No options file. Using default values";
        runFrames = -1;
        rotationFrequency = 15;
        rotationConstant = 1;
    }
    if(argc == 2)
    {
        if(strcmp(argv[1], "-o") == 0)
        {
            useOctree = true;
        }
        else if(strcmp(argv[1], "-d") == 0)
        {
            useOctree = false;
        }
        else
        {
            std::cout << "Scene manager selection not recognized.\n" << usageText;
            std::cout << argv[1];
            return -1;
        }
    }
    else
    {
        std::cout << "Parameter format not recognized.\n" << usageText;
        return -1;
    }
    
    MyTestApp app(useOctree,runFrames,rotationFrequency,rotationConstant);
    app.initApp();
    app.getRoot()->startRendering();
    app.closeApp();
    return 0;
}
//! [main]
