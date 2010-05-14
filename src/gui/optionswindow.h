#ifndef OPTIONSWINDOW_H
#define OPTIONSWINDOW_H

#include "window.h"
#include "OISKeyboard.h"

/**
 * \class OptionsWindow
 * \brief Fenster Optionen
 */
class OptionsWindow : public Window
{
	public:
	/**
	 * \fn OptionsWindow (Document* doc, OIS::Keyboard *keyboard)
	 * \param keyboard OIS keyboard
	 * \brief Konstruktor
	 */
		OptionsWindow (Document* doc, OIS::Keyboard *keyboard);
	
		/**
		 * \fn virtual void update()
		 * \brief aktualisiert den Inhalt des Fensters
		 */
		virtual void update();
		
		/**
		 * \fn virtual void updateTranslation
		 * \brief aktualisiert die Uebersetzungen
		 */
		virtual void updateTranslation();
		
		
		/**
		 * \fn bool requestsForKey()
		 * \brief Gibt an, ob das Fenster aktuell auf einen Tastendruck wartet
		 */
		bool requestsForKey()
		{
			return m_key_destination != NO_KEY;
		}
		
		/**
		 * \fn setzt den Keycode fuer das per Klick ausgewaehlte Label
		 * \param kc KeyCode
		 */
		void setKeyCode(KeyCode kc);
		
		/**
		 * \fn void reset()
		 * \brief Setzt das Fenster zurueck
		 */
		void reset();
		
	private:
		
		/**
		* \brief Aktion fuer die aktuell ein Shortkey erwartet wird
		 */
		ShortkeyDestination m_key_destination;
		
		/**
		 * \fn bool onShortkeyLabelClicked(const CEGUI::EventArgs& evt)
		 * \brief Behandelt Klick auf ein Shortkey Label
		 */
		bool onShortkeyLabelClicked(const CEGUI::EventArgs& evt);
		
		/**
		 * \fn bool onShortkeyLabelClicked(const CEGUI::EventArgs& evt)
		 * \brief Behandelt Klick auf Button Ok
		 */
		bool onButtonOkClicked(const CEGUI::EventArgs& evt);
		
		/**
		 * \fn bool onAreaMouseButtonPressed(const CEGUI::EventArgs& evt);
		 * \brief Behandelt Druecken der linken Maustaste ueber auf dem Fenster
		 */
		bool onAreaMouseButtonPressed(const CEGUI::EventArgs& evt);
		
		/**
		 * \fn bool onSoundVolumeChanged(const CEGUI::EventArgs& evt);
		 * \brief Behandelt Aendern der Soundlautstarke
		 */
		bool onSoundVolumeChanged(const CEGUI::EventArgs& evt);
		
		/**
		 * \brief Behandelt Aendern der Musiklautstarke
		 */
		bool onMusicVolumeChanged(const CEGUI::EventArgs& evt);
		
		/**
		 * \fn bool onLanguageSelected(const CEGUI::EventArgs& evt)
		 * \brief Behandelt Auswahl einer neuen Sprache
		 */
		bool onLanguageSelected(const CEGUI::EventArgs& evt);
		
		/**
		 * \var OIS::Keyboard *m_keyboard
		 * \brief Repraesentation der Tastatur
		 */
		OIS::Keyboard *m_keyboard;
	
};

#endif

