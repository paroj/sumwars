/*
	Summoning Wars - document.h
	Copyright (C) 2007  Hans Wulf

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


/**
 * \file document.h
 * \defgroup Document \ Dokument
 * \brief Stellt das Dokumentkonzept dar
 * \author Hans Wulf
 * \version 1.0
 * \date 2007/05/28
 * \note Beinhaltet die Klasse Document
 */

#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <string>
#include <list>
#include <map>
#include <set>


// OIS
/*
#ifdef WIN32
    #include "ois\OISEvents.h"
    #include "ois\OISInputManager.h"
    #include "ois\OISMouse.h"
    #include "ois\OISKeyboard.h"
    #include "ois\OISJoyStick.h"
#else
    #include "OISEvents.h"
    #include "OISInputManager.h"
    #include "OISMouse.h"
    #include "OISKeyboard.h"
    #include "OISJoyStick.h"
#endif
*/

#include "networkstruct.h"
#include "constants.h"
#include "projectile.h"
#include "party.h"
#include "clientwobject.h"
#include "clientmplayer.h"
#include "clientnetwork.h"
#include "debug.h"
#include "damage.h"
#include "itemlist.h"
#include "random.h"
#include "serveritem.h"
#include "matrix2d.h"
#include "tiles.h"
#include "dropitem.h"
#include <pthread.h>

#include "gettext.h"

/**
 * \class Document
 * \brief Diese Klasse enthaelt alle Daten fuer Repraesentation der Spielwelt auf der Clientseite.
 *
 * 
 */

#typedef int KeyCode

class Document
{


	public:

	


	/**
	 * \enum State
	 * \brief zaehlt verschiedene Zustaende auf, in denen sich das Dokument befinden kann
	 */
	enum State
	{
		INACTIVE =0,

		CONNECT_REQUEST =1,

		CONNECTING =2,

		CONNECTED =3,

		SHUTDOWN_REQUEST =4,
  
		SHUTDOWN_WRITE_SAVEGAME=8,

		SHUTDOWN =5,

		DISCONNECTED =6,
  
 		START_SERVER =7,
	};


	/**
	 * \enum Modifications
	 * \brief Zaehlt Inhalte auf, die veraendert werden koennen
	 */
	enum Modifications
	{
		NO_MODIFICATIONS=0,
		OBJECTS_MODIFIED=1,
		GUISHEET_MODIFIED=2,
		WINDOWS_MODIFIED=4,
		ITEM_MODIFIED=8,
		ABILITY_MODIFIED=16,
		REGION_MODIFIED = 32,
	};

	/**
	 * \enum GUISheets
	 * \brief Verschiedene Ebenen die angezeigt werden können
	 */
	enum GUISheets
	{
		MAIN_MENU=1,
		GAME_SCREEN=2,
	};

	/**
	 * \enum Windows
	 * \brief Fenster die waerend des Spieles angezeigt werden koennen
	 */
	enum Windows
	{
		NO_WINDOWS =0,
		INVENTORY =1,
		CHARINFO=2,
		SKILLTREE=4,
		MENU =8,
		CHAT = 16,
		PARTY = 32,
		SAVEGAME_LIST = 64,
		START_MENU = 128,
	};

	/**
	 * \enum ShortkeyDestination
	 * \brief Listet Aktion auf, die ueber Kurztasten verfuegbar sind
	 */
	enum ShortkeyDestination
	{
		NO_KEY=0,
		SHOW_INVENTORY =1,
		SHOW_CHARINFO =2,
		SHOW_SKILLTREE =3,
		SHOW_PARTYMENU=4,
		SHOW_CHATBOX =5,
		SWAP_EQUIP=10,
		USE_POTION = 30,
		USE_SKILL_LEFT=100,
		USE_SKILL_RIGHT=300,
	};

	/**
	 * \struct NetworkInfo
	 * \brief Struktur fuer die Daten zum Netzwerk auf Clientseite
	 */
	struct NetworkInfo
	{
		/**
		 * \var m_network_error
		 * \brief false, wenn ein Netzwerkfehler aufgetreten ist
		 */
		bool m_network_error;

		/**
		 * \var m_server_ip[16]
		 * \brief Speichert die IP des Servers
		 */
		char m_server_ip[16];

		/**
		 * \var m_network_slot
		 * \brief Speichert den verwendeten Netzwerk-Slot
		 */
		int m_network_slot;

		/**
		 * \var ClientNetwork* m_network
		 * \brief Objekt für die Netzwerkanbindung des Clients
		 */
		ClientNetwork* m_network;

		/**
		 * \var int m_timeout_counter
		 * \brief Zaehler fuer Timeouts
		 */
		int m_timeout_counter;

		/**
		 * \var bool m_host
		 * \brief Gibt an, ob der eigene Rechner der Host ist
		 */
		bool m_host;
	};

	
	/**
	 * \struct GUIState
	 * \brief Informationen zum aktuellen Zustand der GUI
	 */
	struct GUIState
	{

		/**
		 * \var m_chat_window_content
		 * \brief Inhalt des Chat-Nachrichtenfensters
		 */
		string m_chat_window_content;


		/**
		 * \var m_chat_sendline_content
		 * \brief Inhalt der Chat-Eingabezeile
	 	 */
		string m_chat_sendline_content;

		 /**
		 * \var m_left_mouse_hold
		 * \brief Gibt an, ob die linke Maustaste festgehalten wird
	 	 */
		bool m_left_mouse_hold;

		 /**
		 * \var m_right_mouse_hold
		 * \brief Gibt an, ob die rechte Maustaste festgehalten wird
	 	 */
		bool m_right_mouse_hold;

		/**
		 * \var m_mouse_hold
		 * \brief Gibt an, ob die Shift-Taste festgehalten wird
		 */
		bool m_shift_hold;

		 /**
		 * \var m_clicked_object_id
		 * \brief ID des angeklickten Objekts
	 	 */
		int m_clicked_object_id;

		 /**
		 * \var m_clicked_x
		 * \brief angeklickte x-Koordinate
		  */
		float m_clicked_x;

		 /**
		 * \var m_clicked_y
		 * \brief angeklickte y-Koordinate
		  */
		float m_clicked_y;

		/**
		 * \var float m_left_mouse_hold_time
		 * \brief Zaehler der die Anzahl Frames zaehlt die die linke Maustaste festgehalten wurde
		 */
		float m_left_mouse_hold_time;


		/**
		 * \var float m_right_mouse_hold_time
		 * \brief Zaehler der die Anzahl Frames zaehlt die die rechte Maustaste festgehalten wurde
		 */
		float m_right_mouse_hold_time;

		/**
		 * \var GUISheets* m_sheet
		 * \brief Aktuell angezeigte Ebene
		 */
		GUISheets m_sheet;

		/**
		 * \var int m_shown_windows
		 * \brief aktuell angezeigte Fenster als Bitmaske
		 */
		int m_shown_windows;

		/**
		 * \var KeyCode m_pressed_key
		 * \brief aktuell festgehaltene Taste
		 */
		KeyCode m_pressed_key;

		/**
		 * \var std::string m_cursor_object
		 * \brief Ogre Name des Objektes auf das gerade der Mauszeiger zeigt
		 */
		std::string m_cursor_object;

		/**
		 * \var int m_cursor_object_id
		 * \brief ID des Objektes auf das gerade der Mauszeiger zeigt
		 */
		int m_cursor_object_id;

		/**
		 * \var int m_cursor_item_id
		 * \brief ID des Gegenstandes auf den gerade der Mauszeiger zeigt
		 */
		int m_cursor_item_id;

	};



// Konstruktor / Destruktor
	/**
	 * \fn Document()
	 * \brief Konstruktor
	 *
	 * Legt ein neues Document-Objekt an
	 */
	Document();
	
	
	/**
	 * \fn ~Document()
	 * \brief Destruktor
	 *
	 * Gibt den allokierten Speicher wieder frei
	 */
	~Document();
// Setter und Getter

	/**
	 * \fn void setSaveFile(string s)
	 * \brief Setzt den Name des Savefiles
	 * \param s Name des Savefiles
	 */
	void setSaveFile(string s)
	{
		m_save_file =s;
	}

	/**
	 * \fn string getSaveFile()
	 * \brief Gibt den Name des Savefiles aus
	 */
	string getSaveFile()
	{
		return m_save_file;
	}

	/**
	 * \fn static void* writeSaveFile(void* doc_ptr)
	 * \param doc_ptr Zeiger auf das Dokument
	 * \brief Schreibt das Savefile
	 */
	static void* writeSaveFile(void* doc_ptr);

	/**
	 * \fn NetworkInfo* getNetworkInfo()
	 * \brief Gibt Zeiger auf die Netzwerkinformationen zurueck
	 * return Netzwerkinformation
	 */
	NetworkInfo* getNetworkInfo()
	{
		return &m_network_info;
	}

	/**
	 * \fn GUISTate* getGUIState()
	 * \brief Gibt Zustand der GUI Datenfelder zurueck
	 * \return Zustand der GUI
	 */
	GUIState* getGUIState()
	{
		return &m_gui_state;
	}


	/**
	 * \fn State getState()
	 * \brief Gibt aktuellen Status des Dokuments aus
	 */
	State getState()
	{
		return m_state;
	}

	/**
	 * \fn void setState(State s)
	 * \brief Setzt den Status des Dokuments
	 */
	void setState(State s)
	{
		m_state =s;
	}

	/**
	 * \fn bool getModified()
	 * \brief Gibt aus, ob Daten des Dokumentes geaendert wurden
	 * \return modifiziert
	 */
	int getModified()
	{
		return m_modified;
	}

	/**
	 * \fn void setModified(int mod)
	 * \brief Setzt den Zustand modifiziert
	 * \param mod neuer Zustand
	 */
	void setModified(int mod)
	{
		m_modified = mod;
	}

	/**
	 * \fn map<int, ServerWObject*>* getModifiedObjects()
	 * \brief Gibt Datenstruktur mit allen modifizierten Objekten aus
	 * \return Objekte
	 */
	map<int, ServerWObject*>* getModifiedObjects()
	{
		return m_modified_objects;
	}

	/**
	 * \fn list<Projectile>* getProjectiles()
	 * \brief Gibt die Liste mit den Geschossen aus
	 * \return Liste der Geschosse
	 */
	map<int,Projectile*>* getProjectiles()
	{
		return m_projectiles;
	}
	
	

	
	/**
	 * \fn ServerItem* getDetailedItem()
	 * \brief Gibt Zeiger auf das Item, das aktuell per Tooltip genauer angezeigt wird zurueck
	 */
	ServerItem* getDetailedItem()
	{
		return m_detailed_item;
	}

	
	/**
	 * \fn ServerItem* getDetailedItem()
	 * \brief Gibt Position des Items, das aktuell per Tooltip genauer angezeigt wird zurueck
	 */
	short getDetailedItemPos()
	{
		return m_detailed_item_pos;
	}

	
	/**
	 * \fn Action::ActionType getAbilityPos()
	 * \brief Gibt die Position der Aktion die gerade als Tooltip angezeigt zurueck
	 */
	Action::ActionType getAbilityPos()
	{
		return m_ability_pos;
	}

	/**
	 * \fn ClientMPlayer* getMainPlayer()
	 * \brief Gibt Zeiger auf die Spielerfigur zurueck
	 * \return Spielerobjekt
	 */
	ClientMPlayer* getMainPlayer()
	{
		return m_main_player;
	}

	/**
	 * \fn Party* getParty()
	 * \brief Gibt die Party aus der der Spieler angehoert
	 * \return Party
	 */
	Party* getParty()
	{
		return m_party;
	}

	/**
	 * \fn void setLeftAction(Action::ActionType a)
	 * \brief Setzt die Aktion, die durch die linke Maustaste ausgeloest wird
	 */
	void setLeftAction(Action::ActionType a);

	
	/**
	 * \fn Action::ActionType getLeftAction()
	 * \brief Gibt die Aktion, die durch die linke Maustaste ausgeloest wird, zurueck
	 */
	Action::ActionType getLeftAction()
	{
		return m_left_action;
	}

	/**
	 * \fn void setRightAction(Action::ActionType a)
	 * \brief Setzt die Aktion, die durch die rechte Maustaste ausgeloest wird
	 */
	void setRightAction(Action::ActionType a);

	
	/**
	 * \fn Action::ActionType getRightAction()
	 * \brief Gibt die Aktion, die durch die rechte Maustaste ausgeloest wird, zurueck
	 */
	Action::ActionType getRightAction()
	{
		return m_right_action;
	}

	/**
	 * \fn map<int,DropItem>* getDropItems()
	 * \brief Gibt die Liste aller fuer den Spieler sichtbaren Gegenstaende am Boden zurueck
	 */
	map<int,DropItem>* getDropItems()
	{
		return m_drop_items;
	}


	/**
	 * \fn std:.string getAbilityDescription()
	 * \brief Gibt die Beschreibung der Faehigkeit, ueber der Mauszeiger ist, aus
	 */
	std::string getAbilityDescription();

	/**
	 * \fn KeyCode getMappedKey(ShortkeyDestination sd)
	 * \brief Gibt die Taste zurueck, die das angegebene Ereignis ausloest
	 */
	KeyCode getMappedKey(ShortkeyDestination sd);

	/**
	 * \fn void installShortkey(KeyCode key,ShortkeyDestination dest)
	 * \brief Bindet eine Taste an ein Ereignis
	 * \param key Taste
	 * \param dest Ereignis, das auf die Taste hin ausgeloest werden soll
	 */
	void installShortkey(KeyCode key,ShortkeyDestination dest);

	/**
	 * \fn void lock()
	 * \brief Sperrt das Dokument
	 */
	void lock();

	/**
	 * \fn void unlock()
	 * \brief Entsperrt das Dokument
	 */
	void unlock();



	/**
	 * \fn void onButtonSendMessageClicked ( )
	 * \brief Button zum Absenden einer Nachricht wurde angeklickt
	 */
	void onButtonSendMessageClicked ( );




	/**
	 * \fn void onButtonSaveExitClicked ( )
	 * \brief Button zum Speichern und Beenden des Spiels wurde angeklickt
	 */
	void onButtonSaveExitClicked ( );


	/**
	 * \fn void onButtonInventoryClicked()
	 * \brief Behandelt Klick auf Inventar Button
	 */
	void onButtonInventoryClicked();

	/**
	 * \fn void onButtonCharInfoClicked()
	 * \brief Behandelt Klick auf CharakterInfo Button
	 */
	void onButtonCharInfoClicked();


	/**
	 * \fn void onButtonSkillTreeClicked()
	 * \brief Behandelt Klick auf Skilltree Button
	 */
	void onButtonSkilltreeClicked();

	/**
	 * \fn void onButtonOpenChatClicked()
	 * \brief Behandelt Klick auf Chat oeffnen Button
	 */
	void onButtonOpenChatClicked();

	/**
	 * \fn void onSwapEquip()
	 * \brief Wechselt die Ausruestung
	 */
	void onSwapEquip();

	/**
	 * \fn void onLeftMouseButtonClick (float x, float y)
	 * \brief Behandelt die Reaktion auf einen Linksklick ins Spielfeld
	 * \param x X-Koordinate
	 * \param y Y-Koordinate
	 */
	void onLeftMouseButtonClick(float x, float y);


	/**
	 * \fn void onRightMouseButtonClick (float x, float y)
	 * \brief Behandelt die Reaktion auf einen Rechtsklick ins Spielfeld
	 * \param x X-Koordinate
	 * \param y Y-Koordinate
	 */
	void onRightMouseButtonClick(float x, float y);


	/**
	 * \fn void onItemLeftClick(short pos)
	 * \brief Behandelt Linksklick auf ein Item
	 */
	void onItemLeftClick(short pos);

	/**
	 * \fn void onItemRightClick(short pos)
	 * \brief Behandelt Rechtsklick auf ein Item
	 */
	void onItemRightClick(short pos);

	/**
	 * \fn void requestItemDetailedInfo( short pos)
	 * \brief Fordert vom Server weitere Informationen zu einem Item an
	 */
	void requestItemDetailedInfo( short pos);

	/**
	 * \fn void requestAbilityDamage(Action::ActionType abl)
	 * \brief Fordert vom Server den Schaden einer Faehigkeit an
	 */
	void requestAbilityDamage(Action::ActionType abl);

	/**
	 * \fn void increaseAttribute(CreatureBaseAttr::Attribute attr)
	 * \brief Erhoeht das angegebene Attribut
	 */
	void increaseAttribute(CreatureBaseAttr::Attribute attr);

	/**
	 * \fn void learnAbility(Action::ActionType act)
	 * \brief Lernt die angegebene Faehigkeit
	 */
	void learnAbility(Action::ActionType act);

	/**
	 * \fn void dropCursorItem()
	 * \brief Laesst das Item fallen, das aktuell am Cursor haengt
	 */
	void dropCursorItem();

	/**
	 * \fn bool onKeyPress(KeyCode key)
	 * \brief Behandelt einen Tastendruck
	 * \param key Keycode der Taste
	 */
	bool onKeyPress(KeyCode key);

	/**
	 * \fn bool onKeyRelease(KeyCode key)
	 * \brief Behandelt loslassen einer Taste
	 */
	bool onKeyRelease(KeyCode key);

	void onButtonPartyApply(int pnr);

	void onButtonPartyAccept(int cnr);

	void emitDebugSignal(int i=0);

	/**
	 * \fn void update( )
	 * \brief Aktualisiert den Inhalt der GUI, holt alle Daten neu vom Netzwerk
	 */
	void update(float time=0);

	/**
	 * \fn void serverConnect()
	 * \brief Baut Verbindung zum Server auf
	 */
	void serverConnect();


	// Private stuff
	private:
	//Fields
		/**
	 * \fn void sendCommand(ClientCommand* command)
	 * \param command Kommando, das gesendet werden soll
	 * \return Status des Netzwerkes nach der Sendeanfrage
	 * \brief sendet ein Kommando an den Server
		 **/
		void sendCommand(ClientCommand* comm);

		/**
		 * \fn void sendSavegame()
		 * \brief Sendet ein Savegame zum Server
		 */
		void sendSavegame();
		
		/**
		 * \fn void updateContent(float time)
		 * \brief Aktualisiert den Inhalt des Dokuments
		 */
		void updateContent(float time);

		/**
		 * \fn void handleDatapkg(CharConv* cv)
		 * \param datap Zeiger auf das Datenpaket
		 * \param headerp Zeiger auf den Header des Datenpaketes
		 * \brief Behandlung von Datenpaketen
	 	*/
		void handleDataPkg(CharConv* cv,ServerHeader* headerp);


		/**
		 * \fn void handleSavegame(CharConv* cv)
		 * \brief Behandlung von Savegamepaketen
		 * \param datap Zeiger auf die Daten
	 	*/
		void handleSavegame(CharConv* cv);

		/**
		 * \fn void handleDetailedItem(CharConv* cv)
		 * \brief Behandlung von Informationen zu einem Item
		 * \param datap Zeiger auf die Daten
	 	*/
		void handleDetailedItem(CharConv* cv);

		/**
		 * \fn void handleRegionData(CharConv* cv)
		 * \brief Behandlung der Daten einer Region
	 	 */
		void handleRegionData(CharConv* cv);

	/**
		 * \fn void handleDatapkg(CharConv* cv)
	 * \param datap Zeiger auf das Datenpaket
	 * \param headerp Zeiger auf den Header des Datenpaketes
	 * \brief Behandlung von informationen zum Schaden einer Aktion
	 */
		void handleAbilityDamage(CharConv* cv, ServerHeader* headerp);

	/**
	 * \fn int getObjectAt(float x,float y)
	 * \brief Ermittelt die ID des Objektes an den angegebenen Koordinaten
	 * \return ID
	 */
	int getObjectAt(float x,float y);

	/** \var NetworkInfo m_network_info
	 * \brief Enthaelt Daten zum Netzwerk
	 */
		NetworkInfo m_network_info;

	/**
	 * \var GUIState m_gui_state
	 * \brief Enthaelt alle Informationen zum Zustand der GUI
	 */
		GUIState m_gui_state;


	/**
	 * \var  m_modified_objects
	 * \brief Speichert die an den Client &uuml;bertragenen Objekte in einem Bin&auml;rbaum
	 */
	map<int, ServerWObject*>* m_modified_objects;

	/**
	 * \var list<int> m_removed_objects
	 * \brief Liste mit den IDs geloeschter Objekte
	 */
	list<int> m_removed_objects;
	
	/**
	 * \var list<int> m_removed_projectiles
	 * \brief Liste mit den IDs geloeschter Objekte Projektile
	 */
	list<int> m_removed_projectiles;

	/**
	 * \var RegionData m_region_data
	 * \brief Statische Daten zur aktuellen Region
	 */
	RegionData m_region_data;

	/**
	 * \var m_main_player
	 * \brief Zeiger auf das Spielerobjekt das der Nutzer kontrolliert
	 */
		ClientMPlayer* m_main_player;

	/**
	 * \var Party* m_party
	 * \brief Zeiger auf die Party der der Spieler angehoert
	 */
		Party* m_party;

	/**
	 * \var Action::ActionType m_left_action
	 * \brief Aktion die auf linke Maustaste ausgefuehrt wird
	 */
		Action::ActionType m_left_action;

	/**
	 * \var Action::ActionType m_right_action
	 * \brief Aktion die auf rechte Maustaste ausgefuehrt wird
	 */
		Action::ActionType m_right_action;

	/**
	 * \var map<int,Projectile*> m_projectiles
	 * \brief Liste der Projektil
	 */
	map<int,Projectile*>* m_projectiles;

	/**
	 * \var map<int,DropItem>* m_drop_items;
	 * \brief Liste der Items am Boden
	 */
	map<int,DropItem>* m_drop_items;

	/**
	 * \var ServerItem* m_detailed_item
	 * \brief Item mit detaillierten Informationen
	 */
		ServerItem* m_detailed_item;

	/**
	 * \var short m_detailed_item_pos
	 * \brief Position des Items mit detaillierten Informationen
	 */
		short m_detailed_item_pos;

	/**
	 * \var Damage m_ability_damage
	 * \brief Schaden der Faehigkeit auf die der Mauszeiger zeigt
	 */
		Damage m_ability_damage;

	/**
	 * \var short m_ability_pos
	 * \brief Faehigkeit auf die der Mauszeiger zeigt
	 */
		Action::ActionType m_ability_pos;

	/**
	 * \var m_data_locks
	 * \brief Gibt die Anzahl der Locks auf den Daten des Dokuments an. Die Daten koennen nur veraendert und von aussen gelesen werden, wenn die Anzahl gleich 0 ist
	 */
		int m_data_locks;

	/**
	 * \var char* m_savegame
	 * \brief Aktueller Spielstand in Binaerformat
	 */
		char* m_savegame;


	/**
	 * \var int m_modified
	 * \brief Bitmaske welche angibt welche Teile des Dokuments veraendert wurden
	 */
		int m_modified;

	/**
	 * \var string m_save_file
	 * \brief Dateiname des Savefiles
	 */
		string m_save_file;

	/**
	 * \var State m_state
	 * \brief Status des Dokuments
	 */
		State m_state;

	/**
	 * \var map<unsigned int,ShortkeyDestination> m_shortkey_map
	 * \brief Bildet Taste auf Ereignis, das per Shortkey ausgeloest werden kann ab
	 */
		map<KeyCode,ShortkeyDestination> m_shortkey_map;

	/**
	 * \var set<KeyCode> m_special_keys
	 * \brief Menge der Tasten, die eine besondere Bedeutung haben und die deswegen nicht frei zugewiesen werden koennen
	 */
		set<KeyCode> m_special_keys;


	/**
	* \var float m_save_timer
	* \brief Timer, der der die Zeit zaehlt, die auf ein Savegame gewartet wird
	 */
		float m_save_timer;


};


#endif //DOCUMENT_H



