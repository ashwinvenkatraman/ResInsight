/////////////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2017 Statoil ASA
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

#include "RifEclipseOutputTableFormatter.h"

#include "RigWellLogExtractionTools.h"
#include "RigWellPathIntersectionTools.h"
#include "RigCompletionData.h"

#include "RimExportCompletionDataSettings.h"

#include "cafCmdFeature.h"

#include "cvfBoundingBox.h"


class RimWellPath;
class RimEclipseCase;
class RigEclipseCaseData;
class RigMainGrid;
class RigCell;
class RimFishbonesMultipleSubs;

//==================================================================================================
/// 
//==================================================================================================
struct WellSegmentLateralIntersection {
    WellSegmentLateralIntersection(int segmentNumber, int attachedSegmentNumber, size_t cellIndex, double length, double depth)
        : segmentNumber(segmentNumber),
          attachedSegmentNumber(attachedSegmentNumber),
          cellIndex(cellIndex),
          length(length),
          depth(depth),
          direction(POS_I),
          mainBoreCell(false)
    {}

    int                      segmentNumber;
    int                      attachedSegmentNumber;
    size_t                   cellIndex;
    bool                     mainBoreCell;
    double                   length;
    double                   depth;
    WellPathCellDirection    direction;
};

//==================================================================================================
/// 
//==================================================================================================
struct WellSegmentLateral {
    WellSegmentLateral(size_t lateralIndex) : lateralIndex(lateralIndex) {}

    size_t                                      lateralIndex;
    int                                         branchNumber;
    std::vector<WellSegmentLateralIntersection> intersections;
};

//==================================================================================================
/// 
//==================================================================================================
struct WellSegmentLocation {
    WellSegmentLocation(const RimFishbonesMultipleSubs* subs, double measuredDepth, double trueVerticalDepth, size_t subIndex, int segmentNumber = -1)
        : fishbonesSubs(subs),
          measuredDepth(measuredDepth),
          trueVerticalDepth(trueVerticalDepth),
          subIndex(subIndex),
          segmentNumber(segmentNumber)
    {
    }

    const RimFishbonesMultipleSubs*       fishbonesSubs;
    double                                measuredDepth;
    double                                trueVerticalDepth;
    size_t                                subIndex;
    int                                   segmentNumber;
    std::vector<WellSegmentLateral>       laterals;
};

//==================================================================================================
/// 
//==================================================================================================
struct EclipseCellIndexRange {
    size_t i;
    size_t j;
    size_t k1;
    size_t k2;
};

//==================================================================================================
/// 
//==================================================================================================
typedef std::tuple<size_t, size_t, size_t> EclipseCellIndex;

//==================================================================================================
/// 
//==================================================================================================
class RicWellPathExportCompletionDataFeature : public caf::CmdFeature
{
    CAF_CMD_HEADER_INIT;
protected:

    // Overrides
    virtual bool isCommandEnabled() override;
    virtual void onActionTriggered(bool isChecked) override;
    virtual void setupActionLook(QAction* actionToSetup) override;

    std::vector<RimWellPath*>                    selectedWellPaths();

public:
    static std::vector<WellSegmentLocation>      findWellSegmentLocations(const RimEclipseCase* caseToApply, const RimWellPath* wellPath);
    static std::vector<WellSegmentLocation>      findWellSegmentLocations(const RimEclipseCase* caseToApply, const RimWellPath* wellPath, const std::vector<RimFishbonesMultipleSubs*>& fishbonesSubs);

private:
    static void                                  exportCompletions(const std::vector<RimWellPath*>& wellPaths, const RimExportCompletionDataSettings& exportSettings);

    static void                                  generateCompdatTable(RifEclipseOutputTableFormatter& formatter, const std::vector<RigCompletionData>& completionData);
    static void                                  generateWpimultTable(RifEclipseOutputTableFormatter& formatter, const std::vector<RigCompletionData>& completionData);

    static std::vector<RigCompletionData>        generateFishbonesCompdatValues(const RimWellPath* wellPath, const RimExportCompletionDataSettings& settings);
    static std::vector<RigCompletionData>        generateFishbonesWellPathCompdatValues(const RimWellPath* wellPath, const RimExportCompletionDataSettings& settings);
    static std::vector<RigCompletionData>        generatePerforationsCompdatValues(const RimWellPath* wellPath, const RimExportCompletionDataSettings& settings);

    static std::map<size_t, double>              computeLateralsPerCell(const std::vector<WellSegmentLocation>& segmentLocations, bool removeMainBoreCells);
    static std::vector<size_t>                   findIntersectingCells(const RigEclipseCaseData* grid, const std::vector<cvf::Vec3d>& coords);
    static void                                  markWellPathCells(const std::vector<size_t>& wellPathCells, std::vector<WellSegmentLocation>* locations);
    static bool                                  wellSegmentLocationOrdering(const WellSegmentLocation& first, const WellSegmentLocation& second);
    static bool                                  isPointBetween(const cvf::Vec3d& pointA, const cvf::Vec3d& pointB, const cvf::Vec3d& needle);
    static void                                  calculateLateralIntersections(const RimEclipseCase* caseToApply, WellSegmentLocation* location, int* branchNum, int* segmentNum);
    static void                                  assignBranchAndSegmentNumbers(const RimEclipseCase* caseToApply, std::vector<WellSegmentLocation>* locations);

    static void                                  appendCompletionData(std::map<IJKCellIndex, RigCompletionData>* completionData, const std::vector<RigCompletionData>& data);

    static CellDirection                         wellPathCellDirectionToCellDirection(WellPathCellDirection direction);
};