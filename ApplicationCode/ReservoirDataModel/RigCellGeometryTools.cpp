/////////////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2017     Statoil ASA
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


#include "RigCellGeometryTools.h"
#include "cvfStructGrid.h"
#include "cvfGeometryTools.h"

#include "cafHexGridIntersectionTools/cafHexGridIntersectionTools.h"
#include "cvfBoundingBox.h"

#include "clipper/clipper.hpp"

#include <vector>

//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
RigCellGeometryTools::RigCellGeometryTools()
{

}

//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
RigCellGeometryTools::~RigCellGeometryTools()
{

}

//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
bool RigCellGeometryTools::planeHexCellIntersection(cvf::Vec3d * hexCorners, cvf::Plane fracturePlane, std::list<std::pair<cvf::Vec3d, cvf::Vec3d > > & intersectionLineSegments)
{
    bool isCellIntersected = false;
    for (int face = 0; face < 6; ++face)
    {
        cvf::ubyte faceVertexIndices[4];
        cvf::StructGridInterface::cellFaceVertexIndices(static_cast<cvf::StructGridInterface::FaceType>(face), faceVertexIndices);

        cvf::Vec3d faceCenter = cvf::GeometryTools::computeFaceCenter(hexCorners[faceVertexIndices[0]], hexCorners[faceVertexIndices[1]], hexCorners[faceVertexIndices[2]], hexCorners[faceVertexIndices[3]]);

        for (int i = 0; i < 4; i++)
        {
            int next = i < 3 ? i + 1 : 0;
            caf::HexGridIntersectionTools::ClipVx triangleIntersectionPoint1;
            caf::HexGridIntersectionTools::ClipVx triangleIntersectionPoint2;

            bool isMostVxesOnPositiveSideOfP1 = false;

            bool isIntersectingPlane = caf::HexGridIntersectionTools::planeTriangleIntersection(fracturePlane,
                hexCorners[faceVertexIndices[i]], 0,
                hexCorners[faceVertexIndices[next]], 1,
                faceCenter, 2,
                &triangleIntersectionPoint1, &triangleIntersectionPoint2, &isMostVxesOnPositiveSideOfP1);

            if (isIntersectingPlane)
            {
                isCellIntersected = true;
                intersectionLineSegments.push_back({ triangleIntersectionPoint1.vx, triangleIntersectionPoint2.vx });
            }
        }
    }    return isCellIntersected;
}

//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
void RigCellGeometryTools::createPolygonFromLineSegments(std::list<std::pair<cvf::Vec3d, cvf::Vec3d>> &intersectionLineSegments, std::vector<std::vector<cvf::Vec3d>> &polygons)
{
    bool startNewPolygon = true;
    while (!intersectionLineSegments.empty())
    {
        if (startNewPolygon)
        {
            std::vector<cvf::Vec3d> polygon;
            //Add first line segments to polygon and remove from list
            std::pair<cvf::Vec3d, cvf::Vec3d > linesegment = intersectionLineSegments.front();
            polygon.push_back(linesegment.first);
            polygon.push_back(linesegment.second);
            intersectionLineSegments.remove(linesegment);
            polygons.push_back(polygon);
            startNewPolygon = false;
        }

        std::vector<cvf::Vec3d>& polygon = polygons.back();

        //Search remaining list for next point...

        bool isFound = false;
        float tolerance = 0.0001f;

        for (std::list<std::pair<cvf::Vec3d, cvf::Vec3d > >::iterator lIt = intersectionLineSegments.begin(); lIt != intersectionLineSegments.end(); lIt++)
        {
            cvf::Vec3d lineSegmentStart = lIt->first;
            cvf::Vec3d lineSegmentEnd = lIt->second;
            cvf::Vec3d polygonEnd = polygon.back();
            
            double lineSegmentLength = (lineSegmentStart - lineSegmentEnd).lengthSquared();
            if (lineSegmentLength < tolerance*tolerance)
            {
                intersectionLineSegments.erase(lIt);
                isFound = true; 
                break;
            }


            double lineSegmentStartDiff = (lineSegmentStart - polygonEnd).lengthSquared();
            if (lineSegmentStartDiff < tolerance*tolerance)
            {
                polygon.push_back(lIt->second);
                intersectionLineSegments.erase(lIt);
                isFound = true;
                break;
            }

            double lineSegmentEndDiff = (lineSegmentEnd - polygonEnd).lengthSquared();
            if (lineSegmentEndDiff < tolerance*tolerance)
            {
                polygon.push_back(lIt->first);
                intersectionLineSegments.erase(lIt);
                isFound = true;
                break;
            }
        }

        if (isFound)
        {
            continue;
        }
        else
        {
            startNewPolygon = true;
        }
    }



}

//--------------------------------------------------------------------------------------------------
///  
//--------------------------------------------------------------------------------------------------
void RigCellGeometryTools::findCellLocalXYZ(cvf::Vec3d * hexCorners, cvf::Vec3d &localXdirection, cvf::Vec3d &localYdirection, cvf::Vec3d &localZdirection)
{
    cvf::ubyte faceVertexIndices[4];
    cvf::StructGridInterface::FaceEnum face;

    face = cvf::StructGridInterface::NEG_I;
    cvf::StructGridInterface::cellFaceVertexIndices(face, faceVertexIndices);
    cvf::Vec3d faceCenterNegI = cvf::GeometryTools::computeFaceCenter(hexCorners[faceVertexIndices[0]], hexCorners[faceVertexIndices[1]], hexCorners[faceVertexIndices[2]], hexCorners[faceVertexIndices[3]]);
    //TODO: Should we use face centroids instead of face centers?

    face = cvf::StructGridInterface::POS_I;
    cvf::StructGridInterface::cellFaceVertexIndices(face, faceVertexIndices);
    cvf::Vec3d faceCenterPosI = cvf::GeometryTools::computeFaceCenter(hexCorners[faceVertexIndices[0]], hexCorners[faceVertexIndices[1]], hexCorners[faceVertexIndices[2]], hexCorners[faceVertexIndices[3]]);

    face = cvf::StructGridInterface::NEG_J;
    cvf::StructGridInterface::cellFaceVertexIndices(face, faceVertexIndices);
    cvf::Vec3d faceCenterNegJ = cvf::GeometryTools::computeFaceCenter(hexCorners[faceVertexIndices[0]], hexCorners[faceVertexIndices[1]], hexCorners[faceVertexIndices[2]], hexCorners[faceVertexIndices[3]]);

    face = cvf::StructGridInterface::POS_J;
    cvf::StructGridInterface::cellFaceVertexIndices(face, faceVertexIndices);
    cvf::Vec3d faceCenterPosJ = cvf::GeometryTools::computeFaceCenter(hexCorners[faceVertexIndices[0]], hexCorners[faceVertexIndices[1]], hexCorners[faceVertexIndices[2]], hexCorners[faceVertexIndices[3]]);
    
    cvf::Vec3d faceCenterCenterVectorI = faceCenterPosI - faceCenterNegI;
    cvf::Vec3d faceCenterCenterVectorJ = faceCenterPosJ - faceCenterNegJ;

    localZdirection.cross(faceCenterCenterVectorI, faceCenterCenterVectorJ);
    localZdirection.normalize();

    cvf::Vec3d crossPoductJZ;
    crossPoductJZ.cross(faceCenterCenterVectorJ, localZdirection);
    localXdirection = faceCenterCenterVectorI + crossPoductJZ;
    localXdirection.normalize();

    cvf::Vec3d crossPoductIZ;
    crossPoductIZ.cross(faceCenterCenterVectorI, localZdirection);
    localYdirection = faceCenterCenterVectorJ - crossPoductIZ;
    localYdirection.normalize();
    
    //TODO: Check if we end up with 0-vectors, and handle this case...
}

//--------------------------------------------------------------------------------------------------
///  
//--------------------------------------------------------------------------------------------------
//TODO: Suggested rename: polygonLengthWeightedByArea. polygonAverageLengthWeightedByArea
double RigCellGeometryTools::polygonAreaWeightedLength(cvf::Vec3d directionOfLength, std::vector<cvf::Vec3d> polygonToCalcLengthOf)
{
    //TODO: Check that polygon is in xy plane

    //Find bounding box
    cvf::BoundingBox polygonBBox;
    for (cvf::Vec3d nodeCoord : polygonToCalcLengthOf) polygonBBox.add(nodeCoord);
    cvf::Vec3d bboxCorners[8];
    polygonBBox.cornerVertices(bboxCorners);


    //Split bounding box in multiple polygons (2D)
    int resolutionOfLengthCalc = 20;
    cvf::Vec3d widthOfPolygon = polygonBBox.extent() / resolutionOfLengthCalc;

    std::vector<double> areasOfPolygonContributions;
    std::vector<double> lengthOfPolygonContributions;

    for (int i = 0; i < resolutionOfLengthCalc; i++)
    {
        //Doing the same thing twice, this part can be optimized... 
        std::pair<cvf::Vec3d, cvf::Vec3d> line1 = getLineThroughBoundingBox(directionOfLength, polygonBBox, bboxCorners[0] + widthOfPolygon*i);    
        std::pair<cvf::Vec3d, cvf::Vec3d> line2 = getLineThroughBoundingBox(directionOfLength, polygonBBox, bboxCorners[0] + widthOfPolygon*(i+1));
        std::vector<cvf::Vec3d> polygon;
        polygon.push_back(line1.first);
        polygon.push_back(line1.second);
        polygon.push_back(line2.second);
        polygon.push_back(line2.first);

        //Use clipper to find overlap between bbpolygon and fracture
        std::vector<std::vector<cvf::Vec3d> > clippedPolygons = clipPolygons(polygonToCalcLengthOf, polygon);

        double area = 0;
        double length = 0;
        cvf::Vec3d areaVector = cvf::Vec3d::ZERO;

        //Calculate length (max-min) and area    
        for (std::vector<cvf::Vec3d> clippedPolygon : clippedPolygons)
        {
            areaVector = cvf::GeometryTools::polygonAreaNormal3D(clippedPolygon);
            area += areaVector.length();
            length += getLengthOfPolygonAlongLine(line1, clippedPolygon); //For testing that parts of code fit together... 
        }
        areasOfPolygonContributions.push_back(area);
        lengthOfPolygonContributions.push_back(length);
    }

    //Calculate area-weighted length average.  
    double totalArea = 0.0;
    double totalAreaXlength = 0.0;

    for (size_t i = 0; i < areasOfPolygonContributions.size(); i++)
    {
        totalArea += areasOfPolygonContributions[i];
        totalAreaXlength += (areasOfPolygonContributions[i] * lengthOfPolygonContributions[i]);
    }

    double areaWeightedLength = totalAreaXlength / totalArea;
    return areaWeightedLength;
}

//--------------------------------------------------------------------------------------------------
///  
//--------------------------------------------------------------------------------------------------
std::vector<std::vector<cvf::Vec3d> > RigCellGeometryTools::clipPolygons(std::vector<cvf::Vec3d> polygon1, std::vector<cvf::Vec3d> polygon2)
{
    int polygonScaleFactor = 10000; //For transform to clipper int 
    int xInt, yInt;
    
    //Convert to int for clipper library and store as clipper "path"
    ClipperLib::Path polygon1path;
    for (cvf::Vec3d& v : polygon1)
    {
        xInt = v.x()*polygonScaleFactor;
        yInt = v.y()*polygonScaleFactor;
        polygon1path.push_back(ClipperLib::IntPoint(xInt, yInt));
    }

    ClipperLib::Path polygon2path;
    for (cvf::Vec3d& v : polygon2)
    {
        xInt = v.x()*polygonScaleFactor;
        yInt = v.y()*polygonScaleFactor;
        polygon2path.push_back(ClipperLib::IntPoint(xInt, yInt));
    }

    ClipperLib::Clipper clpr;
    clpr.AddPath(polygon1path, ClipperLib::ptSubject, true);
    clpr.AddPath(polygon2path, ClipperLib::ptClip, true);

    ClipperLib::Paths solution;
    clpr.Execute(ClipperLib::ctIntersection, solution, ClipperLib::pftEvenOdd, ClipperLib::pftEvenOdd);

    //Convert back to std::vector<std::vector<cvf::Vec3d> >
    std::vector<std::vector<cvf::Vec3d> > clippedPolygons;
    for (ClipperLib::Path pathInSol : solution)
    {
        std::vector<cvf::Vec3d> clippedPolygon;
        for (ClipperLib::IntPoint IntPosition : pathInSol)
        {
            cvf::Vec3d v = cvf::Vec3d::ZERO;
            v.x() = (float)IntPosition.X / (float)polygonScaleFactor;
            v.y() = (float)IntPosition.Y / (float)polygonScaleFactor;
            clippedPolygon.push_back(v);
        }
        clippedPolygons.push_back(clippedPolygon);
    }

    return clippedPolygons;

}

//--------------------------------------------------------------------------------------------------
///  
//--------------------------------------------------------------------------------------------------
std::pair<cvf::Vec3d, cvf::Vec3d> RigCellGeometryTools::getLineThroughBoundingBox(cvf::Vec3d lineDirection, cvf::BoundingBox polygonBBox, cvf::Vec3d pointOnLine)
{
    cvf::Vec3d bboxCorners[8];
    polygonBBox.cornerVertices(bboxCorners);

    cvf::Vec3d startPoint = pointOnLine;
    cvf::Vec3d endPoint = pointOnLine;
    

    //To avoid doing many iterations in loops below linedirection should be quite large. 
    lineDirection.normalize();
    lineDirection = lineDirection * polygonBBox.extent().length() / 5;

    //Extend line in positive direction
    while (polygonBBox.contains(startPoint))
    {
        startPoint = startPoint + lineDirection;
    }
    //Extend line in negative direction
    while (polygonBBox.contains(endPoint))
    {
        endPoint = endPoint - lineDirection;
    }

    std::pair<cvf::Vec3d, cvf::Vec3d> line;
    line = { startPoint, endPoint };
    return line;
}

//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
double RigCellGeometryTools::getLengthOfPolygonAlongLine(std::pair<cvf::Vec3d, cvf::Vec3d> line, std::vector<cvf::Vec3d> polygon)
{
    cvf::BoundingBox lineBoundingBox;

    std::vector<cvf::Vec3d> pointsOnLine;

    for (cvf::Vec3d polygonPoint : polygon)
    {
        cvf::Vec3d pointOnLine = cvf::GeometryTools::projectPointOnLine(line.first, line.second, polygonPoint, nullptr);
        lineBoundingBox.add(pointOnLine);
    }

    double length = lineBoundingBox.extent().length();
    return length;
}