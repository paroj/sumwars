#include "mainwindow.h"

#include <string>
#include <sstream>

#include "gettext.h"
#include "player.h"

#include "charinfo.h"
#include "controlpanel.h"
#include "inventory.h"
#include "skilltree.h"
#include "mainmenu.h"
#include "savegamelist.h"
#include "chatline.h"
#include "partyinfo.h"
#include "charcreate.h"
#include "questinfo.h"
#include "minimapwindow.h"
#include "tradewindow.h"
#include "optionswindow.h"
#include "networkwindows.h"
#include "worldmap.h"
#include "messageboxes.h"
#include "dialoguewindow.h"
#include "music.h"

MainWindow::MainWindow(Ogre::Root* ogreroot, CEGUI::System* ceguisystem,Ogre::RenderWindow* window,Document* doc)
{
	m_ogre_root = ogreroot;
	m_cegui_system = ceguisystem;
	m_window = window;
	m_document = doc;
	m_scene_manager = m_ogre_root->getSceneManager("DefaultSceneManager");

	// Initialisieren
	bool res = init();
	if (res==false)
	{
		ERRORMSG("GUI erzeugen fehlgeschlagen");
	}
	
	m_key =0;
	m_highlight_id =0;
	m_ready_to_start = false;
}

bool MainWindow::init()
{
	bool result = true;

	// Eingabegeraete initialisieren
	result &= initInputs();


	// Hauptmenue erzeugen
	result &= setupMainMenu();

	// Hauptspielfenster erzeugen
	result &= setupGameScreen();

	// Aktuelle Ebene setzen
	CEGUI::System::getSingleton().setGUISheet(m_main_menu);

	
	return result;

}


MainWindow::~MainWindow()
{
	m_ois->destroyInputObject(m_mouse);
	m_ois->destroyInputObject(m_keyboard);
	OIS::InputManager::destroyInputSystem(m_ois);

	// Dynamisch angelegte Objekte loeschen
	delete m_scene;
}

bool MainWindow::initInputs()
{
	DEBUGX("init inputs");
	OIS::ParamList pl;
	// Handle fuer das aktuelle Fenster
	unsigned long hWnd;
	m_window->getCustomAttribute("WINDOW", &hWnd);
	// OIS mitteilen fuer welches Fenster die Eingaben abgefangen werden sollen
	pl.insert(OIS::ParamList::value_type("WINDOW", Ogre::StringConverter::toString(hWnd)));
	pl.insert( std::make_pair(std::string("x11_mouse_grab"), std::string("false") ) );
	pl.insert( std::make_pair(std::string("x11_keyboard_grab"), std::string("false")));

	m_ois = OIS::InputManager::createInputSystem( pl );

	// Maus initialisieren
	m_mouse = static_cast<OIS::Mouse*>(m_ois->createInputObject( OIS::OISMouse, true ));
	m_mouse->setEventCallback(this);
	m_mouse->getMouseState().width = (int) m_window->getWidth();
	m_mouse->getMouseState().height = (int) m_window->getHeight();

	// Tastatur initialisieren
	m_keyboard = static_cast<OIS::Keyboard*>(m_ois->createInputObject( OIS::OISKeyboard, true));
	m_keyboard->setEventCallback(this);
	m_keyboard->setTextTranslation (OIS::Keyboard::Unicode);
	return true;
}

bool MainWindow::setupMainMenu()
{
	try
	{

		
		CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();

		// Oberstes Fenster der Hauptmenue Schicht
		m_main_menu = win_mgr.createWindow("DefaultWindow", "MainMenu");
		
		
		CEGUI::Window* img;
		img  = win_mgr.createWindow("TaharezLook/StaticImage", "StartScreenImage");
		m_main_menu->addChildWindow(img);
        img->setProperty("Image", "set:startscreen.png image:full_image");
		img->moveToBack ();
		img->setMousePassThroughEnabled(true);
		
		CEGUI::ProgressBar* bar = static_cast<CEGUI::ProgressBar*>(win_mgr.createWindow("TaharezLook/ProgressBar", "LoadRessourcesProgressBar"));
		m_main_menu->addChildWindow(bar);
		bar->setPosition(CEGUI::UVector2(cegui_reldim(0.3f), cegui_reldim( 0.9f)));
		bar->setSize(CEGUI::UVector2(cegui_reldim(0.40f), cegui_reldim( 0.05f)));
		bar->setWantsMultiClickEvents(false);
		bar->setProgress(0.0);
		
		CEGUI::Window* credits;
		credits = win_mgr.createWindow("TaharezLook/StaticText", "CreditWindow");
		m_main_menu->addChildWindow(credits);
		credits->setPosition(CEGUI::UVector2(cegui_reldim(0.25f), cegui_reldim( 0.20f)));
		credits->setSize(CEGUI::UVector2(cegui_reldim(0.50f), cegui_reldim( 0.6f)));
		credits->setProperty("FrameEnabled", "false");
		credits->setProperty("BackgroundEnabled", "true");
		credits->setProperty("HorzFormatting", "HorzCentred");
		std::string content = "Programming: \
				\n Hans Wulf (Lastmerlin) \
				\n\nGraphics: \
				\n Andreas Schönefeldt \
				\n Steffen Schönefeldt (Nori) \
				\n\n Scripting: \
				\n Steffen Schönefeldt (Nori) \
				\n Hans Wulf (Lastmerlin) \
				\n\nStory: \
				\n Steffen Schönefeldt (Nori) \
				\n Andreas Schönefeldt \
				\n\nWebsite: \
				\n Andreas Schönefeldt \
				\n Michael Kempf (Hangman)";
		credits->setText((CEGUI::utf8*) content.c_str());
		credits->setProperty("BackgroundColours", "tl:99000000 tr:99000000 bl:99000000 br:99000000");
		credits->setAlpha(0.9);
		credits->setFont("DejaVuSerif-12");

		SavegameList* sgl = new SavegameList(m_document);
		m_sub_windows["SavegameList"] = sgl;
		m_main_menu->addChildWindow(sgl->getCEGUIWindow());
		
		MessageQuestionWindow * delchar = new MessageQuestionWindow(m_document,"DeleteChar","Really delete savegame?", "Yes",CEGUI::Event::Subscriber(&SavegameList:: onDeleteCharConfirmClicked, sgl),"No",CEGUI::Event::Subscriber(&SavegameList:: onDeleteCharAbortClicked, sgl));
		m_sub_windows["DeleteChar"] = delchar;
		m_main_menu->addChildWindow(delchar->getCEGUIWindow());
		
		Window * wnd = new MainMenu(m_document);
		m_sub_windows["MainMenu"] = wnd;
		m_main_menu->addChildWindow(wnd->getCEGUIWindow());
		
		wnd = new CharCreate(m_document);
		m_sub_windows["CharCreate"] = wnd;
		m_main_menu->addChildWindow(wnd->getCEGUIWindow());
		
		wnd = new OptionsWindow(m_document,m_keyboard);
		m_sub_windows["Options"] = wnd;
		m_main_menu->addChildWindow(wnd->getCEGUIWindow());
		
		wnd = new HostGameWindow(m_document);
		m_sub_windows["HostGame"] = wnd;
		m_main_menu->addChildWindow(wnd->getCEGUIWindow());
		
		wnd = new JoinGameWindow(m_document);
		m_sub_windows["JoinGame"] = wnd;
		m_main_menu->addChildWindow(wnd->getCEGUIWindow());
		
	// Verbinden mit dem Document
	}
	catch (CEGUI::Exception e)
	{
		ERRORMSG("Error message: %s",e.getMessage().c_str());
	}
	return true;
}

void MainWindow::update(float time)
{
	// Eingaben abfangen
	m_mouse->capture();
	m_keyboard->capture();
	
	if (m_document->getGUIState()->m_left_mouse_pressed || m_document->getGUIState()->m_right_mouse_pressed)
	{
		Vector pos = getIngamePos(m_mouse->getMouseState().X.abs,m_mouse->getMouseState().Y.abs);
		m_document->getGUIState()->m_clicked = pos;
	}
	
	// Tastenwiederholung erzeugen
	if (m_key !=0)
	{
		if (m_key_repeat)
		{
			if ( m_key_repeat_timer.getTime()>60)
			{
				m_key_repeat_timer.start();
				m_cegui_system->injectChar(m_key);
				
				// Sonderbehandlung fuer Backspace
				if (m_key == 8)
				{
					m_cegui_system->injectKeyDown(OIS::KC_BACK);
				}
			}
		}
		else
		{
			if ( m_key_repeat_timer.getTime()>400)
			{
				m_key_repeat_timer.start();
				m_key_repeat = true;
			}
		}
	}

	
	if (m_document->getModified() & Document::SAVEGAME_MODIFIED)
	{
		m_scene->updateCharacterView();
		m_document->setModified(m_document->getModified() & (~Document::SAVEGAME_MODIFIED));
	}
	
	CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();

	// Sprache anpassen
	if (Gettext::getLocaleChanged())
	{
		DEBUG("language changed");
		std::map<std::string, Window*>::iterator it;
		for (it= m_sub_windows.begin(); it != m_sub_windows.end(); ++it)
		{
			it->second->updateTranslation();
		}
	}
	
	// Flags, welche Fenster gezeigt werden
	int wflags = m_document->getGUIState()->m_shown_windows;

	
	
	// Testen ob die Anzeige der Fenster geaendert werden muss
	if (m_document->getModified() & Document::GUISHEET_MODIFIED)
	{
		if (m_document->getGUIState()->m_sheet ==  Document::MAIN_MENU)
		{
			updateMainMenu();
			m_cegui_system->setGUISheet(m_main_menu);
			m_main_menu->addChildWindow(m_sub_windows["Options"]->getCEGUIWindow());
			MusicManager::instance().stop();
		}

		if (m_document->getGUIState()->m_sheet ==  Document::GAME_SCREEN)
		{
			m_cegui_system->setGUISheet(m_game_screen);
			m_game_screen->addChildWindow(m_sub_windows["Options"]->getCEGUIWindow());
			MusicManager::instance().stop();
		}
		m_document->setModified(m_document->getModified() & (~Document::GUISHEET_MODIFIED));
	}
	
	// Testen ob Anzeige der Subfenster geaendert werden muss
	if (m_document->getModified() & Document::WINDOWS_MODIFIED)
	{
		if (m_document->getGUIState()->m_sheet ==  Document::MAIN_MENU)
		{
			updateMainMenu();
		}
		DEBUGX("new shown windows %x",wflags);

		// Auswahlliste Savegames  anzeigen wenn entsprechendes Flag gesetzt
		CEGUI::FrameWindow* savelist = (CEGUI::FrameWindow*) win_mgr.getWindow("SavegameMenu");
		if (wflags & Document::SAVEGAME_LIST)
		{
			
			savelist->setVisible(true);
			m_sub_windows["SavegameList"]->update();
		}
		else
		{
			savelist->setVisible(false);
		}

		// Menu Spielstart anzeigen wenn entsprechendes Flag gesetzt
		CEGUI::FrameWindow* start_menu = (CEGUI::FrameWindow*) win_mgr.getWindow("StartMenu");
		if (wflags & Document::START_MENU)
		{
			start_menu->setVisible(true);
		}
		else
		{
			start_menu->setVisible(false);
		}
		
		CEGUI::FrameWindow* host_game = (CEGUI::FrameWindow*) win_mgr.getWindow("HostGameWindow");
		if (wflags & Document::HOST_GAME)
		{
			m_sub_windows["HostGame"]->update();
			host_game->setVisible(true);
		}
		else
		{
			host_game->setVisible(false);
		}
		
		CEGUI::FrameWindow* join_game = (CEGUI::FrameWindow*) win_mgr.getWindow("JoinGameWindow");
		if (wflags & Document::JOIN_GAME)
		{
			m_sub_windows["JoinGame"]->update();
			join_game->setVisible(true);
		}
		else
		{
			join_game->setVisible(false);
		}
		
		CEGUI::FrameWindow* options = (CEGUI::FrameWindow*) win_mgr.getWindow("OptionsWindow");
		if (wflags & Document::OPTIONS)
		{
			static_cast<OptionsWindow*>(m_sub_windows["Options"])->reset();
			options->setVisible(true);
		}
		else
		{
			options->setVisible(false);
		}
		
		CEGUI::FrameWindow* char_create = (CEGUI::FrameWindow*) win_mgr.getWindow("CharCreate");
		if (wflags & Document::CHAR_CREATE)
		{
			if (!char_create->isVisible())
			{	
				static_cast<CharCreate*>(m_sub_windows["CharCreate"])->updateClassList();
			}
			char_create->setVisible(true);
		}
		else
		{
			char_create->setVisible(false);
		}

		if (m_document->getGUIState()->m_sheet ==  Document::GAME_SCREEN)
		{
			// Charinfo anzeigen wenn entsprechendes Flag gesetzt
			CEGUI::FrameWindow* char_info = (CEGUI::FrameWindow*) win_mgr.getWindow("CharInfo");
			if (wflags & Document::CHARINFO)
			{
				char_info->setVisible(true);
			}
			else
			{
				char_info->setVisible(false);
			}
	
			// Inventar anzeigen wenn entsprechendes Flag gesetzt
			CEGUI::FrameWindow* inventory = (CEGUI::FrameWindow*) win_mgr.getWindow("Inventory");
			if (wflags & Document::INVENTORY)
			{
				inventory->setVisible(true);
			}
			else
			{
				inventory->setVisible(false);
			}
			
			// QuestInfo anzeigen wenn entsprechendes Flag gesetzt
			CEGUI::FrameWindow* quest_info = (CEGUI::FrameWindow*) win_mgr.getWindow("QuestInfo");
			if (wflags & Document::QUEST_INFO)
			{
				quest_info->setVisible(true);
				m_sub_windows["QuestInfo"]->update();
			}
			else
			{
				quest_info->setVisible(false);
			}
			
			CEGUI::FrameWindow* minimap = (CEGUI::FrameWindow*) win_mgr.getWindow("MinimapWindow");
			if (wflags & Document::MINIMAP)
			{
				minimap->setVisible(true);
			}
			else
			{
				minimap->setVisible(false);
			}
			
			CEGUI::FrameWindow* trade = (CEGUI::FrameWindow*) win_mgr.getWindow("TradeWindow");
			if (wflags & Document::TRADE)
			{
				static_cast<TradeWindow*>(m_sub_windows["Trade"])->reset();
				m_sub_windows["Trade"]->update();
				trade->setVisible(true);
			}
			else
			{
				trade->setVisible(false);
			}
			
			CEGUI::FrameWindow* worldmap = (CEGUI::FrameWindow*) win_mgr.getWindow("WorldmapWindow");
			if (wflags & Document::WORLDMAP)
			{
				m_sub_windows["Worldmap"]->update();
				worldmap->setVisible(true);
			}
			else
			{
				worldmap->setVisible(false);
			}
			
			CEGUI::FrameWindow* save_exit = (CEGUI::FrameWindow*) win_mgr.getWindow("SaveExitWindow");
			if (wflags & Document::SAVE_EXIT)
			{
				save_exit->setVisible(true);
				save_exit->setModalState(true);
			}
			else
			{
				save_exit->setVisible(false);
				save_exit->setModalState(false);
			}
			
			// Chat Fenster anzeigen wenn entsprechendes Flag gesetzt
			CEGUI::FrameWindow* chat_window = (CEGUI::FrameWindow*) win_mgr.getWindow("Chatline");
			if (wflags & Document::CHAT)
			{
				// Fokus setzen, wenn das Fenster gerade geoeffnet wurde
				if (!chat_window->isVisible())
				{
					CEGUI::Editbox* chatline;
					chatline = static_cast<CEGUI::Editbox*>(win_mgr.getWindow("Chatline"));
					chatline->activate();
				}
				
				chat_window->setVisible(true);
			}
			else
			{
				chat_window->setVisible(false);
				// Chatzeile deaktivieren
				CEGUI::Editbox* chatline;
				chatline = static_cast<CEGUI::Editbox*>(win_mgr.getWindow("Chatline"));
				chatline->deactivate();
			}
			
			CEGUI::FrameWindow* party_info = (CEGUI::FrameWindow*) win_mgr.getWindow("PartyInfo");
			if (wflags & Document::PARTY)
			{
				party_info->setVisible(true);
			}
			else
			{
				party_info->setVisible(false);
			}
	
			if (m_document->getLocalPlayer()!=0)
			{
				// Skilltree anzeigen wenn entsprechendes Flag gesetzt
				
				CEGUI::TabControl* skilltree = (CEGUI::TabControl*) win_mgr.getWindow("Skilltree");
				if (wflags & Document::SKILLTREE)
				{
					skilltree->setVisible(true);
				}
				else
				{
					skilltree->setVisible(false);
				}
				
			}
		}
		
		m_document->setModified(m_document->getModified() & ~Document::WINDOWS_MODIFIED);
	}
	
	if (wflags & Document::OPTIONS)
	{
		// Fenster Optionen aktualisieren
		m_sub_windows["Options"]->update();
	}

	// Objekte aus dem Dokument darstellen
	if (m_document->getLocalPlayer()!=0 && m_document->getLocalPlayer()->getRegion()!=0)
	{

		// ObjectInfo aktualisieren
		updateObjectInfo();
		
		updateItemInfo();
		updateRegionInfo();
		updateChatContent();
		updateDamageVisualizer();
		
		// Szene aktualisieren
		m_scene->update(time);
		
		// Bild am Curso aktualisieren
		updateCursorItemImage();
		
		// Informationen ueber Partymitglieder aktualisieren
		updatePartyInfo();

		if (wflags & Document::CHARINFO)
		{
			// Fenster CharacterInfo aktualisieren
			m_sub_windows["CharInfo"]->update();
		}

		if (wflags & Document::CHAT)
		{
			// Fenster CharacterInfo aktualisieren
			m_sub_windows["Chatline"]->update();
		}

		if (wflags & Document::MINIMAP)
		{
			// Fenster Minimap aktualisieren
			m_sub_windows["Minimap"]->update();
		}
		
		
		// Steuerleiste aktualisieren
		m_sub_windows["ControlPanel"]->update();


		if (wflags & Document::INVENTORY)
		{
			// Fenster Inventar aktualisieren
			m_sub_windows["Inventory"]->update();
		}
		if (wflags & Document::SKILLTREE)
		{
			// Skilltree aktualisieren
			m_sub_windows["SkillTree"]->update();
		}
		
		if (wflags & Document::PARTY)
		{
			// Skilltree aktualisieren
			m_sub_windows["PartyInfo"]->update();
		}
		
		if (wflags & Document::TRADE)
		{
			// Skilltree aktualisieren
			m_sub_windows["Trade"]->update();
		}
		
		m_sub_windows["DialogueWindow"]->update();
		
		// + Buttons fuer Levelup aktualisieren
		bool vis = false;
		CEGUI::PushButton* btn;
		btn = static_cast<CEGUI::PushButton*>(win_mgr.getWindow("CharInfoUpgradeButton"));
		if (!(wflags & Document::CHARINFO) && m_document->getLocalPlayer()->getAttributePoints() >0)
		{
			vis = true;
		}
		
		if (btn->isVisible() != vis)
		{
			btn->setVisible(vis);
		}
		
		vis = false;
		btn = static_cast<CEGUI::PushButton*>(win_mgr.getWindow("SkillUpgradeButton"));
		if (!(wflags & Document::SKILLTREE) && m_document->getLocalPlayer()->getSkillPoints() >0)
		{
			vis = true;
		}
		
		if (btn->isVisible() != vis)
		{
			btn->setVisible(vis);
		}
		
		/*
		vis = false;
		CEGUI::Window* label = win_mgr.getWindow("MinimapImage");
		if (wflags & Document::MINIMAP)
		{
			vis = true;
		}
		if (label->isVisible() != vis)
		{
			if (vis)
			{
				label->setProperty("Image", "set:minimap image:minimap_img"); 
			}
			label->setVisible(vis);
		}
		*/
	}
	
	// Musik aktualisieren
	updateMusic();
}


bool MainWindow::setupGameScreen()
{
	DEBUGX("setup game screen");
	try
	{
		// Szene erzeugen
		m_scene = new Scene(m_document,m_window);

		// GUI Elemente erzeugen
		CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();

		// Oberstes Fenster der Hauptmenue Schicht
		m_game_screen =  win_mgr.createWindow("DefaultWindow", "GameScreen");

		// Kontrollleiste anlegen
		setupControlPanel();

		// Inventar Fenster anlegen
		setupInventory();

		// CharInfo Fenster anlegen
		setupCharInfo();

		// Skilltree Fenster anlegen
		setupSkilltree();
		
		setupQuestInfo();

		// Leiste fuer Item/Objekt-Info anlegen
		setupObjectInfo();
		
		setupItemInfo();
		setupRegionInfo();
		setupSaveExitWindow();
		setupChatContent();
		
		// Chatfenster anlegen
		setupChatWindow();
		
		// Fenster fuer Partyinfos
		setupPartyInfo();
		
		// Bild fuer das Item am Cursor
		setupCursorItemImage();
		
		setupMinimap();
		
		setupTrade();
		
		setupWorldmap();
		
		m_sub_windows["DialogueWindow"] = new DialogueWindow(m_document,m_scene);
		
		CEGUI::Window* label;
		label = win_mgr.createWindow("TaharezLook/StaticImage", "CharacterPreviewImage");
		m_main_menu->addChildWindow(label);
		label->setProperty("FrameEnabled", "false");
		label->setProperty("BackgroundEnabled", "false");
		label->setPosition(CEGUI::UVector2(cegui_reldim(0.5f), cegui_reldim( 0.15)));
		label->setSize(CEGUI::UVector2(cegui_reldim(0.5f), cegui_reldim( 0.7f)));
		label->setMousePassThroughEnabled(true);
		label->setInheritsAlpha(false);
		label->setProperty("Image", "set:character image:character_img"); 
		
		label = win_mgr.createWindow("TaharezLook/StaticText", "CharacterPreviewBackground");
		m_main_menu->addChildWindow(label);
		label->setProperty("FrameEnabled", "false");
		label->setProperty("BackgroundEnabled", "true");
		label->setPosition(CEGUI::UVector2(cegui_reldim(0.5f), cegui_reldim( 0.0)));
		label->setSize(CEGUI::UVector2(cegui_reldim(0.5f), cegui_reldim( 1.0f)));
		label->setMousePassThroughEnabled(true);
		label->moveToBack();
	}
	catch (CEGUI::Exception e)
	{
		ERRORMSG("Error message: %s",e.getMessage().c_str());
	}

	return true;
}

void MainWindow::setupControlPanel()
{
	Window* wnd = new ControlPanel(m_document);
	m_sub_windows["ControlPanel"] = wnd;
	
	
	// Charakterinfo anfangs ausblenden
	m_game_screen->addChildWindow(wnd->getCEGUIWindow());
	
	// zwei weitere Buttons die Skilltree bzw Charinfo oeffnen, wenn Punkte zu verteilen sind
	CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();
	CEGUI::PushButton* btn;
	
	btn = static_cast<CEGUI::PushButton*>(win_mgr.createWindow("TaharezLook/Button", "CharInfoUpgradeButton"));
	m_game_screen->addChildWindow(btn);
	btn->setPosition(CEGUI::UVector2(cegui_reldim(0.02f), cegui_reldim( 0.65f)));
	btn->setSize(CEGUI::UVector2(cegui_reldim(0.05f), cegui_reldim( 0.07f)));
	btn->setText("+");
	btn->setWantsMultiClickEvents(false);
	btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&ControlPanel::onButtonCharInfoClicked, static_cast<ControlPanel*>(wnd)));
	btn->setVisible(false);
	
	btn = static_cast<CEGUI::PushButton*>(win_mgr.createWindow("TaharezLook/Button", "SkillUpgradeButton"));
	m_game_screen->addChildWindow(btn);
	btn->setPosition(CEGUI::UVector2(cegui_reldim(0.92f), cegui_reldim( 0.65f)));
	btn->setSize(CEGUI::UVector2(cegui_reldim(0.05f), cegui_reldim( 0.07f)));
	btn->setText("+");
	btn->setWantsMultiClickEvents(false);
	btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&ControlPanel::onButtonSkilltreeClicked, static_cast<ControlPanel*>(wnd)));
	btn->setVisible(false);
}

void MainWindow::setupCharInfo()
{
	Window* wnd = new CharInfo(m_document);
	m_sub_windows["CharInfo"] = wnd;
	
	
	// Charakterinfo anfangs ausblenden
	m_game_screen->addChildWindow(wnd->getCEGUIWindow());
	wnd->getCEGUIWindow()->setVisible(false);
}

void MainWindow::setupInventory()
{
	
	Window* wnd = new Inventory(m_document);
	m_sub_windows["Inventory"] = wnd;
	
	// Inventar anfangs ausblenden
	m_game_screen->addChildWindow(wnd->getCEGUIWindow());
	wnd->getCEGUIWindow()->setVisible(false);
	

}

void MainWindow::setupQuestInfo()
{
	Window* wnd = new QuestInfo(m_document);
	m_sub_windows["QuestInfo"] = wnd;
	
	// anfangs ausblenden
	m_game_screen->addChildWindow(wnd->getCEGUIWindow());
	wnd->getCEGUIWindow()->setVisible(false);
}


void MainWindow::setupSkilltree()
{
	
	SkillTree* wnd = new SkillTree(m_document,m_keyboard);
	m_sub_windows["SkillTree"] = wnd;
	
	// Skilltree anfangs ausblenden

	m_game_screen->addChildWindow(wnd->getCEGUIWindow());
	wnd->getCEGUIWindow()->setVisible(false);

}

void MainWindow::setupChatWindow()
{
	Window* wnd = new ChatLine(m_document);
	m_sub_windows["Chatline"] = wnd;
	
	
	// Inventar anfangs ausblenden
	m_game_screen->addChildWindow(wnd->getCEGUIWindow());
	wnd->getCEGUIWindow()->setVisible(false);
}

void MainWindow::setupCursorItemImage()
{
	CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();
	
	CEGUI::Window* label;
	label = win_mgr.createWindow("TaharezLook/StaticImage", "CursorItemImage");
	m_game_screen->addChildWindow(label);
	label->setProperty("FrameEnabled", "false");
	label->setProperty("BackgroundEnabled", "false");
	label->setPosition(CEGUI::UVector2(cegui_reldim(0.05f), cegui_reldim( 0.05)));
	label->setSize(CEGUI::UVector2(cegui_reldim(0.04f), cegui_reldim( 0.06f)));
	label->setProperty("Image", "set:TaharezLook image:CloseButtonNormal"); 
	label->setVisible(false);
	label->setAlwaysOnTop(true);
	label->setMousePassThroughEnabled(true);
	label->setID(0);
}

void MainWindow::setupMinimap()
{
	Window* wnd = new MinimapWindow(m_document);
	m_sub_windows["Minimap"] = wnd;
	m_game_screen->addChildWindow(wnd->getCEGUIWindow());
	/*
	CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();
	
	CEGUI::Window* label;
 	label = win_mgr.createWindow("TaharezLook/StaticImage", "MinimapImage");
	m_game_screen->addChildWindow(label);
	label->setProperty("FrameEnabled", "false");
	label->setProperty("BackgroundEnabled", "false");
	label->setPosition(CEGUI::UVector2(cegui_reldim(0.05f), cegui_reldim( 0.05)));
	label->setSize(CEGUI::UVector2(cegui_reldim(0.9f), cegui_reldim( 0.8f)));
	label->setProperty("Image", "set:TaharezLook image:CloseButtonNormal"); 
	label->setVisible(false);
	label->setMousePassThroughEnabled(true);
	label->setID(0);
	*/
	
	
}

bool MainWindow::setupObjectInfo()
{
	DEBUGX("setup object info");

	// Fenstermanager
	CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();
	CEGUI::Window* label;

	// Leiste fuer Informationen
	
	CEGUI::ProgressBar* bar = static_cast<CEGUI::ProgressBar*>(win_mgr.createWindow("TaharezLook/ProgressBar", "MonsterHealthProgressBar"));
	m_game_screen->addChildWindow(bar);
	bar->setPosition(CEGUI::UVector2(cegui_reldim(0.2f), cegui_reldim( 0.02f)));
	bar->setSize(CEGUI::UVector2(cegui_reldim(0.6f), cegui_reldim( 0.07f)));
	bar->setWantsMultiClickEvents(false);
	bar->setProperty("MousePassThroughEnabled","true");
	bar->setProgress(1.0);
	
	
	label = win_mgr.createWindow("TaharezLook/StaticText", "ObjectInfoLabel");
	m_game_screen->addChildWindow(label);
	label->setProperty("FrameEnabled", "false");
	label->setProperty("BackgroundEnabled", "false");
	label->setProperty("HorzFormatting", "HorzCentred");
	label->setPosition(CEGUI::UVector2(cegui_reldim(0.2f), cegui_reldim( 0.02f)));
	label->setSize(CEGUI::UVector2(cegui_reldim(0.6f), cegui_reldim( 0.07f)));
	label->setAlpha(1.0);
	label->setProperty("MousePassThroughEnabled","true");
	
	

	return true;
}

bool MainWindow::setupItemInfo()
{
	DEBUGX("setup object info");

	// Fenstermanager
	CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();
	CEGUI::Window* label;

	// Leiste fuer Informationen
	label = win_mgr.createWindow("TaharezLook/StaticText", "ItemInfoLabel");
	m_game_screen->addChildWindow(label);
	label->setProperty("FrameEnabled", "false");
	label->setProperty("BackgroundEnabled", "true");
	label->setProperty("BackgroundColours", "tl:77000000 tr:77000000 bl:77000000 br:77000000"); 
	label->setPosition(CEGUI::UVector2(cegui_reldim(0.2f), cegui_reldim( 0.02f)));
	label->setSize(CEGUI::UVector2(cegui_reldim(0.08f), cegui_reldim( 0.03f)));
	label->setText("");
	label->setVisible(false);
	label->setAlpha(0.9);
	label->setProperty("MousePassThroughEnabled","true");

	return true;
}


bool MainWindow::setupPartyInfo()
{
	
	Window* wnd = new PartyInfo(m_document);
	m_sub_windows["PartyInfo"] = wnd;
	
	// PartyInfo anfangs ausblenden
	m_game_screen->addChildWindow(wnd->getCEGUIWindow());
	wnd->getCEGUIWindow()->setVisible(false);
	
	CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();
	
	CEGUI::Window* img;
	CEGUI::ProgressBar* bar;
	
	std::ostringstream stream;
	
	for (int i=0; i<7; i++)
	{
		stream.str("");
		stream << "PartyMemberImage";
		stream << i;
		
		img = win_mgr.createWindow("TaharezLook/StaticImage",stream.str());
		m_game_screen->addChildWindow(img);
		img->setProperty("FrameEnabled", "true");
		img->setProperty("BackgroundEnabled", "true");
		img->setPosition(CEGUI::UVector2(cegui_reldim(0.01f), cegui_reldim( 0.01f + 0.1f *i)));
		img->setSize(CEGUI::UVector2(cegui_reldim(0.05f), cegui_reldim( 0.07f)));
		img->setID(i);
		img->setVisible(false);
		img->subscribeEvent(CEGUI::Window::EventMouseButtonDown, CEGUI::Event::Subscriber(&MainWindow::onPartyMemberClicked, this));
		
		stream.str("");
		stream << "PartyMemberHealthBar";
		stream << i;
		
		bar = static_cast<CEGUI::ProgressBar*>(win_mgr.createWindow("TaharezLook/ProgressBar", stream.str()));
		m_game_screen->addChildWindow(bar);
		m_game_screen->addChildWindow(bar);
		bar->setPosition(CEGUI::UVector2(cegui_reldim(0.01f), cegui_reldim( 0.08f+ 0.1f *i)));
		bar->setSize(CEGUI::UVector2(cegui_reldim(0.05f), cegui_reldim( 0.02f)));
		bar->setProgress(1.0);
		bar->setVisible(false);
	}
	
	return true;
}

void MainWindow::setupTrade()
{
	Window* wnd = new TradeWindow(m_document);
	m_sub_windows["Trade"] = wnd;
	
	
	// Inventar anfangs ausblenden
	m_game_screen->addChildWindow(wnd->getCEGUIWindow());
	wnd->getCEGUIWindow()->setVisible(false);
}


void MainWindow::setupWorldmap()
{
	Window* wnd = new Worldmap(m_document);
	m_sub_windows["Worldmap"] = wnd;
	
	
	// Inventar anfangs ausblenden
	m_game_screen->addChildWindow(wnd->getCEGUIWindow());
	wnd->getCEGUIWindow()->setVisible(false);
}

void MainWindow::setupRegionInfo()
{
	CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();
	
	CEGUI::Window* label;
	label = win_mgr.createWindow("TaharezLook/StaticText", "RegionInfoLabel");
	m_game_screen->addChildWindow(label);
	label->setProperty("FrameEnabled", "false");
	label->setProperty("BackgroundEnabled", "false");
	label->setProperty("HorzFormatting", "HorzCentred");
	label->setPosition(CEGUI::UVector2(cegui_reldim(0.25f), cegui_reldim( 0.05)));
	label->setSize(CEGUI::UVector2(cegui_reldim(0.5f), cegui_reldim( 0.06f)));
	label->setVisible(false);
	label->setAlwaysOnTop(true);
	label->setMousePassThroughEnabled(true);
	label->setID(0);
}

void MainWindow::setupSaveExitWindow()
{
	Window* wnd = new SaveExitWindow(m_document);
	m_sub_windows["saveExit"] = wnd;
	
	
	// Inventar anfangs ausblenden
	m_game_screen->addChildWindow(wnd->getCEGUIWindow());
	wnd->getCEGUIWindow()->setVisible(false);
}

void MainWindow::setupChatContent()
{
	CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();
	
	CEGUI::Window* label;
	label = static_cast<CEGUI::MultiLineEditbox*>(win_mgr.createWindow("TaharezLook/StaticText", "ChatContent"));
	m_game_screen->addChildWindow(label);
	label->setProperty("FrameEnabled", "false");
	//label->setProperty("BackgroundEnabled", "false");
	label->setPosition(CEGUI::UVector2(cegui_reldim(0.07f), cegui_reldim( 0.63f)));
	label->setSize(CEGUI::UVector2(cegui_reldim(.43f), cegui_reldim( 0.2f)));
	label->setProperty("BackgroundColours", "tl:44000000 tr:44000000 bl:44000000 br:44000000");
	label->setProperty("VertFormatting", "VertCentred");
	label->setProperty("HorzFormatting", "WordWrapLeftAligned");
	label->setText("");
	label->setAlwaysOnTop(true);
	label->setMousePassThroughEnabled(true);
	label->setID(0);
}

void  MainWindow::setWindowExtents(int width, int height){
	//Set Mouse Region.. if window resizes, we should alter this to reflect as well
	const OIS::MouseState &ms = m_mouse->getMouseState();
	ms.width = width;
	ms.height = height;
}


void  MainWindow::updateMainMenu()
{
	CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();
	CEGUI::Window* img;
	img  = win_mgr.getWindow("StartScreenImage");
	CEGUI::Window* label;
	label = win_mgr.getWindow("CharacterPreviewImage");
	CEGUI::Window* label2;
	label2 = win_mgr.getWindow("CharacterPreviewBackground");
	
	
	
	int wflags = m_document->getGUIState()->m_shown_windows;
	if (wflags & (Document::SAVEGAME_LIST | Document::CHAR_CREATE))
	{
		img->setVisible(false);
		label->setVisible(true);
		label2->setVisible(true);
	}
	else
	{
		img->setVisible(true);
		label->setVisible(false);
		label2->setVisible(false);
	}
	
	CEGUI::Window* credits;
	credits = win_mgr.getWindow("CreditWindow");
	
	if (wflags & Document::CREDITS)
	{
		credits->setVisible(true);
	}
	else
	{
		credits->setVisible(false);
	}

}

void  MainWindow::updateCharCreate()
{
	m_sub_windows["CharCreate"]->update();

}


void MainWindow::updateCursorItemImage()
{
	Player* player = m_document->getLocalPlayer();
	Item* item = player->getEquipement()->getItem(Equipement::CURSOR_ITEM);
	
	
	
	CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();
	CEGUI::Window* label = win_mgr.getWindow("CursorItemImage");
	
	if (item ==0)
	{
		// Kein Item mehr am Cursor
		// Bild verstecken
		
		if (label->isVisible())
		{
			label->setVisible(false);
		}
	}
	else
	{
		// Es befindet sich ein Item am Cursor
		// Bild zeigen
		
		if (! label->isVisible())
		{
			label->setVisible(true);
		}	
		
		// Groesse des Items am Cursor ermitteln
		unsigned int size = item->m_size;
		
			
		if (size != label->getID())
		{
			// Groesse des Labels setzen
			if (size == Item::BIG)
			{
				label->setSize(CEGUI::UVector2(cegui_reldim(0.060f), cegui_reldim( 0.075f)));
			}
			else if (size == Item::MEDIUM)
			{
				label->setSize(CEGUI::UVector2(cegui_reldim(0.045f), cegui_reldim( 0.06f)));
			}
			else if (size == Item::SMALL)
			{
				label->setSize(CEGUI::UVector2(cegui_reldim(0.033f), cegui_reldim( 0.043f)));
			}
			
			label->setID(size);
		}
		
		std::ostringstream out_stream("");
		out_stream<< ItemWindow::getItemImage(item->m_subtype);
		
		if (label->getProperty("Image")!=out_stream.str())
		{
			label->setProperty("Image", out_stream.str());
			OIS::MouseEvent me(m_mouse,m_mouse->getMouseState ());
			
			mouseMoved(me);
		}
		
	}
}

void MainWindow::updateObjectInfo()
{
	Player* player = m_document->getLocalPlayer();
	Region* reg = player->getRegion();
	if (reg ==0)
		return;
	
	// Ogre Name des Objektes auf das der Mauszeiger zeigt
	std::string objname = "";

	// Fenstermanager
	CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();
	CEGUI::Window* label = win_mgr.getWindow("ObjectInfoLabel");
	CEGUI::Window* itmlabel = win_mgr.getWindow("ItemInfoLabel");
	CEGUI::ProgressBar* bar = static_cast<CEGUI::ProgressBar*>(win_mgr.getWindow( "MonsterHealthProgressBar"));
	
	
	// Position der Maus
	float x = m_mouse->getMouseState().X.abs;
	float y = m_mouse->getMouseState().Y.abs;


	// Position des Mausklicks relativ zum Viewport
	Ogre::Viewport* viewport = m_scene->getViewport();
	float relx = x*1.0/(viewport->getActualWidth());
	float rely = y*1.0/(viewport->getActualHeight());

	// Strahl von der Kamera durch den angeklickten Punkt
	Ogre::Camera* camera = m_scene->getCamera();
	Ogre::Ray ray = camera->getCameraToViewportRay(relx,rely);

	// Query zum Anfragen auf getroffene Objekte
	Ogre::RaySceneQuery * query = m_scene_manager->createRayQuery(ray);
	query->setQueryTypeMask(Ogre::SceneManager::ENTITY_TYPE_MASK);
	query->setSortByDistance (true);
	query->setQueryMask(Scene::WORLDOBJECT | Scene::ITEM);
	
	// Anfrage ausfuehren
	Ogre::RaySceneQueryResult& result = query->execute();
	Ogre::RaySceneQueryResult::iterator it;
	
	int id=0;
	int objid =0;
	if (reg->getCutsceneMode () == true || player->getDialogue () != 0)
	{
		objid = 0;
	}
	else if (m_mouse->getMouseState().buttons !=0)
	{
		// fokussiertes Objekt nicht wechseln, wenn Maustaste gedrueckt
		objid = m_document->getGUIState()->m_cursor_object_id;
	}
	else
	{

		WorldObject* wo;
		GameObject* go;
		float dist, mindist = 100000;
		for (it = result.begin(); it != result.end(); ++it)
		{
			dist = it->distance;
			
			// unsichtbare Objekte nicht fokussierbar
			if (it->movable->getVisible() == false)
			{
				continue;
			}
			
			try
			{
				id = Ogre::any_cast<int>(it->movable->getUserAny());
			}
			catch (Ogre::Exception& e)
			{
				WARNING("Object %s has no ID",it->movable->getName().c_str());
				continue;
			}
			DEBUGX("ray scene query dist %f obj %s mask %x id %i",it->distance, it->movable->getName().c_str(),it->movable->getQueryFlags(), id);
			// nur aktive Objekte beruecksichtigen
			go = reg->getGameObject(id);
			if (go == 0)
			{
				continue;
			}
			if (go->getState() != WorldObject::STATE_ACTIVE)
				continue;
			
			// verbuendete Einheiten bekommen einen Malus - dadurch selektiert man im Kampf bevorzugt Feinde
			wo = dynamic_cast<WorldObject*>(go);
			if (wo != 0)
			{
				if (wo->checkInteractionFlag(WorldObject::USABLE) == false)
				{
					continue;
				}
				
				if (World::getWorld()->getRelation(m_document->getLocalPlayer()->getFraction(), wo->getFraction()) == Fraction::ALLIED)
				{
					dist *=10;
				}
			}
			
			// Pruefung gegen das Polygon
			// get the entity to check
			
			Ogre::Entity *pentity = dynamic_cast<Ogre::Entity*>(it->movable);
			bool rayhit = true;
			if (((wo!=0 && wo->checkInteractionFlag(WorldObject::EXACT_MOUSE_PICKING)) || go->getType() == "DROPITEM")  && pentity != 0)
			{
				rayhit = false;
				
				// mesh data to retrieve
				size_t vertex_count;
				size_t index_count;
				Ogre::Vector3 *vertices;
				unsigned long *indices;
	
				// get the mesh information
				Scene::getMeshInformation(pentity->getMesh(), vertex_count, vertices, index_count, indices,
										pentity->getParentNode()->_getDerivedPosition(),
										pentity->getParentNode()->_getDerivedOrientation(),
										pentity->getParentNode()->_getDerivedScale());
	
				// test for hitting individual triangles on the mesh
				for (int i = 0; i < static_cast<int>(index_count); i += 3)
				{
					// check for a hit against this triangle
					std::pair<bool, Ogre::Real> hit = Ogre::Math::intersects(ray, vertices[indices[i]],
							vertices[indices[i+1]], vertices[indices[i+2]], true, false);
	
					// if it was a hit check if its the closest
					if (hit.first)
					{
						if ( (hit.second < dist))
						{
							// this is the closest so far, save it off
							dist = hit.second;
						}
						rayhit = true;
					}
				}
	
				// free the verticies and indicies memory
				delete[] vertices;
				delete[] indices;
				
				if (rayhit)
				{
					dist /=10;
				}
				
				if (go->getType() == "DROPITEM")
					rayhit = true;
			}
			
			// Objekt wird vom Kamerastrahl geschnitten
			// Distance zum Spieler ausrechnen
			
			if (dist<mindist && rayhit)
			{
				// Objekt ist das bisher naechste
				objid = id;
				mindist = dist;
			}
		}
		m_document->getGUIState()->m_cursor_object=objname;

	}
	
	
	if (objid==0)
	{
		// aktuell kein Objekt unterm Mauszeiger
		m_document->getGUIState()->m_cursor_object_id =0;
		m_document->getGUIState()->m_cursor_item_id =0;

		label->setVisible(false);
		bar->setVisible(false);
		itmlabel->setVisible(false);
			
	}
	
	std::string highlightmat ="white_highlight_alpha_nodepth";
	
	std::string name;
	std::ostringstream string_stream;
	if (objid!=0)
	{
		// es gibt ein ein Objekt unterm Mauszeiger
		// zur ID gehoerendes Objekt
		GameObject* go = reg->getGameObject(objid);
		if (go != 0)
		{
			WorldObject* cwo = dynamic_cast<WorldObject*>(go);
			Creature* cr;
				
			if (cwo !=0)
			{
				// Objekt existiert
				m_document->getGUIState()->m_cursor_object_id = objid;
				m_document->getGUIState()->m_cursor_item_id =0;
				cr = dynamic_cast<Creature*>(cwo);
				if (cr != 0)
				{
					if (!bar->isVisible())
						bar->setVisible(true);
					
					// Objekt ist ein Lebewesen
					// Lebenspunkte anfuegen
					string_stream<< dgettext("sumwars-xml",cr->getName().c_str());
					float perc = cr->getDynAttr()->m_health / cr->getBaseAttrMod()->m_max_health;
					if (bar->getProgress() != perc)
					{
						bar->setProgress(perc);
					}
				}
				else
				{
					string_stream<< dgettext("sumwars-xml",cwo->getName().c_str());
					bar->setVisible(false);
				}
				
				if (!label->isVisible())
				{
					label->setVisible(true);
				}
				name = string_stream.str();
				if (label->getText() != (CEGUI::utf8*) name.c_str())
				{
					label->setText((CEGUI::utf8*) name.c_str());
				}
				
				if (World::getWorld()->getRelation(cwo->getFraction(),player) == Fraction::HOSTILE)
				{
					highlightmat ="red_highlight_alpha_nodepth";
				}
			}
			else
			{
				label->setVisible(false);
				bar->setVisible(false);
			}
			
			DropItem* di = dynamic_cast<DropItem*>(go);
			if (di != 0)
			{
				Item* itm = di->getItem();
				m_document->getGUIState()->m_cursor_item_id = id;
				m_document->getGUIState()->m_cursor_object_id =0;
				
				if (!m_document->getGUIState()->m_item_labels)
				{
					itmlabel->setVisible(true);
				}
				
				name = itm->getName();
				
				float len = itmlabel->getArea().getWidth().d_scale;
				
				if (itmlabel->getText() != (CEGUI::utf8*) name.c_str())
				{
					CEGUI::Font* font = itmlabel->getFont();
					float width = font->getTextExtent((CEGUI::utf8*) name.c_str());
					CEGUI::Rect rect = m_game_screen->getInnerRectClipper();
					len = width / rect.getWidth();
					
					itmlabel->setText((CEGUI::utf8*) name.c_str());
				}
				
				if (fabs( itmlabel->getArea().getWidth().d_scale - len) > 0.001)
				{
					DEBUGX("old value %f new value %f",itmlabel->getArea().getWidth().d_scale, len);
					itmlabel->setSize(CEGUI::UVector2(cegui_reldim(len), cegui_reldim( 0.03f)));
				}
				
				std::string propold = itmlabel->getProperty("TextColours").c_str();
				std::string propnew = "tl:FFFFFFFF tr:FFFFFFFF bl:FFFFFFFF br:FFFFFFFF";
				if (itm->m_rarity == Item::MAGICAL)
				{
					propnew = "tl:FF8888FF tr:FF8888FF bl:FF8888FF br:FF8888FF";
				}
				if (propold != propnew)
				{
					itmlabel->setProperty("TextColours", propnew); 
				}
				
				
				std::pair<float,float> rpos = m_scene->getProjection(di->getPosition());
				itmlabel->setPosition(CEGUI::UVector2(CEGUI::UDim(std::max(0.0,rpos.first-0.03),0), CEGUI::UDim(std::max(0.0,rpos.second-0.05),0)));
			}
			else
			{
				itmlabel->setVisible(false);
			}
		}
		else
		{
			DEBUG("no object with id %i",objid);
			m_document->getGUIState()->m_cursor_object_id =0;
		}
	}

	int hlid = objid;
	if (hlid == player->getId())
		hlid =0;
	
	if (m_highlight_id !=  hlid)
	{
		GraphicObject* grobj = m_scene->getGraphicObject(m_highlight_id);
		if (grobj != 0)
		{
			grobj->setHighlight(false,"red_highlight_alpha_nodepth");
		}
		
		if (objid != 0)
		{
			grobj = m_scene->getGraphicObject(hlid);
			if (grobj != 0)
			{
				grobj->setHighlight(true,highlightmat);
			}
		}
		m_highlight_id = hlid;
	}

}

void MainWindow::updateItemInfo()
{
	// Zaehler wie viele Labels fuer Items existieren
	static int lcount =0;
	
	// Fenstermanager
	CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();
	CEGUI::Window* label = win_mgr.getWindow("ItemInfoLabel");
	
	
	
	DropItem* di;
	
	Player* player = m_document->getLocalPlayer();
	Vector plpos = player->getShape()->m_center;
	
	DropItemMap* itms = player->getRegion()->getDropItems();
	DropItemMap::iterator it;
	
	if (m_document->getGUIState()->m_item_labels)
	{
		label->setVisible(false);
		
		// Alt gedrueckt
		// fuer alle Items ein Label erstellen
		std::ostringstream stream;
		int nr =0;
		std::string name;
		std::pair<float,float> rpos;
		
		float height = 0.030;
		int num = (int) (0.9/height);
		// fuer jede 0.025 hohe Spalte ein Element des Vektors
		// fuer jede Spalte eine Liste von Wertepaaren: Anfang, Ende
		std::vector< std::list < std::pair<float,float> > > itempos(num);
		
		// Lokales Koordinatensystem fuer die Items herstellen
		Vector pu,pl,pll,pmr,pml,ox,oy;
		pu = getIngamePos(0.5,0.1,true);
		pl = getIngamePos(0.5,0.9,true);
		pll = getIngamePos(0,0.9,true);
		pmr = getIngamePos(1.0,0.5,true);
		pml = getIngamePos(0,0.5,true);
		
		oy = pu - pl;
		ox = pmr - pml;
		
		Vector po = pll - pml;
		po.projectOn(ox);
		po = pll - po;
		
		// Abfangen numerischer Schwankungen
		static Vector oldoy = Vector(0,-1);
		
		if (fabs(oldoy.angle(oy)) < 0.01 && fabs(oy.getLength() - oldoy.getLength())<0.01 )
		{
			oy = oldoy;
		}
		else
		{
			oldoy = oy;
		}
		DEBUGX("oy %f %f",oy.m_x, oy.m_y);
		
		// Skalierung der Achsen
		float oxl = ox.getLength();
		float oyl = oy.getLength();
		
		float xscal = oxl;
		float yscal = oyl*height/0.9;
		
		ox.normalize();
		oy.normalize();

		DEBUGX("scal %f %f",xscal,yscal);
		
		// Ursprung des neuen Koordinatensystems
		Vector ypart = po;
		float ylen = ypart*oy;
		ypart.projectOn(oy);
		Vector xpart = po - ypart;
		
		DEBUGX("xpart %f %f",xpart.m_x, xpart.m_y);
		DEBUGX("ypart %f %f",ypart.m_x, ypart.m_y);
		float yoffs = ylen/yscal - roundf(ylen/yscal);
		
		
		Vector o = xpart + oy*(yscal*( roundf(ylen/yscal)));
		
		DEBUGX("pll %f %f %f",pll.m_x, pll.m_y,ylen);
		DEBUGX("o %f %f ", o.m_x, o.m_y);
		
		
		DEBUGX("\nitems\n");
		
		// Distanz zur wahren Position
		float optdist,dist;
		int optrow;
		float optcol;
		float len;
		// Begrenzung fuer den Schriftzug
		float lbound=0,rbound =1;
		int app;
		
		std::string propnew, propold;
		
		Vector tpos,pos;
		for (it = itms->begin();it != itms->end();++it)
		{
			
			di = it->second;
			
			
			name = di->getItem()->getName();
			DEBUGX(" ");
			DEBUGX(" %s %i",name.c_str(), di->getId());
			
			tpos = di->getPosition();
			tpos -= o;
			
			pos.m_x = tpos*ox;
 			pos.m_y = tpos*oy;
			
			DEBUGX("tpos %f %f  %f",(tpos+o).m_x, (tpos+o).m_y,yscal);
			DEBUGX("test %f %f %f   oy %f %f",o*oy/yscal, (tpos+o)*oy/yscal, pos.m_y/yscal,oy.m_x, oy.m_y);
			
			// Position auf dem Bildschirm ermitteln
			DEBUGX("pos %f %f  -- %f",pos.m_x, pos.m_y,pos.m_y/yscal);
			rpos.first = pos.m_x / oxl;
			rpos.second = 0.9-pos.m_y / oyl*0.9;
			DEBUGX("rpos %f %f  - %f",rpos.first, rpos.second, rpos.second/height);
			
			// nur Items die wirklich sichtbar sind behandeln
			if (rpos.first<0 || rpos.first>1 || rpos.second<0 || rpos.second>0.9)
			{
				continue;
			}
			
			
			DEBUGX("optpos %i %f",optrow,optcol);
			
			// Laenge des Schriftzugs
			CEGUI::Font* font = label->getFont();
			float width = font->getTextExtent((CEGUI::utf8*) name.c_str());
			CEGUI::Rect rect = m_game_screen->getInnerRectClipper();
			len = width / rect.getWidth();
			
			// eine Laenge fuer die Darstellung (mit Rand), eine ohne
			float reallen = len;
			float margin = 0.004;
			len +=2*margin;
			
			DEBUGX("length %f",len);
			
			std::list < std::pair<float,float> >::iterator jt, optjt;
			// optimale Position ermitteln
			// Distanz zur wahren Position
			optdist = 1000000;
			
			// beste gefundene Platzierung
			rpos.first -= len/2;
			optrow= (int) (rpos.second/height);
			optcol= rpos.first;
			app = 1;
			
			
			
			// schleife ueber die Zeilen
			int row, centerrow = optrow;
			float col= rpos.second;
			int i;
			for (i=0; i<2*num; i++)
			{
				
				lbound=0;
				rbound =1;
				
				
				
				row = centerrow;
				if (i%2 == 0)
					row += (i+1)/2;
				else
					row -= (i+1)/2;
				
				if (row<0 || row >= num)
					continue;
				
				// Zeilen ueberspringen die so weit wegliegen, dass garantiert schon eine bessere Platzierung gefunden ist
				if ((rpos.second- row*height)*(rpos.second- row*height) > optdist)
					continue;
				
				// iterieren ueber die Liste der Gegenstaende, die bisher in der Zeile liegen
				// (genauer: die Luecken zwischen den Labels)
				for (jt = itempos[row].begin(); jt != itempos[row].end(); jt++)
				{
					rbound = jt->first;
					
					// Test ob das Label in die Luecke passt
					if (rbound -lbound < len)
					{
						lbound = jt->second;
						continue;
					}
					
					// Optimale Platzierung des Labels in der Luecke ermitteln
					if (rpos.first+len > rbound)
					{
						col = rbound-len;
					}
					else if (rpos.first<lbound)
					{
						col = lbound;
					}
					else
						col = rpos.first;
					
					dist = (rpos.first-col)*(rpos.first-col) + (centerrow-row)*(centerrow-row)*height*height;
					if (dist+0.0001 < optdist)
					{
						optdist = dist;
						optcol = col;
						optrow = row;
						optjt = jt;
						app = 0;
						
						DEBUGX("optdist %f pos %i %f ",optdist, row, col);
						DEBUGX("lbound %f rbound %f",lbound,rbound);
					}
					
					lbound = jt->second;
				}
				
				// Option, das Label hinter dem letzten bestehenden Label einzufuegen
				rbound = 1;
				if (rbound-lbound >= len)
				{
					if (rpos.first<lbound)
					{
						col = lbound;
					}
					else
						col = rpos.first;
					
					dist = (rpos.first-col)*(rpos.first-col) + (centerrow-row)*(centerrow-row)*height*height;
					
					if (dist+0.0001 < optdist)
					{
						app = 1;
						optdist = dist;
						optcol = col;
						optrow = row;
						
						DEBUGX("optdist %f optrow %i pos %i %f  app %i",optdist, optrow, row, col,app);
						DEBUGX("lbound %f rbound %f",lbound,rbound);
					}
					
				}
				
				
				
			}
			
			if (optdist > 10000)
				continue;
			
			DEBUGX("center %i rpos %f  %f",centerrow,rpos.second, rpos.second-centerrow*height);
			
			rpos.first=optcol;
			rpos.second=(optrow + yoffs) *height;
			
			
			
			DEBUGX("optdist %f optrow %i pos ",optdist, optrow);
			DEBUGX("final position %f %f",rpos.second, rpos.first);
			
			if (app == 1)
			{
				itempos[optrow].push_back(std::make_pair(rpos.first,rpos.first+len));
			}
			else
			{
				DEBUGX("insert before %f %f",optjt->first, optjt->second);
				itempos[optrow].insert(optjt,std::make_pair(rpos.first,rpos.first+len));
			}
			
			if (rpos.first<0 || rpos.first+len>1 || rpos.second<0 || rpos.second+height>0.9)
				continue;
			
			stream.str("");
			stream << "ItemLabel";
			stream << nr;
			
			if (nr >= lcount)
			{
				lcount ++;
				label = win_mgr.createWindow("TaharezLook/StaticText", stream.str());
				m_game_screen->addChildWindow(label);
				label->setProperty("FrameEnabled", "false");
				label->setProperty("BackgroundEnabled", "true");
				label->setProperty("BackgroundColours", "tl:77000000 tr:77000000 bl:77000000 br:77000000"); label->setPosition(CEGUI::UVector2(cegui_reldim(0.2f), cegui_reldim( 0.02f)));
				label->setSize(CEGUI::UVector2(cegui_reldim(reallen), cegui_reldim( 0.03f)));
				label->setText("");
				label->setAlpha(0.9);
				label->subscribeEvent(CEGUI::Window::EventMouseButtonDown, CEGUI::Event::Subscriber(&MainWindow::onDropItemClicked, this));
				
			}
			else
			{
				label = win_mgr.getWindow(stream.str());
				
			}
			label->setVisible(true);
			
			
			if (label->getText() != (CEGUI::utf8*) name.c_str())
			{
	
				label->setText((CEGUI::utf8*) name.c_str());
			}
			
			if (fabs( label->getArea().getWidth().d_scale - reallen) > 0.001)
			{
				DEBUGX("old value %f new value %f",label->getArea().getWidth().d_scale, len);
				label->setSize(CEGUI::UVector2(cegui_reldim(reallen), cegui_reldim( 0.03f)));
			}
			
			label->setID(it->first);
			
			
			label->setPosition(CEGUI::UVector2(CEGUI::UDim(std::max(0.0f,rpos.first+margin),0), CEGUI::UDim(std::max(0.0f,rpos.second),0)));
			
			propold = label->getProperty("TextColours").c_str();
			propnew = "tl:FFFFFFFF tr:FFFFFFFF bl:FFFFFFFF br:FFFFFFFF";
			if (di->getItem()->m_rarity == Item::MAGICAL)
			{
				propnew = "tl:FF8888FF tr:FF8888FF bl:FF8888FF br:FF8888FF";
			}
			if (propold != propnew)
			{
				label->setProperty("TextColours", propnew); 
			}
			nr++;

		}
		
		
		for (; nr<lcount; nr++)
		{
			stream.str("");
			stream << "ItemLabel";
			stream << nr;
			
			label = win_mgr.getWindow(stream.str());
			label->setVisible(false);
		}
		
	}
	else
	{
		std::ostringstream stream;
		for (int i=0; i<lcount; i++)
		{
			stream.str("");
			stream << "ItemLabel";
			stream << i;
			label = win_mgr.getWindow(stream.str());
			label->setVisible(false);
		}
	}
}


void MainWindow::updatePartyInfo()
{
	int windows = m_document->getGUIState()->m_shown_windows;
	if (windows & Document::PARTY)
	{
		// Fenster PartyInfo aktualisieren
		m_sub_windows["PartyInfo"]->update();
	}
	
	CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();
	
	CEGUI::Window* img;
	CEGUI::ProgressBar* bar;
	
	std::ostringstream stream;
	
	int i=0;
	if (!( windows & Document::PARTY || windows & Document::CHARINFO || windows & Document::QUEST_INFO)) 
	{
		float hperc;
		Player* player = m_document->getLocalPlayer();
		Player* pl;
		Party* party = World::getWorld()->getParty(player->getFraction());
		
		std::set<int>::iterator it;
		// Schleife ueber die Mitglieder der Party des Spielers
		for (it =  party->getMembers().begin(); it!= party->getMembers().end(); it++)
		{
			// eigenen Spieler ueberspringen
			if (*it == player->getId())
				continue;
			
			pl =  static_cast<Player*>(World::getWorld()->getPlayer(*it));
			if (pl ==0)
				continue;
			
			stream.str("");
			stream << "PartyMemberImage";
			stream << i;
			img = win_mgr.getWindow(stream.str());
			img->setID(pl->getId());
			
			// Bild setzen und anzeigen
			if (!img->isVisible())
			{
				img->setVisible(true);
			}
			std::string imagestr = pl->getEmotionImage(pl->getSpeakText().m_emotion);
			if (img->getProperty("Image") != imagestr)
			{
				img->setProperty("Image",imagestr);
			}
			
			if (img->getTooltipText() != (CEGUI::utf8*) pl->getName().c_str())
			{
				img->setTooltipText((CEGUI::utf8*) pl->getName().c_str());
			}
			
			stream.str("");
			stream << "PartyMemberHealthBar";
			stream << i;
			bar = static_cast<CEGUI::ProgressBar*>(win_mgr.getWindow(stream.str()));
			
			// Fortschrittsbalken setzen und anzeigen
			hperc = pl->getDynAttr()->m_health / pl->getBaseAttrMod()->m_max_health;
			if (bar->getProgress() != hperc)
			{
				bar->setProgress(hperc);
			}
			
			stream.str("");
			stream << (int) pl->getDynAttr()->m_health<<"/"<< (int) pl->getBaseAttrMod()->m_max_health;
			if (bar->getTooltipText() != stream.str())
			{
				bar->setTooltipText(stream.str());
			}
			
			if (!bar->isVisible())
			{
				bar->setVisible(true);
			}
			
			i++;
		}
	}
	
	
	// restliche Bilder verstecken
	for (;i<7; i++)
	{
		stream.str("");
		stream << "PartyMemberImage";
		stream << i;
		img = win_mgr.getWindow(stream.str());
		if (img->isVisible())
		{
			img->setVisible(false);
		}
		
		stream.str("");
		stream << "PartyMemberHealthBar";
		stream << i;
		bar = static_cast<CEGUI::ProgressBar*>(win_mgr.getWindow(stream.str()));
		if (bar->isVisible())
		{
			bar->setVisible(false);
		}

	}
}

void MainWindow::updateRegionInfo()
{
	static int region = -1;
	static int id = 0;
	static Timer timer;
	
	bool refresh = false;
	
	Player* pl = m_document->getLocalPlayer();	
	if (pl->getRegion() != 0)
	{
		// update, wenn neuer Spieler oder neue Region gesetzt
		if (pl->getId() != id)
		{
			refresh = true;
			id = pl->getId();
		}
		
		if (pl->getRegion()->getId() != region)
		{
			refresh = true;
			region = pl->getRegion()->getId();
			MusicManager::instance().stop();
		}
		
		if (refresh)
		{
			timer.start();
		}
		
		float time = timer.getTime();
		
		bool vis = false;
		if (time > 500 &&  time < 3000)
		{
			vis = true;
		}
		
		CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();
	
		CEGUI::Window* label;
		label = win_mgr.getWindow("RegionInfoLabel");
		
		// nur anzeigen, wenn minimap nicht geoeffnet
		int wflags = m_document->getGUIState()->m_shown_windows;
		vis &= !(wflags & Document::MINIMAP);
		
		if (label->isVisible() != vis)
		{
			label->setVisible(vis);
			
			if (vis == true)
			{
				label->setText( (CEGUI::utf8*) dgettext("sumwars-xml",pl->getRegion()->getName().c_str()));
			}
		}
		
	}
}

void MainWindow::updateDamageVisualizer()
{
	// Zaehler wie viele Labels fuer Schaden existieren
	static int lcount =0;
	CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();
	CEGUI::Window* label;
			
	Player* player = m_document->getLocalPlayer();
	Region* reg = player->getRegion();
	std::map<int,DamageVisualizer>& dmgvis = reg->getDamageVisualizer();
	std::map<int,DamageVisualizer>::iterator it;
	
	int nr =0;
	std::stringstream stream;
	
	std::pair<float,float> pos;
	
	float maxtime = 1000.f;
	
	for (it = dmgvis.begin(); it != dmgvis.end(); ++it)
	{
		pos = m_scene->getProjection(it->second.m_position,1.0f);
		pos.second -= 0.1*(maxtime -it->second.m_time)/maxtime;
		
		
		// nur Kreaturen behandeln, die wirklich zu sehen sind
		if (pos.first <0 || pos.first >1 || pos.second <0 || pos.second >1)
			continue;
		
		stream.str("");
		stream << "DamageVisualizerLabel";
		stream << nr;
		
		if (nr >= lcount)
		{
			lcount ++;
			label = win_mgr.createWindow("TaharezLook/StaticText", stream.str());
			m_game_screen->addChildWindow(label);
			label->setProperty("FrameEnabled", "false");
			label->setProperty("BackgroundEnabled", "false");
			label->setText("");
			label->setAlpha(0.9);
			label->setFont("DejaVuSerif-12");
			label->setProperty("TextColours","tl:FFFF5555 tr:FFFF5555 bl:FFFF5555 br:FFFF5555" ); 
			label->	setMousePassThroughEnabled(true);
		}
		else
		{
			label = win_mgr.getWindow(stream.str());
				
		}
		
		stream.str("");
		stream << it->second.m_number;
		
		if (label->getText() != (CEGUI::utf8*) stream.str().c_str())
		{
			label->setText((CEGUI::utf8*) stream.str().c_str());
		}
		
		std::string fontname = "DejaVuSerif-16";
		if (it->second.m_size == 1)
		{
			fontname = "DejaVuSerif-12";
		}
		if (label->getFont()->getProperty("Name") != fontname)
		{
			label->setFont(fontname);
		}
		
		CEGUI::Font* font = label->getFont();
			
		float width = font->getTextExtent((CEGUI::utf8*) stream.str().c_str())+15;
		float height = font->getFontHeight() +15;
			
		label->setSize(CEGUI::UVector2(CEGUI::UDim(0,width),  CEGUI::UDim(0,height)));
		label->setPosition(CEGUI::UVector2(CEGUI::UDim(pos.first,-width/2), CEGUI::UDim(pos.second,-height)));
		
		float fadetime = 400;
		if (it->second.m_time < fadetime)
		{
			label->setAlpha(0.9 * it->second.m_time / fadetime);
		}
		else
		{
			label->setAlpha(0.9);
		}
		
		label->setVisible(true);
		nr ++;
	}
	
	for (; nr<lcount; nr++)
	{
		stream.str("");
		stream << "DamageVisualizerLabel";
		stream << nr;
			
		label = win_mgr.getWindow(stream.str());
		label->setVisible(false);
	}
}

void MainWindow::updateChatContent()
{
	static Timer timer;
	int windows = m_document->getGUIState()->m_shown_windows;
	
	CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();
	Player* pl = m_document->getLocalPlayer();
	
	CEGUI::Window* label;
	label =  win_mgr.getWindow("ChatContent");
	
	if (label->getText() != (CEGUI::utf8*) pl->getMessages().c_str())
	{
		label->setText((CEGUI::utf8*) pl->getMessages().c_str());
		
	}
	
	bool vis = true;
	if (label->getText() == "" || ( windows & Document::PARTY || windows & Document::CHARINFO || windows & Document::QUEST_INFO))
	{
		vis = false;
	}
	else
	{
		// Fenster auf die richtige Groesse bringen
		CEGUI::Font* fnt = label->getFont();
		CEGUI::Size area(CEGUI::System::getSingleton().getRenderer()->getDisplaySize());

        //TODO
		CEGUI::String text = label->getText();
		float width =  50; //PixelAligned(fnt->getFormattedTextExtent(text, area, CEGUI::LeftAligned));
		
		float maxwidth = area.d_width * 0.43;
		width += 3;
		if (width > maxwidth)
		{
			width = maxwidth;
		}
		
		CEGUI::Size larea = area;
		larea.d_width = width;
		float height =  50; //PixelAligned(fnt->getFormattedLineCount(text, larea, CEGUI::WordWrapLeftAligned) * fnt->getLineSpacing());
        
		float relwidth = width / area.d_width;
		float relheight = (height+6) / area.d_height;
		
		if ( fabs(label->getArea().getWidth().d_scale - relwidth) > 0.0001
				   || fabs(label->getArea().getHeight().d_scale - relheight) > 0.0001)
		{
			label->setPosition(CEGUI::UVector2(cegui_reldim(0.07f), cegui_reldim( 0.82f - relheight)));
			label->setSize(CEGUI::UVector2(cegui_reldim(relwidth), cegui_reldim(relheight)));
		}
		
		pl->updateMessageTimer(timer.getTime());
	}
	
	if (label->isVisible() != vis)
	{
		label->setVisible(vis);
		
	}
	
	timer.start();
}

void MainWindow::updateMusic()
{
	// laufende Musik nicht unterbrechen
	if (MusicManager::instance().isPlaying())
		return;
	
	std::string source;
	if (m_document->getGUIState()->m_sheet ==  Document::MAIN_MENU)
	{
		// Titlescreen
		source ="main_theme.ogg";
	}
	else if (m_document->getGUIState()->m_sheet ==  Document::GAME_SCREEN)
	{
		if (m_document->getLocalPlayer() == 0)
			return;
		
		// zufaellig einen Track der Region auswaehlen
		Region* reg = m_document->getLocalPlayer()->getRegion();
		source = "";
		if (reg != 0)
		{
			std::list<MusicTrack> tracks = reg->getMusicTracks();
			std::list<MusicTrack>::iterator it;
			
			if (!tracks.empty())
			{
				// Auslosung
				int nr = Random::randi(tracks.size());
				it = tracks.begin();
				while (nr > 0 )
				{
					it++;
					nr --;
				}
				
				source = *it;
			}
		}
	}
	
	if (source == "")
		return;
	
	// Titelmusik laden
	Ogre::FileInfoListPtr files;
	Ogre::FileInfoList::iterator it;
	files = Ogre::ResourceGroupManager::getSingleton().findResourceFileInfo("music",source);
	
	it = files->begin();
	if (it != files->end())
	{
		std::string filename;
		filename = it->archive->getName();
		filename += "/";
		filename += it->filename;
		
		MusicManager::instance().play(filename);
		MusicManager::instance().update();
	}
}

Vector MainWindow::getIngamePos(float screenx, float screeny, bool relative)
{
	// Position des Mausklicks relativ zum Viewport
	Ogre::Viewport* viewport = m_scene->getViewport();
	float relx,rely;
	if (relative)
	{
		relx = screenx;
		rely = screeny;
	}
	else
	{
		relx = screenx*1.0/(viewport->getActualWidth());
		rely = screeny*1.0/(viewport->getActualHeight());
	}
	DEBUGX("relative Koordinaten %f %f",relx,rely);

	// Strahl von der Kamera durch den angeklickten Punkt
	Ogre::Camera* camera = m_scene->getCamera();
	Ogre::Ray ray = camera->getCameraToViewportRay(relx,rely);

	// Ursprung und Richtung des Strahles
	const Ogre::Vector3& orig = ray.getOrigin();
	DEBUGX("ray orig %f %f %f",orig.x,orig.y,orig.z);
	const Ogre::Vector3& dir = ray.getDirection();
	DEBUGX("ray dir %f %f %f",dir.x,dir.y,dir.z);

	// Schnittpunkt mit der Ebene y=0 ausrechnen
	
	Vector ret;
	float dy = dir.y;
	if (dir.y>=0)
	{
		dy = -0.001;
	}
	
	
	// Durchstosspunkt durch die Ebene y=0
	Ogre::Vector3 p = orig + dir*(orig.y/(-dy));
	DEBUGX("schnittpunkt %f %f %f",p.x,p.y,p.z);

	// Umrechnen in Spielkoordinaten
	ret.m_x = p.x/50;
	ret.m_y = p.z/50;
	DEBUGX("Punkt in Spielkoordinaten %f %f",ret.m_x,ret.m_y);
	
	return ret;

}

// MouseListener
bool MainWindow::mouseMoved(const OIS::MouseEvent &evt) {
	m_cegui_system->injectMouseWheelChange(evt.state.Z.rel);
	
	
	//return m_cegui_system->injectMouseMove(evt.state.X.rel, evt.state.Y.rel);
	//DEBUG("injection position %i %i",evt.state.X.abs,evt.state.Y.abs);
	m_document->onMouseMove(evt.state.X.rel, evt.state.Y.rel,evt.state.Z.rel);
	
	CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();
	CEGUI::Window* label = win_mgr.getWindow("CursorItemImage");
	
	int off = 0;
	if (label->getID() == Item::BIG)
		off = 24;
	if (label->getID() == Item::MEDIUM)
		off = 16;
	if (label->getID() == Item::SMALL)
		off = 12;
	
	
	label->setPosition(CEGUI::UVector2(CEGUI::UDim(0,std::max(0,evt.state.X.abs-off)),CEGUI::UDim(0,std::max(0,evt.state.Y.abs- off))));
	
	
	return m_cegui_system->injectMousePosition(evt.state.X.abs,evt.state.Y.abs);
}

bool MainWindow::mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID btn)
{
	m_gui_hit = false;

	CEGUI::MouseButton button = CEGUI::NoButton;

	if (btn == OIS::MB_Left)
		button = CEGUI::LeftButton;
	else if (btn == OIS::MB_Middle)
		button = CEGUI::MiddleButton;
	else if (btn == OIS::MB_Right)
		button = CEGUI::RightButton;
	else
		return true;

    bool ret = false; // CEGUI 0.7 changed its default behaviour a little bit and creates a invisible root window that always processes events

    // if the returned window is anything else than "GameScreen" then CEGUI needs to process the input
    if (!(m_cegui_system->getWindowContainingMouse()->getName() == "GameScreen")) 
    {
        ret = true; // set ret to false if the events where processed by the invisible window
        m_cegui_system->injectMouseButtonDown(button);
    }
    
	if (m_document->getGUIState()->m_sheet ==  Document::MAIN_MENU && m_ready_to_start)
	{
		m_document->onStartScreenClicked();
		return true;
	}

	// Koordinaten an denen der Mausklick stattfand
	int x =evt.state.X.abs;
	int y =evt.state.Y.abs;
	DEBUGX("maus %i %i",x,y);

	Player* player = m_document->getLocalPlayer();
	if (player!=0)
	{
		// Spiel ist mit Server verbunden
		if (btn == OIS::MB_Middle)
		{
			m_document->getGUIState()->m_middle_mouse_pressed=true;
		}

		// Testet, dass man nicht auf die untere Steuerleiste geklickt hat
		if (!ret)
		{
			if (player->getEquipement()->getItem(Equipement::CURSOR_ITEM)!=0)
			{
				// Spieler hat Item in der Hand, fallen lassen
				m_document->dropCursorItem();
			}
			else
			{

				Vector pos = getIngamePos(x,y);

				if (btn == OIS::MB_Left)
				{
					m_document->getGUIState()->m_left_mouse_pressed=true;
					m_document->onLeftMouseButtonClick(pos);
				}
				else if (btn == OIS::MB_Right)
				{
					m_document->getGUIState()->m_right_mouse_pressed=true;
					m_document->onRightMouseButtonClick(pos);
				}


				
			}
		}

	}

	return ret;
}

bool MainWindow::mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID btn) {
	CEGUI::MouseButton button = CEGUI::NoButton;

	if (btn==OIS::MB_Left)
	{
		DEBUGX("Button release");
		m_document->getGUIState()->m_left_mouse_pressed=false;
		m_document->getGUIState()->m_clicked_object_id=0;
	}

	if (btn==OIS::MB_Right)
	{
		DEBUGX("Right Button release");
		m_document->getGUIState()->m_right_mouse_pressed=false;
		m_document->getGUIState()->m_clicked_object_id=0;
	}
	
	if (btn == OIS::MB_Middle)
	{
		m_document->getGUIState()->m_middle_mouse_pressed=false;
	}

	if (btn == OIS::MB_Left)
		button = CEGUI::LeftButton;

	if (btn == OIS::MB_Middle)
		button = CEGUI::MiddleButton;

	if (btn == OIS::MB_Right)
		button = CEGUI::RightButton;

	return m_cegui_system->injectMouseButtonUp(button);
}


bool MainWindow::keyPressed(const OIS::KeyEvent &evt) {
	unsigned int ch = evt.text;
	
	DEBUGX("keycode %i %s %x",evt.key,m_keyboard->getAsString(evt.key).c_str(), ch);
	if (m_document->getGUIState()->m_shown_windows & Document::OPTIONS)
	{
		if (static_cast<OptionsWindow*>(m_sub_windows["Options"])->requestsForKey())
		{
			static_cast<OptionsWindow*>(m_sub_windows["Options"])->setKeyCode(evt.key);
			return true;
		}
	}
	
	if (evt.key == OIS::KC_UNASSIGNED)
	{
		ch &= 0xff;
		m_cegui_system->injectChar((CEGUI::utf32) ch);
		return true;
	}
	
	bool ret =m_cegui_system->injectKeyDown(evt.key);
	
	ret |= m_cegui_system->injectChar(ch);
	
	if (ch != 0 &&  (m_document->getGUIState()->m_shown_windows & Document::CHAT))
	{
		m_key = ch;
		m_key_repeat_timer.start();
		m_key_repeat = false;
	}
	
	
	if (evt.key == OIS::KC_RSHIFT || evt.key == OIS::KC_LSHIFT)
	{
		m_document->getGUIState()->m_shift_hold = true;
	}
	
	if (not ret)
	{
		ret = m_document->onKeyPress(evt.key);
	}
	return ret;
}

bool MainWindow::keyReleased(const OIS::KeyEvent &evt)
{

	m_key =0;
	m_key_repeat = false;
	
	bool ret = m_cegui_system->injectKeyUp(evt.key);
	
	if (evt.key == OIS::KC_RSHIFT || evt.key == OIS::KC_LSHIFT)
	{
		m_document->getGUIState()->m_shift_hold = false;
	}

	if (not ret)
	{
		ret = m_document->onKeyRelease(evt.key);
	}

	return ret;
}

bool MainWindow::onDropItemClicked(const CEGUI::EventArgs& evt)
{
	const CEGUI::MouseEventArgs& we =
			static_cast<const CEGUI::MouseEventArgs&>(evt);
	unsigned int id = we.window->getID();
	
	DEBUGX("pick up item %i",id);
	m_document->onDropItemClick(id);
	return true;
}




bool MainWindow::consumeEvent(const CEGUI::EventArgs& evt)
{
	return true;
}

bool MainWindow::onPartyMemberClicked(const CEGUI::EventArgs& evt)
{
	const CEGUI::MouseEventArgs& we =
			static_cast<const CEGUI::MouseEventArgs&>(evt);
	unsigned int id = we.window->getID();
	
	Player* pl = static_cast<Player*> (World::getWorld()->getLocalPlayer());
	if (World::getWorld() ==0 || pl ==0 || pl->getRegion() ==0)
		return true;
	
	// Spieler auf den die Aktion wirkt
	WorldObject* pl2 = pl->getRegion()->getObject(id);
	if (pl2 ==0)
		return true;
	
	DEBUG("party member Click %i",id);
	
	m_document->getGUIState()->m_cursor_object_id = id;
	if (we.button == CEGUI::LeftButton)
	{
		m_document->onLeftMouseButtonClick(pl2->getPosition());
	}

	if (we.button == CEGUI::RightButton)
	{
		m_document->onRightMouseButtonClick(pl2->getPosition());
	}

	return true;
}

void MainWindow::setReadyToStart(bool ready)
{
	m_ready_to_start = ready;
	
	CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();
	
	CEGUI::ProgressBar* bar = static_cast<CEGUI::ProgressBar*>(win_mgr.getWindow( "LoadRessourcesProgressBar"));
	bar->setVisible(!ready);
}

void MainWindow::setRessourceLoadingBar(float percent)
{
	CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();
	
	CEGUI::ProgressBar* bar = static_cast<CEGUI::ProgressBar*>(win_mgr.getWindow( "LoadRessourcesProgressBar"));
	bar->setProgress(percent);
}





