#include <iostream>
#include <string>

#include <Ogre.h>
#include <OgreApplicationContext.h>

std::string usageText = "<appname> <-o or -d>\n-o is octree -d is default.\n";

class MyTestApp : public OgreBites::ApplicationContext, public OgreBites::InputListener
{
public:
    MyTestApp(bool useBSP);
    void setup();
	Ogre::SceneManager* create_scene();
    bool keyPressed(const OgreBites::KeyboardEvent& evt);
private:
	bool useOctree; // enable Octree culling optimization
};

//! [constructor]
MyTestApp::MyTestApp(bool useOctree) : OgreBites::ApplicationContext("OgreTutorialApp")
{
	this->useOctree = useOctree;
}
//! [constructor]

//! [key_handler]
bool MyTestApp::keyPressed(const OgreBites::KeyboardEvent& evt)
{
    if (evt.keysym.sym == SDLK_ESCAPE)
    {
        getRoot()->queueEndRendering();
    }
    return true;
}
//! [key_handler]

//! [setup]
void MyTestApp::setup(void)
{
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("/usr/share/OGRE/Media/414", "FileSystem");
    // do not forget to call the base first
    OgreBites::ApplicationContext::setup();
    
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

    // create the camera
    Ogre::Camera* cam = scnMgr->createCamera("myCam");
    cam->setNearClipDistance(5); // specific to this sample
    cam->setAutoAspectRatio(true);
    camNode->attachObject(cam);

    // and tell it to render into the main window
    getRenderWindow()->addViewport(cam);

    Ogre::Entity *e = scnMgr->createEntity("terrain", "tutoriallevel.mesh");
	Ogre::SceneNode *sn = scnMgr->getRootSceneNode()->createChildSceneNode("MySceneNode");
	sn->attachObject(e);
}
//! [setup]

//! [main]
int main(int argc, char *argv[])
{
    bool useOctree = false;
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
    MyTestApp app(useOctree);
    app.initApp();
    app.getRoot()->startRendering();
    app.closeApp();
    return 0;
}
//! [main]
