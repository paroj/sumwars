#include "filebrowser.h"
#include <dirent.h>
#include <list>

using namespace CEGUI;

FileBrowser::~FileBrowser()
{
	m_winManager->destroyWindow(m_rootWindow);
}

void FileBrowser::init(CEGUI::String defaultDir, bool visible)
{
	m_guiSystem = System::getSingletonPtr();
	m_winManager = WindowManager::getSingletonPtr();
	m_gameScreen = m_winManager->getWindow("GameScreen");

	m_rootWindow = m_winManager->loadWindowLayout("FileBrowser.layout");
	m_gameScreen->addChildWindow(m_rootWindow);
	m_rootWindow->setVisible(visible);

	m_rootWindow->subscribeEvent(CEGUI::FrameWindow::EventCloseClicked, Event::Subscriber(&FileBrowser::handleCloseWindow, this));

	m_pathBox = static_cast<Editbox*>(m_rootWindow->getChild("CurrentPath"));
	m_pathBox->setText(defaultDir);

	m_browserBox = static_cast<ItemListbox*>(m_rootWindow->getChild("Browser"));

	fillBrowser(defaultDir);

}

void FileBrowser::fillBrowser(CEGUI::String inDir)
{
	m_browserBox->resetList();

	DIR *dir;
	struct dirent *ent;

	CEGUI::ItemEntry *item;

	std::list<std::string> dirs;
	std::list<std::string> files;
	std::list<std::string>::iterator it;

	/* open directory stream */
	dir = opendir (inDir.c_str());
	if (dir != NULL)
	{

		/* print all the files and directories within directory */
		while ((ent = readdir (dir)) != NULL)
		{
			switch (ent->d_type)
			{
			case DT_REG:
				files.push_back(ent->d_name);
				//printf ("%*.*s\n", ent->d_namlen, ent->d_namlen, ent->d_name);
				break;

			case DT_DIR:
				dirs.push_back(ent->d_name);
				//printf ("%s (dir)\n", ent->d_name);
				break;

			default:
				break;
				//printf ("%s:\n", ent->d_name);
			}
		}

		closedir (dir);
	}

	dirs.sort();
	files.sort();

	for (it=dirs.begin(); it!=dirs.end(); ++it)
	{
		if(*it != ".")
		{
			dirs.insert(dirs.begin(), ".");
			dirs.insert(dirs.begin(), "..");
		}

		item = (CEGUI::ItemEntry*)m_winManager->createWindow("TaharezLook/ListboxItem");
		item->setText(*it);
		item->subscribeEvent(CEGUI::ItemEntry::EventMouseDoubleClick, CEGUI::Event::Subscriber(&FileBrowser::handleBrowserDblClick, this));
		m_browserBox->addItem(item);
	}
	for (it=files.begin(); it!=files.end(); ++it)
	{
		item = (CEGUI::ItemEntry*)m_winManager->createWindow("TaharezLook/ListboxItem");
		item->setText(*it);
		item->subscribeEvent(CEGUI::ItemEntry::EventMouseDoubleClick, CEGUI::Event::Subscriber(&FileBrowser::handleBrowserDblClick, this));
		m_browserBox->addItem(item);
	}
}

bool FileBrowser::handleBrowserDblClick(const CEGUI::EventArgs &e)
{
	CEGUI::String newDir;
	CEGUI::String selectedDir = m_browserBox->getFirstSelectedItem()->getText();
	CEGUI::String oldDir = m_pathBox->getText();

#ifdef WIN32
	CEGUI::String dirDelemiter = "\\";
#else
	CEGUI::String dirDelemiter = "/";
#endif

	if(selectedDir == "..")
	{
		if(oldDir.find_last_of(dirDelemiter) == oldDir.size())
			oldDir = oldDir.erase(oldDir.size()-1, CEGUI::String::npos);

		int pos = oldDir.find_last_of(dirDelemiter);
		newDir = oldDir.erase(pos, oldDir.size()- pos);
	}
	else
		if(oldDir.find_last_of(dirDelemiter) == oldDir.size()-1)
			newDir = oldDir + selectedDir;
		else
			newDir = oldDir +  dirDelemiter + selectedDir;

	m_pathBox->setText(newDir);

	fillBrowser(newDir + dirDelemiter);

	return true;
}

bool FileBrowser::handleCloseWindow(const CEGUI::EventArgs& e)
{
	m_rootWindow->hide();
	return true;
}

