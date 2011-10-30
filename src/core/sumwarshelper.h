/*
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef SUMWARSHELPER_H
#define SUMWARSHELPER_H

#include "config.h"

#include <physfs.h>
#include <OgreString.h>

#if defined (__APPLE__)
#include <CoreFoundation/CFBundle.h>
#endif


/**
 * \brief Helper class. Used as a namespace to store utility static functions.
 */
class SumwarsHelper
{
public:
	/**
	 * \fn static Ogre::String gameDataPath()
	 *
	 * \brief Returns the path where the game data is stored (e.g.,
	 * /usr/share/games/sumwars in Unix)
	 */
	static Ogre::String gameDataPath()
	{
		Ogre::String path = userPath();

#if defined (__unix__)
		// redefine if necessary
		path = CFG_FILES_DIR;
#elif defined (__APPLE__)
		// redefine if necessary
#elif defined (_WIN32)
		// redefine if necessary
#endif

		return path;
	}


	/**
	 * \fn static Ogre::String userPath()
	 * \brief Returns the writable sumwars directory in the users directory
	 */
	static Ogre::String userPath()
	{
		Ogre::String path = PHYSFS_getUserDir();
#ifdef __APPLE__
		path.append("/Library/Application Support/Sumwars");
#else
		path.append(".sumwars");
#endif
		return path;
	}


	/**
	 * \brief Gets the updated resolution string from a base resolution string.
	 * Purpose: manage differences between Direct3D and OpenGL resolution strings.
	 *
	 * \param initialString The source resolution string.
	 * \param newWidth The new width to use.
	 * \param newHeight The new height to use.
	 * \return The full updated name of the entry for the resolution.
	 * \author Augustin Preda (if anything doesn't work, he's the one to bash).
	 */
	static std::string getUpdatedResolutionString (const std::string& initialString, int newWidth, int newHeight);

	/**
	 * \brief Parse a basic resolution string and get the width and height.
	 * Purpose: manage differences between Direct3D and OpenGL resolution strings.
	 *
	 * \param initialString The source resolution string.
	 * \param[out] newWidth The width to read.
	 * \param[out] newHeight The height to read.
	 * \return Nothing.
	 * \author Augustin Preda (if anything doesn't work, he's the one to bash).
	 */
	static void getSizesFromResolutionString (const std::string& initialString, int& videoModeWidth, int& videoModeHeight);

#if defined (__APPLE__)
	// TODO: copied function here from application.h; Investigate integration with userPath function.
	// TODO: investigate removal of original function from application.h

	/**
	 * \fn Ogre::String macPath()
	 * \brief Returns the path to the Resources directory on mac.
	 */
	Ogre::String macPath()
	{
		Ogre::String path;
		CFBundleRef mainBundle = CFBundleGetMainBundle();
		CFURLRef resourcesURL = CFBundleCopyResourcesDirectoryURL(mainBundle);
		char resPath[PATH_MAX];
		CFURLGetFileSystemRepresentation(resourcesURL, TRUE, (UInt8 *)resPath, PATH_MAX);
		CFRelease(resourcesURL);
		path = resPath;
		return path;
	}
#endif

};
#endif // SUMWARSHELPER_H
