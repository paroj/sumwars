#include "application.h"

Application::Application()
{
	// Anwendung initialisieren
	bool ret = init();

	if (ret == false)
	{
		// ERRORMSG("Initialisation failed, stop");
		m_shutdown = true;
	}

}

bool Application::init()
{
	bool ret = true;

	// Ogre Root erzeugen
	// pluginfile: plugins.cfg
	// configfile: keines
	#ifdef WIN32
        #ifdef _DEBUG
            m_ogre_root = new Ogre::Root("plugins_win_d.cfg", "ogre.cfg");
        #else
            m_ogre_root = new Ogre::Root("plugins_win.cfg", "ogre.cfg");
        #endif
    #else
        m_ogre_root = new Ogre::Root("plugins.cfg", "ogre.cfg");
    #endif

	if (m_ogre_root ==0)
			return false;

	// Ogre configurieren
	ret = configureOgre();
	if (ret==false)
	{
		 ERRORMSG("Configuring Ogre failed");
		return false;
	}

	// Ogre initialisieren
	ret = initOgre();
	if (ret==false)
	{
		 ERRORMSG("Initialisation of Ogre failed");
		return false;
	}

	// Ressourcen festlegen
	ret = setupResources();
	if (ret==false)
	{
		 ERRORMSG("Setting up Resources failed");
		return false;
	}

	//Gettext initialisieren
	ret = initGettext();
	if (ret==false)
	{
		ERRORMSG("Initialisation of Gettext failed");
		return false;
	}


	//CEGUI initialisieren
	ret = initCEGUI();
	if (ret==false)
	{
		 ERRORMSG("Initialisation of CEGUI failed");
		return false;
	}

	// Document anlegen
	ret = createDocument();
	if (ret==false)
	{
		 ERRORMSG("cant create document");
		return false;
	}
	// View anlegen
	ret = createView();
	if (ret==false)
	{
		 ERRORMSG("cant create view");
		return false;
	}

	DEBUG("application initialized");
	// debugging
	//MyFrameListener* mfl = new MyFrameListener(m_main_window,m_document);
	//m_ogre_root->addFrameListener(mfl);

	return true;
}


Application::~Application()
{
	// dynamisch angelegte Objekte in umgekehrter Reihenfolge freigeben

	printf("deleting application\n");
	delete m_main_window;
	delete m_document;
	delete m_ogre_cegui_renderer;
	delete m_ogre_root;
}

void Application::run()
{
	printf("started\n");
	Ogre::Timer timer;
	unsigned long ltime;
	float time=0,ftime=0;
	// float timesum=0;
	// int count=0;
    Ogre::Timer timer2;

	timer.reset();
	while (m_document->getState() != Document::SHUTDOWN)
	{

		ltime =timer.getMicroseconds ();
		ftime = ltime / ( 1000000.0);
		timer.reset();

		if (ftime*1000 >50)
		{
        	DEBUG5("overall frame time was %f",ftime*1000);
		}

	  	timer2.reset();

		update();

		ltime =timer2.getMicroseconds ();
		time = ltime / ( 1000000.0);
		if (time*1000 > 20)
		{
        	DEBUG("update time was %f",time*1000);
		}


		 timer2.reset();
		// run the message pump
		Ogre::WindowEventUtilities::messagePump();
		ltime =timer2.getMicroseconds ();
		time = ltime / ( 1000000.0);
		if (time*1000>20)
		{
			DEBUG("message pump time was %f",time*1000);
		}

		timer2.reset();

		// Document aktualisieren
		m_document->update(ftime*1000);

		ltime =timer2.getMicroseconds ();
		time = ltime / ( 1000000.0);
		if (time*1000 > 20)
		{
			DEBUG("document update time was %f",time*1000);
		}

	/*
		count ++;
		timesum += time;
		if (count ==100)
		{
			count =0;
			timesum *=10;
			DEBUG5("frame time is %f",timesum);
			timesum =0;
		}
		*/

		try
		{
			timer2.reset();

			// View aktualisieren
			DEBUG5("main window update");
			m_main_window->update();

			ltime =timer2.getMicroseconds ();
			time = ltime / ( 1000000.0);
			if (time*1000 > 20)
			{
				DEBUG("view update time was %f",time*1000);
			}
		}
		catch (CEGUI::Exception e)
		{
			ERRORMSG("Error message: %s",e.getMessage().c_str());
		}

		timer2.reset();
		//DEBUG("frame time in s %f",time);
		m_cegui_system->injectTimePulse(ftime);

		ltime =timer2.getMicroseconds ();
		time = ltime / ( 1000000.0);
		if (time*1000 > 20)
		{
			DEBUG("cegui update time was %f",time*1000);
		}

		// rendern
		timer2.reset();

		m_ogre_root->renderOneFrame();

		ltime =timer2.getMicroseconds ();
		time = ltime / ( 1000000.0);
		if (time*1000 > 50)
		{
	         DEBUG5("ogre frame time was %f",time*1000);
		}

	}



}

bool Application::initOgre()
{
	DEBUG("init ogre");
	// Fenster anlegen, Ogre initialisieren
	m_window = m_ogre_root->initialise(true);

	// Szenemanager anlegen
	m_scene_manager = m_ogre_root->createSceneManager(Ogre::ST_GENERIC,"DefaultSceneManager");

	Ogre::LogManager::getSingleton().setLogDetail(Ogre::LL_LOW);
	return true;

}

bool Application::configureOgre()
{
	DEBUG("configure ogre");
	// Logging nur fuer Fehlermeldungen verwenden
	Ogre::LogManager::getSingleton().setLogDetail(Ogre::LL_LOW );
	// Rendering System waehlen
	// Liste aller verfuegbaren Systeme ausgeben lassen und das erste davon nehmen
	Ogre::RenderSystemList *renderSystems = NULL;
	Ogre::RenderSystemList::iterator r_it;
	renderSystems = m_ogre_root->getAvailableRenderers();
	if (renderSystems->empty())
	{
		 ERRORMSG("no rendering system available");
		return false;
	}
	r_it = renderSystems->begin();
	m_ogre_root->setRenderSystem(*r_it);

	// config Dialog anzeigen
	// TODO: Einstellungen direkt setzen oder aus einem File einlesen
	bool result = true;
	//result = m_ogre_root->showConfigDialog();
	m_ogre_root->restoreConfig();
	if (result == false)
	{
		// DEBUG("User pressed Cancel on config Dialog");
		return false;
	}


	return true;
}

bool Application::setupResources()
{
	DEBUG("initalizing resources");
#ifdef NOMIPMAPS
	Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(0);
#endif
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("../resources/models", "FileSystem", "General");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("../resources/materials/scripts", "FileSystem", "General");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("../resources/materials/textures", "FileSystem", "General");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("../resources/particle", "FileSystem", "General");

	// CEGUI Resourcen laden
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("../resources/gui/configs", "FileSystem", "GUI");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("../resources/gui/fonts", "FileSystem", "GUI");

	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("../resources/gui/imagesets", "FileSystem", "GUI");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("../resources/gui/layouts", "FileSystem", "GUI");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("../resources/gui/looknfeel", "FileSystem", "GUI");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("../resources/gui/schemes", "FileSystem", "GUI");

	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("../resources/gui/schemes", "FileSystem", "GUI");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("../resources/itempictures", "FileSystem", "GUI");

	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("../save", "FileSystem", "Savegame");


#if defined(WIN32)
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("c:\\windows\\fonts", "FileSystem", "GUI");
#endif

	// Gruppen initialisieren
	Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup("General");
	Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup("Savegame");
	Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup("GUI");

	Ogre::ResourceGroupManager::getSingleton().loadResourceGroup("General");
	Ogre::ResourceGroupManager::getSingleton().loadResourceGroup("Savegame");
	Ogre::ResourceGroupManager::getSingleton().loadResourceGroup("GUI");


	// Debugging: Meshes direkt anlegen

	Ogre::Plane plane(Ogre::Vector3::UNIT_Y, 0);

	Ogre::MeshManager::getSingleton().createPlane("square44", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane,200,200,1,1,true,1,1,1,Ogre::Vector3::UNIT_X);


	Ogre::MeshManager::getSingleton().createPlane("rect81", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane,400,50,1,1,true,1,1,1,Ogre::Vector3::UNIT_X);



	return true;
}

bool Application::initGettext()
{
	DEBUG("initializing internationalisation");
	#ifndef WIN32
	bool result = false;
	char * language;
	language = getenv("LANG");

	if ((language[0] == 'd' && language[1] == 'e') ||
		(language[0] == 'e' && language[1] == 'n'))
	{
		result = setlocale (LC_MESSAGES, "");
		//result = setlocale (LC_MESSAGES, "de_DE");
	}
	else
	{
		result = setlocale (LC_MESSAGES, "en");

		if (!result)
			result = setlocale (LC_MESSAGES, "en_GB");
		if (!result)
			result = setlocale (LC_MESSAGES, "en_US");
		if (!result)
			result = setlocale (LC_MESSAGES, "en_NZ");
	}

	bindtextdomain ("sumwars","../translation/");
	textdomain ("sumwars");
	return result;
	#endif
	return true;
}

bool Application::initCEGUI()
{
	DEBUG("init CEGUI\n");
	m_ogre_cegui_renderer = new CEGUI::OgreCEGUIRenderer(
	m_window,							// Fenster in das CEGUi rendert
	Ogre::RENDER_QUEUE_OVERLAY,	// Render Queue von CEGUI
	false,								// CEGUI in der Render Queue zuerst bearbeitet
	3000,									// max quads for the UI
 	m_scene_manager						// verwendeter Szenemanager
	);

	// Groesse festlegen
	m_ogre_cegui_renderer->setDisplaySize(CEGUI::Size((int) m_window->getWidth(),(int) m_window->getHeight()));

	// Basisklasse von CEGUI erzeugen
	m_cegui_system = new CEGUI::System(m_ogre_cegui_renderer);


	// Log level
	CEGUI::Logger::getSingleton().setLoggingLevel(CEGUI::Informative);

	// Scheme laden
	CEGUI::SchemeManager::getSingleton().loadScheme((CEGUI::utf8*)"TaharezLook.scheme", (CEGUI::utf8*)"GUI");

	// Imagesets laden
	//CEGUI::Imageset* imgset = CEGUI::ImagesetManager::getSingleton().createImageset("test.imageset");
	CEGUI::ImagesetManager::getSingleton().createImageset("skills.imageset");
	
	// TODO: Dateien laden
	CEGUI::ImagesetManager::getSingleton().createImagesetFromImageFile ("Armbrust.png","Armbrust.png",(CEGUI::utf8*)"GUI");
	CEGUI::ImagesetManager::getSingleton().createImagesetFromImageFile ("Axt.png","Axt.png",(CEGUI::utf8*)"GUI");
	CEGUI::ImagesetManager::getSingleton().createImagesetFromImageFile ("Bogen.png","Bogen.png",(CEGUI::utf8*)"GUI");
	CEGUI::ImagesetManager::getSingleton().createImagesetFromImageFile ("doppelaxt.png","doppelaxt.png",(CEGUI::utf8*)"GUI");
	CEGUI::ImagesetManager::getSingleton().createImagesetFromImageFile ("Hellebarde.png","Hellebarde.png",(CEGUI::utf8*)"GUI");
	CEGUI::ImagesetManager::getSingleton().createImagesetFromImageFile ("Schild_dmg.png","Schild_dmg.png",(CEGUI::utf8*)"GUI");
	CEGUI::ImagesetManager::getSingleton().createImagesetFromImageFile ("Schild.png","Schild.png",(CEGUI::utf8*)"GUI");
	CEGUI::ImagesetManager::getSingleton().createImagesetFromImageFile ("Schwert.png","Schwert.png",(CEGUI::utf8*)"GUI");
	
	MainWindow::registerItemImage("short_sw","set:Schwert.png image:full_image");
	MainWindow::registerItemImage("long_sw","set:Schwert.png image:full_image");
	MainWindow::registerItemImage("battle_axe","set:doppelaxt.png  image:full_image");
	MainWindow::registerItemImage("wood_sh","set:Schild_dmg.png  image:full_image");
	MainWindow::registerItemImage("iron_sh","set:Schild.png  image:full_image");
	

	
	

	// Mauscursor setzen (evtl eher in View auslagern ? )
	m_cegui_system->setDefaultMouseCursor((CEGUI::utf8*)"TaharezLook", (CEGUI::utf8*)"MouseArrow");

	// Font setzen
	CEGUI::FontManager::getSingleton().createFont("DejaVuSerif-8.font", (CEGUI::utf8*)"GUI");
	m_cegui_system->setDefaultFont((CEGUI::utf8*)"DejaVuSerif-8");

	// default ToolTip erzeugen
	CEGUI::System::getSingleton().setDefaultTooltip( (CEGUI::utf8*)"TaharezLook/Tooltip" );
	CEGUI::Tooltip* ttip = CEGUI::System::getSingleton().getDefaultTooltip();
	ttip->setDisplayTime(0);

	// eigene Factorys einfuegen
	return true;
}

bool Application::createDocument()
{
	DEBUG("create document\n");
	m_document = new Document();

	m_document->installShortkey(OIS::KC_ESCAPE,Document::CLOSE_ALL);
	m_document->installShortkey(OIS::KC_I,Document::SHOW_INVENTORY);
	m_document->installShortkey(OIS::KC_C,Document::SHOW_CHARINFO);
	m_document->installShortkey(OIS::KC_T,Document::SHOW_SKILLTREE);
	m_document->installShortkey(OIS::KC_P,Document::SHOW_PARTYMENU);
	m_document->installShortkey(OIS::KC_M,Document::SHOW_CHATBOX);
	m_document->installShortkey(OIS::KC_W,Document::SWAP_EQUIP);
	m_document->installShortkey(OIS::KC_1,Document::USE_POTION);
	m_document->installShortkey(OIS::KC_2,(Document::ShortkeyDestination) (Document::USE_POTION+1));
	m_document->installShortkey(OIS::KC_3,(Document::ShortkeyDestination) (Document::USE_POTION+2));
	m_document->installShortkey(OIS::KC_4,(Document::ShortkeyDestination) (Document::USE_POTION+3));
	m_document->installShortkey(OIS::KC_5,(Document::ShortkeyDestination) (Document::USE_POTION+4));
	m_document->installShortkey(OIS::KC_6,(Document::ShortkeyDestination) (Document::USE_POTION+5));
	m_document->installShortkey(OIS::KC_7,(Document::ShortkeyDestination) (Document::USE_POTION+6));
	m_document->installShortkey(OIS::KC_8,(Document::ShortkeyDestination) (Document::USE_POTION+7));
	m_document->installShortkey(OIS::KC_9,(Document::ShortkeyDestination) (Document::USE_POTION+8));
	m_document->installShortkey(OIS::KC_0,(Document::ShortkeyDestination) (Document::USE_POTION+9));

	return true;
}

bool Application::createView()
{
	DEBUG("create view\n");
	m_main_window = new MainWindow(m_ogre_root, m_cegui_system,m_window,m_document);

	DEBUG("done");
	return true;
}

void  Application::update()
{

}


