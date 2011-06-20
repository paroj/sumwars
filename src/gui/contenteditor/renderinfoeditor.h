#ifndef RENDERINFOEDITOR_H
#define RENDERINFOEDITOR_H

#include "graphicmanager.h"
#include "contenteditortab.h"
#include "worldobject.h"
#include "objectloader.h"

/**
 * \brief Editor class for FixedObject XML
 */
class RenderInfoEditor : public ContentEditorTab
{
	public:
		/**
		 * \brief Initialises the panel
		 * \param parent parent CEGUI window
		 */
		virtual void init(CEGUI::Window* parent);
		
		/**
		 * \brief Updates the content of the tab
		 * Function is called once each tick.
		 */
		virtual void update();
		
		/**
		 * \brief returns the GraphicObject modified with this editor
		 */
		GraphicObject* getEditedGraphicObject()
		{
			return m_edited_graphicobject;
		}
		
		/**
		 * \brief Makes a unique copy of the current renderinfo
		 */
		std::string getUniqueRenderinfo();
		
	private:
		/**
		 * \param evt CEGUI event arguments
		 * \brief Called upon selecting a mesh from the combobox
		 * Updates the content of the editor scene to display the selected mesh.
		 */
		bool onMeshSelected(const CEGUI::EventArgs& evt);
		
		/**
		 * \param evt CEGUI event arguments
		 * \brief Called upon selecting a mesh from the submesh combobox
		 */
		bool onSubMeshSelected(const CEGUI::EventArgs& evt);
		
		/**
		 * \param evt CEGUI event arguments
		 * \brief Called clicking add Submesh button
		 */
		bool onSubMeshAdded(const CEGUI::EventArgs& evt);
		
		/**
		 * \param evt CEGUI event arguments
		 * \brief Called upon selecting a subobject from the subobject combobox
		 */
		bool onSubObjectSelected(const CEGUI::EventArgs& evt);
		
		/**
		 * \param evt CEGUI event arguments
		 * \brief Called upon changing any element in the submesh tab
		 */
		bool onSubMeshModified(const CEGUI::EventArgs& evt);
		
		/**
		 * \param evt CEGUI event arguments
		 * \brief Called upon clicking on the delete Submesh button
		 */
		bool onSubMeshDeleted(const CEGUI::EventArgs& evt);
		
		/**
		 * \param evt CEGUI event arguments
		 * \brief Called submitting new RenderinfoXML
		 */
		bool onRenderinfoXMLModified(const CEGUI::EventArgs& evt);
		
		/**
		 * \brief updates the content of the preview image and the GUI Elements
		 */
		void updateRenderInfoGUI();
		
		/**
		 * \brief updates the submesh editor component
		 * \param objectname name of the Object to be edited
		 * \param updateList if set to true, the list of objects will be updated, too
		 */
		void updateSubmeshEditor(std::string objectname="", bool updateList = true);
		
		/**
		 * \brief updates the list of bones to attach to
		 * \param minfo data structure with information on the mesh
		 */
		void updateBoneList();
		
		/**
		 * \brief updates the content of the Renderinfo XML editor
		 */
		void updateRenderInfoXML();
		
		/**
		 * \brief updates the preview Image
		 */
		void updatePreviewImage();
		
		/**
		 * \brief Checks for all attached Objects, whether their parent mesh and bone are still present
		 * Dangling submeshes are deleted
		 */ 
		void checkAttachedObjects();
		
		/**
		 * \brief marks if the renderinfo has been modified
		 */
		bool m_modified_renderinfo;
		
		/**
		 * \brief marks that the XML of the renderinfo was modified
		 */
		bool m_modified_renderinfo_xml;
		
		/**
		 * \brief Renderinfo edited with this window
		 */
		GraphicRenderInfo m_edited_renderinfo;
		
		/**
		 * \brief XML representation of the  Renderinfo edited with this window
		 */
		TiXmlDocument m_renderinfo_xml;  
		
		/**
		 * \brief GraphicObject of the Renderinfo edited with this window
		 */
		GraphicObject* m_edited_graphicobject;
		
		/**
		 * \brief unique ID for renderinfo objects
		 */
		int m_unique_id;

};


#endif // RENDERINFOEDITOR_H