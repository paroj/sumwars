#ifndef GRAPHIC_MANAGER_H
#define GRAPHIC_MANAGER_H


#include "OgreSceneManager.h"
#include "OgreRoot.h"
#include "OgreParticleSystem.h"
#include "OgreEntity.h"
#include "OgreMovableObject.h"
#include "graphicobject.h"
#include "graphicobjectrenderinfo.h"

#include "../tinyxml/tinyxml.h"
#include <string>
#include <list>
#include <map>

#include <OgreRenderQueueListener.h> 
#include <OgreRenderSystem.h>
#include <OgreRenderQueue.h>
#include <OgreRenderQueueListener.h> 

#ifndef RENDER_QUEUE_MAIN
#define RENDER_QUEUE_MAIN 50
#endif

#define RENDER_QUEUE_HIGHLIGHT_MASK	RENDER_QUEUE_MAIN + 1
#define RENDER_QUEUE_HIGHLIGHT_OBJECTS	RENDER_QUEUE_MAIN + 2
#define LAST_STENCIL_OP_RENDER_QUEUE	RENDER_QUEUE_HIGHLIGHT_OBJECTS

#define STENCIL_VALUE_FOR_OUTLINE_GLOW 1
#define STENCIL_VALUE_FOR_FULL_GLOW 2
#define STENCIL_FULL_MASK 0xFFFFFFFF

class StencilOpQueueListener : public Ogre::RenderQueueListener 
{ 
	public: 
		virtual void renderQueueStarted(Ogre::uint8 queueGroupId, const Ogre::String& invocation, bool& skipThisInvocation);
		

		virtual void renderQueueEnded(Ogre::uint8 queueGroupId, const Ogre::String& invocation, bool& repeatThisInvocation);

}; 

/**
 * \brief Graphic object creation and destruction class
 * Class that manages creation and destruction of Graphicobject and Ogre::Movableobject objects. 
 */
class GraphicManager
{
	public:
		/**
		 * \brief initialises the object
		 */
		static void init();
	
		/**
		 * \brief Frees memory of internal data structures
		 */
		static void cleanup();
	
		/**
		 * \brief Reads the given XML file and stores the contained Renderinfo data
		 * \param pFilename Renderinfo XML file name
		 */
		static void loadRenderInfoData(const char* pFilename);
		
		/**
		 * \brief Creates a new Graphicobject
		 * \param type Name of the Renderinfo used for this object
		 * \param name Name of the Object
		 * \param id ID of the created Object. Usually matches the ID of the underlying Gameobject
		 * \return  pointer to the new object. Returns NULL if creation was not succesful.
		 * \sa destroyGraphicObject
		 */
		static GraphicObject* createGraphicObject(GraphicObject::Type type, std::string name, int id);
		
		/**
		 * \brief Destroys a GraphicObject
		 * \param obj pointer to the object
		 * Currently this just calls delete on the pointer, put some caching might be done in future
		 * \sa createGraphic
		 */
		static void destroyGraphicObject(GraphicObject* obj);
		
		/**
		 * \brief Returns the Scenemanager that is used to create new Ogre Objects
		 * \sa setSceneManager
		 */
		static Ogre::SceneManager* getSceneManager()
		{
			return m_scene_manager;
		}
		
		/**
		 * \brief Sets the Scenemanager that is used to create new Ogre Objects
		 * \param scene_manager OGRE SceneManager
		 * \sa getSceneManager()
		 */
		static void setSceneManager(Ogre::SceneManager* scene_manager)
		{
			m_scene_manager = scene_manager;
		}
		
		/**
		 * \brief Creates a new OGRE MovableObject based on the information supplied
		 * \param info Structure containing all required information on the new Object
		 * \param name Unique name of the new object
		 * It is not guaranteed that the object is really newly created. For performance reasons deleted objects may be cached and reused. It is guaranteed that the object return is can not be distinguished from a new object.
		 * \sa destroyMovableObject
		 */
		static Ogre::MovableObject* createMovableObject(MovableObjectInfo& info, std::string name);
		
		/**
		 * \brief Destroys a MovableObject
		 * \param obj pointer to the object
		 * For performance reasons the object might be cached instead of being destroyed and returned on the next create call.
		 * \sa createMovableObject
		 */
		static void destroyMovableObject(Ogre::MovableObject* obj);
		
		/**
		 * \brief Detaches a MovableObject from its parent node
		 * \param obj pointer to the object
		 */
		static void detachMovableObject(Ogre::MovableObject* obj);
		
		/**
		 * \brief Registers a mapping that describes which renderinfo is used to visualize an ingame object
		 * \param objecttype Ingame Object type
		 * \param renderinfo Renderinfo used to display objects of type \a objecttype
		 * \sa getGraphicType
		 */
		static void registerGraphicMapping(std::string objecttype, GraphicObject::Type renderinfo);
		
		/**
		 * \brief Returns the Renderinfo that is used to display the given objecttype
		 * \param objecttype Ingame object type
		 * \return Renderinfo used to display objects of type \a objecttype
		 * Returns an empty string if no data was found
		 * \sa registerGraphicMapping
		 */
		static GraphicObject::Type getGraphicType(std::string objecttype);
		
		/**
		 * \brief Returns the RenderInfo data structure that corresponds to the supplied renderinfo name
		 * \param renderinfoname Name of a Renderinfo
		 * \param returns pointer to RenderInfo structure, NULL if no data was found
		 * \sa  registerRenderInfo
		 */
		static GraphicRenderInfo* getRenderInfo(std::string renderinfoname);
		
		/**
		 * \brief clears the internal particle pool
		 */
		static void clearParticlePool();
		
	private:
		/**
		 * \brief Registers a Renderinfo data structure with the given name
		 * \param name Name of a Renderinfo
		 * \param info corresponding data structure
		 * The data can be retrieved by a getRenderInfo call
		 * \sa getRenderInfo
		 */
		static void registerRenderInfo(std::string name, GraphicRenderInfo* info)
		{
			if (m_render_infos.count(name) ==0)
			{
				m_render_infos[name] = info;
			}
		}
		
		/**
		 * \brief Returns an OGRE Particlesystem with the requested type
		 * \param type Type of a Particlesystem
		 * The particle system is taken from the particle pool. A new particle system is only created if there is none of the requested type in the pool
		 */
		static Ogre::ParticleSystem* getParticleSystem(std::string type);
	
		/**
		 * \brief Inserts the particle system into the particle pool
		 * \param part particle system
	 	*/
		static void putBackParticleSystem(Ogre::ParticleSystem* part);
		
		/**
		 * Data structure containg all Renderinfo data. Maps name of Renderinfos to the real data
		 */
		static std::map<std::string, GraphicRenderInfo*> m_render_infos;
		
		/**
		 * Data structure containing the mapping of ingame object types to the name of renderinfos used to display these objects
		 */
		static std::map<std::string, GraphicObject::Type> m_graphic_mapping;
		
		/**
		 * \brief Reads the Renderinfos recursively from an XML node
		 * \param node XML node
		 */
		static void loadRenderInfos(TiXmlNode* node);
		
		/**
		 * \brief Reads Renderinfo from an XML node
		 * \param node XML node
		 */
		static void loadRenderInfo(TiXmlNode* node, GraphicRenderInfo* info);
		
		/**
		 * \brief Reads MovableObjectInfo from an XML node
		 * \param node XML node
		 * \param info data structure where results are written to
		 */
		static void loadMovableObjectInfo(TiXmlNode* node, MovableObjectInfo* info);
		
		/**
		 * \brief Reads ActionRenderInfo from an XML node
		 * \param node XML node
		 * \param info data structure where results are written to
		 */
		static void loadActionRenderInfo(TiXmlNode* node, ActionRenderInfo* info);
		
		/**
		 * \brief OGRE SceneManager that is used to create all OGRE Objects
		 */
		static Ogre::SceneManager* m_scene_manager;
		
		/**
		 * \brief Helper Class that is used for highlighting (just copied from OGRE wiki)
		 */
		static StencilOpQueueListener* m_stencil_listener;
		
		/**
		 * \brief internal pool of Particle system
		 */
		static std::multimap<std::string, Ogre::ParticleSystem*> m_particle_system_pool;
		
		/**
		 * \brief Name of XML file being read at the moment
		 */
		static std::string m_filename;
		
	public:
		static double g_global_scale;
};

#endif

