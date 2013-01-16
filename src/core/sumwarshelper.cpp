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

#include "sumwarshelper.h"
#include "config.h"

#if defined (_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#elif (__APPLE__)
#include <CoreFoundation/CFBundle.h>
#elif (__unix__)
#include <X11/Xlib.h>
#include <X11/extensions/Xrandr.h>
#endif

// Instance of the singleton.
template<> SumwarsHelper* Ogre::Singleton<SumwarsHelper>::SUMWARS_OGRE_SINGLETON = 0;

/**
 * Constructor; will call the init function.
 */
SumwarsHelper::SumwarsHelper()
{
}


/**
* \brief Initialize the helper; this will also reload the list of supported features.
*/
bool SumwarsHelper::init()
{
	supportedFeatures_.clear ();

#if defined (_WIN32)
	supportedFeatures_.push_back ("non-exclusive-fullscreen"); // aka windowed (fullscreen) mode.

	// Feature to allow some settings to be pre-loaded from a different config file:
	// see getPreloadConfigFile.
	supportedFeatures_.push_back ("allows-preload");
#elif defined (__unix__)
#elif defined (__APPLE__)
#endif

    std::string storagePath (SumwarsHelper::getSingletonPtr ()->getStorageBasePath ());

    // Add the user directory
    if (PHYSFS_addToSearchPath 	 (storagePath.c_str (), 1) == 0)
    {
        printf("PHYSFS_addToSearchPath 	 failed: %s\n", PHYSFS_getLastError ());
        return false;
    }

    // Set the user directory as the default location to write to.
    if (PHYSFS_setWriteDir (storagePath.c_str ()) == 0)
    {
        printf("PHYSFS_setWriteDir failed: %s\n", PHYSFS_getLastError ());
        return false;
    }

#ifdef __APPLE__
    if (!PHYSFS_exists("Library/Application Support/Sumwars"))
    {
        if (PHYSFS_mkdir("Library/Application Support/Sumwars") == 0)
        {
            printf("mkdir failed: %s\n", PHYSFS_getLastError());
            return false;
        }
        if (PHYSFS_mkdir("Library/Application Support/Sumwars/save") == 0)
        {
            printf("mkdir failed: %s\n", PHYSFS_getLastError());
            return false;
        }
    }
    if (!PHYSFS_exists("Library/Application Support/Sumwars/save"))
    {
        if (PHYSFS_mkdir("Library/Application Support/Sumwars/save") == 0)
        {
            printf("mkdir failed: %s\n", PHYSFS_getLastError());
            return false;
        }
    }
#else
    std::string ogreCfgUser = ".sumwars/ogre.cfg";
    std::string pluginsCfgUser = ".sumwars/plugins.cfg";
    std::string resourcesCfgUser = ".sumwars/resources.cfg";

    /*
    We have to have something in the default ogre.cfg file, otherwise the options window will
    fail to update it properly. We try hard to make it proper defaults suitable for the system though.
    */
    const std::string defaultOgreCfg =
        "Render System=OpenGL Rendering Subsystem\n" // FIXME: Do we want DirectX9 as default renderer on Win?
        "\n"
        "[OpenGL Rendering Subsystem]\n"
        "FSAA=0\n"
#ifdef _DEBUG // Do not start in fullscreen in debug mode as default
        "Full Screen=No\n"
#else
        "Full Screen=Yes\n"
#endif
        "RTT Preferred Mode=FBO\n"
        "Video Mode=" + SumwarsHelper::getNativeResolutionString() + "\n\n"

        "[Direct3D9 Rendering Subsystem]\n"
        "Allow NVPerfHUD=No\n"
        "FSAA=0\n"
        "Floating-point mode=Fastest\n"
        "Full Screen=Yes\n"
        "Resource Creation Policy=Create on all devices\n"
        "VSync=No\n"
        "Video Mode=" + SumwarsHelper::getNativeResolutionString() + SumwarsHelper::getNativeColourDepthString() + "\n";


    const std::string defaultPluginsCfg =
        "# This file's template has been autogenerated using cmake"
        "# (see src/gui/application.cpp and src/gui/config.h.in)\n"
        "# Feel free to change the contents!\n"
        "# If you break this file and sumwars no longer starts, just remove it and it will be\n"
        "# created again with default values."
        "\n"
        "# Define plugin folder\n"
        "PluginFolder=" SUMWARS_OGRE_PLUGIN_DIR "\n"
        "\n"
        "Plugin=RenderSystem_GL" SUMWARS_DEBUG_POSTFIX "\n"
#ifdef _WIN32
        "Plugin=RenderSystem_Direct3D9" SUMWARS_DEBUG_POSTFIX "\n"
#endif
        "Plugin=Plugin_ParticleFX" SUMWARS_DEBUG_POSTFIX "\n"
        "Plugin=Plugin_OctreeSceneManager" SUMWARS_DEBUG_POSTFIX "\n";

#include "default_resources.cfg.inc"

    // creating 'user' directory
    if (!PHYSFS_exists(SumwarsHelper::userPath().c_str()))
    {
        printf("User dir doesn't exist, making it: %s%s\n", PHYSFS_getWriteDir(), SumwarsHelper::userPath().c_str());
        int result = PHYSFS_mkdir(SumwarsHelper::userPath().c_str());
        if (result == 0)
        {
            printf("PHYSFS_mkdir failed: %s\n", PHYSFS_getLastError());
            return false;
        }
    }

    // creating 'save' directory
    if (!PHYSFS_exists(SumwarsHelper::savePath().c_str()))
    {
        printf("Save dir doesn't exist, making it: %s%s\n", PHYSFS_getWriteDir(), SumwarsHelper::savePath().c_str());
        int result = PHYSFS_mkdir(SumwarsHelper::savePath().c_str());
        if (result == 0)
        {
            printf("PHYSFS_mkdir failed: %s\n", PHYSFS_getLastError());
            return false;
        }
    }

    if (!PHYSFS_exists(ogreCfgUser.c_str()))
    {
        PHYSFS_file* ogreFile = PHYSFS_openWrite(ogreCfgUser.c_str());
        int count = PHYSFS_write(ogreFile,
                     defaultOgreCfg.c_str(), sizeof(char), defaultOgreCfg.size());

        if (count < defaultOgreCfg.size())
        {
            printf("Attempting to write a local Ogre configuration file failed: PHYSFS_write('%s') failed: %s\n",
                 ogreCfgUser.c_str(),
                 PHYSFS_getLastError());
            return false;
        }
        else
        {
            printf("Created '%s%s'\n", PHYSFS_getWriteDir(), ogreCfgUser.c_str());
        }
    }

    if (!PHYSFS_exists(pluginsCfgUser.c_str()))
    {
        PHYSFS_file* pluginsFile = PHYSFS_openWrite(pluginsCfgUser.c_str());
        int count = PHYSFS_write(pluginsFile,
                     defaultPluginsCfg.c_str(), sizeof(char), defaultPluginsCfg.size());

        if (count < defaultPluginsCfg.size())
        {
            printf("Attempting to write local Ogre plugins file failed: PHYSFS_write('%s') failed: %s\n",
                 pluginsCfgUser.c_str(),
                 PHYSFS_getLastError());
            return false;
        }
        else
        {
            printf("Created '%s%s'\n", PHYSFS_getWriteDir(), pluginsCfgUser.c_str());
        }
    }

    if (!PHYSFS_exists(resourcesCfgUser.c_str()))
    {
        PHYSFS_file* resourcesFile = PHYSFS_openWrite(resourcesCfgUser.c_str());
        int count = PHYSFS_write(resourcesFile,
                     defaultResourcesCfg.c_str(), sizeof(char), defaultResourcesCfg.size());

        if (count < defaultResourcesCfg.size())
        {
            printf("Attempting to write local resources config file failed: PHYSFS_write('%s') failed: %s\n",
                 resourcesCfgUser.c_str(),
                 PHYSFS_getLastError());
            return false;
        }
        else
        {
            printf("Created '%s%s'\n", PHYSFS_getWriteDir(), resourcesCfgUser.c_str());
        }
    }
#endif
}



/**
 * Search in the list of stored features for a specific one.
 */
bool SumwarsHelper::hasFeature (const std::string& featureName) const
{
	for (std::vector <std::string>::const_iterator it = supportedFeatures_.begin ();
		it != supportedFeatures_.end (); ++ it)
	{
		if (*it == featureName)
		{
			return true;
		}
	}
	return false;
}


/**
 * \brief Retrieve the preload file name. Should not be used if the preload feature is not available.
 */
std::string SumwarsHelper::getPreloadFileName () const
{
	// TODO: investigate whether an OS specific file name should be used
	return std::string ("swpreload.cfg");
}

/**
 * \brief Get the path to use for storage.
 */
const std::string& SumwarsHelper::getStorageBasePath()
{
#ifdef SUMWARS_PORTABLE_MODE
	static std::string ret(PHYSFS_getBaseDir());
#else
	static std::string ret(PHYSFS_getUserDir());
#endif

	return ret;
}

Ogre::String SumwarsHelper::userPath ()
{
	//Ogre::String path = PHYSFS_getUserDir ();
	Ogre::String path = "";
	
#if defined (_WIN32)
	path.append(".sumwars");
#elif defined (__unix__)
	path.append(".sumwars");
#elif defined (__APPLE__)
	path.append("/Library/Application Support/Sumwars");
#endif

	return path;
}

Ogre::String SumwarsHelper::savePath ()
{
	return userPath() + "/save";
}

std::string SumwarsHelper::getUpdatedResolutionString (const std::string& initialString, int newWidth, int newHeight)
{
	std::string returnValue;
	returnValue = initialString;

	//std::string sLeft; 
	std::string sRight;
	int nPos = returnValue.find (" ");
	if (nPos == std::string::npos)
	{
		// some error...
		return "";
	}

	//sLeft = returnValue.substr (0, nPos);
	sRight= returnValue.substr (nPos + 3); // + 3 chars for " x "
	nPos = sRight.find (" ");
	//std::string sAux;

	if (nPos == std::string::npos)
	{
		// we don't have a colour depth.
		//sAux = sRight;
		sRight = "";
	}
	else
	{
		//sAux = sRight.substr(0, nPos);
		sRight = sRight.substr(nPos);
	}

	// Use a string stream to parse the needed string.
	std::stringstream ss;
	ss << newWidth << " x " << newHeight << sRight;
	returnValue = ss.str ();

	return returnValue;
}

std::string SumwarsHelper::getNativeResolutionString()
{
	int xRes = 800;
	int yRes = 600;

#if defined (_WIN32)
	xRes = GetSystemMetrics(SM_CXSCREEN);
	yRes = GetSystemMetrics(SM_CYSCREEN);
#elif defined (__unix__)
	int num_sizes;
	Rotation original_rotation;

	Display *dpy = XOpenDisplay(NULL);
	Window root = RootWindow(dpy, 0);
	XRRScreenSize *xrrs = XRRSizes(dpy, 0, &num_sizes);

	XRRScreenConfiguration *conf = XRRGetScreenInfo(dpy, root);
	//short original_rate          = XRRConfigCurrentRate(conf);
	SizeID original_size_id      = XRRConfigCurrentConfiguration(conf, &original_rotation);

	xRes = xrrs[original_size_id].width;
	yRes = xrrs[original_size_id].height;
	XCloseDisplay(dpy);
#elif defined (__APPLE__)
	xRes = CGDisplayPixelsWide;
	yRes = CGDisplayPixelsHigh;
#endif

	std::stringstream ss;
	ss << xRes << " x " << yRes;
	return ss.str ();
}

std::string SumwarsHelper::getNativeColourDepthString()
{
#if defined (_WIN32)
	int bitsPerPixel = 32;
	HDC dc = GetDC(NULL);
	bitsPerPixel = GetDeviceCaps(dc, BITSPIXEL);
	ReleaseDC(NULL, dc);

	std::stringstream stream;
	stream << " @ " << bitsPerPixel << "-bit colour";

	return stream.str();
#endif
	return "";
}

void SumwarsHelper::getSizesFromResolutionString (const std::string& initialString, int& videoModeWidth, int& videoModeHeight)
{
	std::string tempString;
	tempString = initialString;

	std::string sLeft, sRight;
	int nPos = tempString.find (" ");
	if (nPos == std::string::npos)
	{
		// some error...
		return;
	}

	sLeft = tempString.substr (0, nPos);
	sRight= tempString.substr (nPos + 3); // + 3 chars for " x "
	nPos = sRight.find (" ");
	std::string sAux;

	if (nPos == std::string::npos)
	{
		// we don't have a colour depth.
		sAux = sRight;
		sRight = "";
	}
	else
	{
		sAux = sRight.substr(0, nPos);
		sRight = sRight.substr(nPos);
	}

	videoModeWidth = atoi (sLeft.c_str ());
	videoModeHeight = atoi (sAux.c_str ());
}

#if defined (__APPLE__)
Ogre::String SumwarsHelper::macPath()
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
