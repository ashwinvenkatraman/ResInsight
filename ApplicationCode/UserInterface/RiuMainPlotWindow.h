/////////////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2016 Statoil ASA
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

#include "RiuMainWindowBase.h"

#include "cafPdmUiDragDropInterface.h"
#include "cafPdmPointer.h"

#include <QMdiArea>

#include <memory>

class QMdiSubWindow;

class RiuViewer;

struct RimMdiWindowGeometry;
class RimViewWindow;

namespace caf
{
     class PdmUiTreeView;
     class PdmObject;
     class PdmUiPropertyView;
     class PdmUiItem;
     class PdmUiToolBarEditor;
}

//==================================================================================================
//
// 
//
//==================================================================================================
class RiuMainPlotWindow : public RiuMainWindowBase
{
    Q_OBJECT

public:
    RiuMainPlotWindow();
    
    QString             mainWindowName() override;
    
    void                initializeGuiNewProjectLoaded();
    void                cleanupGuiBeforeProjectClose();
    void                cleanUpTemporaryWidgets();

    void                removeViewer( QWidget* viewer ) override;
    void                addViewer(QWidget* viewer, const RimMdiWindowGeometry& windowsGeometry) override;
    void                setActiveViewer(QWidget* subWindow) override;

    void                hideAllDockWindows();

    void                setDefaultWindowSize();


    RimMdiWindowGeometry windowGeometryForViewer(QWidget* viewer) override;

    void                tileWindows();
    bool                isAnyMdiSubWindowVisible();
    QMdiSubWindow*      findMdiSubWindow(QWidget* viewer);
    QList<QMdiSubWindow*> subWindowList(QMdiArea::WindowOrder order);

    void                addToTemporaryWidgets(QWidget* widget);

    void                updateSummaryPlotToolBar();

protected:
    void                closeEvent(QCloseEvent* event) override;

private:
    void                setPdmRoot(caf::PdmObject* pdmRoot);

    void                createMenus();
    void                createToolBars();
    void                createDockPanels();

    void                restoreTreeViewState();

    void                refreshToolbars();
    
    static QStringList  toolbarCommandIds(const QString& toolbarName = "");

private slots:

    friend class RiuMdiSubWindow;

    void                slotBuildWindowActions();

    void                slotSubWindowActivated(QMdiSubWindow* subWindow);

    void                selectedObjectsChanged();
    void                customMenuRequested(const QPoint& pos);

private:
    QByteArray          m_initialDockAndToolbarLayout;    // Initial dock window and toolbar layout, used to reset GUI

    QMdiArea*           m_mdiArea;
    caf::PdmPointer<RimViewWindow>  m_activePlotViewWindow;

    QMenu*              m_windowMenu;

    caf::PdmUiToolBarEditor*    m_summaryPlotToolBarEditor;
    std::unique_ptr<caf::PdmUiDragDropInterface> m_dragDropInterface;
    
    caf::PdmUiPropertyView*     m_pdmUiPropertyView;

    bool                        m_blockSlotSubWindowActivated;

    std::vector<QWidget*>       m_temporaryWidgets;
};
