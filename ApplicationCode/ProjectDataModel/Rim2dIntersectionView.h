/////////////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2018-     Statoil ASA
// 
//  ResInsight is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
// 
//  ResInsight is distributed in the hope that it will be useful, but WITHOUT ANY
//  WARRANTY; without even the implied warranty of MERCHANTABILITY or
//  FITNESS FOR A PARTICULAR PURPOSE.
// 
//  See the GNU General Public License at <http://www.gnu.org/licenses/gpl.html> 
//  for more details.
//
/////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "RiuViewerToViewInterface.h"
#include "RimViewWindow.h"

#include "cafAppEnum.h"
#include "cafPdmChildArrayField.h"
#include "cafPdmChildField.h"
#include "cafPdmField.h"
#include "cafPdmFieldCvfColor.h"    
#include "cafPdmFieldCvfMat4d.h"
#include "cafPdmFieldCvfVec3d.h"
#include "cafPdmObject.h"

#include "RivCellSetEnum.h"

#include "cvfArray.h"
#include "cvfBase.h"
#include "cvfCollection.h"
#include "cvfObject.h"

#include <QPointer>


class Rim3dOverlayInfoConfig;
class RimCase;
class RimCellRangeFilter;
class RimCellRangeFilterCollection;
class RimIntersectionCollection;
class RimGridCollection;
class RimPropertyFilterCollection;
class RimViewController;
class RimViewLinker;
class RiuViewer;
class RimWellPathCollection;

namespace cvf
{
class BoundingBox;
class ModelBasicList;
class Scene;
class String;
class Transform;
class Part;
}

namespace caf
{
class DisplayCoordTransform;
}
//==================================================================================================
///  
///  
//==================================================================================================
class Rim2dIntersectionView : public RimViewWindow, public RiuViewerToViewInterface
{
    CAF_PDM_HEADER_INIT;
public:
    Rim2dIntersectionView(void);
    virtual ~Rim2dIntersectionView(void);

    // Public fields: 

    caf::PdmField<QString>                  name;
    caf::PdmField<double>                   scaleZ;
    caf::PdmField<int>                      maximumFrameRate;
    caf::PdmField<bool>                     hasUserRequestedAnimation;

    // Draw style 

    enum MeshModeType    { FULL_MESH, FAULTS_MESH, NO_MESH    };
    enum SurfaceModeType { SURFACE,   NO_SURFACE };

    caf::PdmField< caf::AppEnum< MeshModeType > >    meshMode;
    caf::PdmField< caf::AppEnum< SurfaceModeType > > surfaceMode;

    RiuViewer*                              viewer();

    void                                    setMeshOnlyDrawstyle();
    void                                    setMeshSurfDrawstyle();
    void                                    setSurfOnlyDrawstyle();
    void                                    setFaultMeshSurfDrawstyle();
    void                                    setSurfaceDrawstyle();

    void                                    disableLighting(bool disable);
    bool                                    isLightingDisabled() const;

    void                                    showGridCells(bool enableGridCells);
    bool                                    isGridVisualizationMode() const;

    void                                    setScaleZAndUpdate(double scaleZ);
    virtual bool                            showActiveCellsOnly();
    virtual bool                            isUsingFormationNames() const = 0;

    virtual QImage                          snapshotWindowContent() override;
    virtual void                            zoomAll() override;
    void                                    forceShowWindowOn();

    // Animation
    int                                     currentTimeStep() const { return m_currentTimeStep;}
    void                                    setCurrentTimeStep(int frameIdx);
    void                                    setCurrentTimeStepAndUpdate(int frameIdx) override;

    // Updating 
    void                                    updateCurrentTimeStepAndRedraw() override;
    virtual void                            scheduleGeometryRegen(RivCellSetEnum geometryType) = 0;
    void                                    scheduleCreateDisplayModelAndRedraw();
    void                                    createDisplayModelAndRedraw();
    void                                    createHighlightDisplayModelWithRedraw();
    void                                    updateAnnotationItems();

    cvf::ref<caf::DisplayCoordTransform>    displayCoordTransform() const override;

    virtual RimCase*                        ownerCase() const = 0;
    //Rim3dOverlayInfoConfig*                 overlayInfoConfig() const;

protected:

    void                                    setDefaultView();

    RimWellPathCollection*                  wellPathCollection();
    void                                    addWellPathsToModel(cvf::ModelBasicList* wellPathModelBasicList, 
                                                                const cvf::BoundingBox& wellPathClipBoundingBox);

    void                                    addDynamicWellPathsToModel(cvf::ModelBasicList* wellPathModelBasicList, 
                                                                       const cvf::BoundingBox& wellPathClipBoundingBox);

    void                                    createHighlightDisplayModel();

    // Abstract methods to implement in subclasses

    virtual void                            createDisplayModel() = 0;
    virtual void                            createPartCollectionFromSelection(cvf::Collection<cvf::Part>* parts) = 0;

    virtual void                            updateDisplayModelVisibility() = 0;
    virtual void                            clampCurrentTimestep() = 0;

    virtual void                            updateCurrentTimeStep() = 0;
    virtual void                            updateStaticCellColors() = 0;

    virtual void                            updateScaleTransform() = 0;
    virtual cvf::Transform*                 scaleTransform() = 0;

    virtual void                            resetLegendsInViewer() = 0;

    // Overridden PdmObject methods:

    virtual caf::PdmFieldHandle*            userDescriptionField() override { return &name; }
    virtual void                            setupBeforeSave() override;
    virtual void                            fieldChangedByUi(const caf::PdmFieldHandle* changedField, const QVariant& oldValue, const QVariant& newValue) override;
    virtual void                            defineUiOrdering(QString uiConfigName, caf::PdmUiOrdering& uiOrdering) override;

    // Overridden ViewWindow methods:

    virtual QWidget*                        createViewWidget(QWidget* mainWindowParent) override; 
    virtual void                            updateViewWidgetAfterCreation() override; 
    virtual void                            updateMdiWindowTitle() override;
    virtual void                            deleteViewWidget() override;
    virtual QWidget*                        viewWidget() override;

protected: // Fields
    caf::PdmField<int>                                m_currentTimeStep;
    //caf::PdmChildField<Rim3dOverlayInfoConfig*>       m_overlayInfoConfig;

protected: 
    QPointer<RiuViewer>                               m_viewer;

    cvf::ref<cvf::ModelBasicList>                     m_wellPathPipeVizModel;
    cvf::ref<cvf::ModelBasicList>                     m_crossSectionVizModel;
    cvf::ref<cvf::ModelBasicList>                     m_highlightVizModel;

private:
    // Implementation of RiuViewerToViewInterface

    virtual cvf::Color3f                    backgroundColor() const override { return m_backgroundColor();} 
    virtual void                            setCameraPosition(const cvf::Mat4d& cameraPosition) override               { m_cameraPosition = cameraPosition; }
    virtual void                            setCameraPointOfInterest(const cvf::Vec3d& cameraPointOfInterest) override { m_cameraPointOfInterest = cameraPointOfInterest;}
    virtual QString                         timeStepName(int frameIdx) const override;
    virtual void                            endAnimation() override;
    virtual caf::PdmObjectHandle*           implementingPdmObject() override  { return this; }
    virtual void                            handleMdiWindowClosed() override;
    virtual void                            setMdiWindowGeometry(const RimMdiWindowGeometry& windowGeometry) override;
    virtual void                            selectOverlayInfoConfig() override;

private:
    bool                                    m_previousGridModeMeshLinesWasFaults;
    caf::PdmField<bool>                     m_disableLighting;
    caf::PdmField<cvf::Mat4d>               m_cameraPosition;
    caf::PdmField<cvf::Vec3d>               m_cameraPointOfInterest;
    caf::PdmField< cvf::Color3f >           m_backgroundColor;
};


