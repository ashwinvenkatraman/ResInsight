//##################################################################################################
//
//   Custom Visualization Core library
//   Copyright (C) 2011-2013 Ceetron AS
//
//   This library may be used under the terms of either the GNU General Public License or
//   the GNU Lesser General Public License as follows:
//
//   GNU General Public License Usage
//   This library is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
//   This library is distributed in the hope that it will be useful, but WITHOUT ANY
//   WARRANTY; without even the implied warranty of MERCHANTABILITY or
//   FITNESS FOR A PARTICULAR PURPOSE.
//
//   See the GNU General Public License at <<http://www.gnu.org/licenses/gpl.html>>
//   for more details.
//
//   GNU Lesser General Public License Usage
//   This library is free software; you can redistribute it and/or modify
//   it under the terms of the GNU Lesser General Public License as published by
//   the Free Software Foundation; either version 2.1 of the License, or
//   (at your option) any later version.
//
//   This library is distributed in the hope that it will be useful, but WITHOUT ANY
//   WARRANTY; without even the implied warranty of MERCHANTABILITY or
//   FITNESS FOR A PARTICULAR PURPOSE.
//
//   See the GNU Lesser General Public License at <<http://www.gnu.org/licenses/lgpl-2.1.html>>
//   for more details.
//
//##################################################################################################

#pragma once

#include "cafPdmUiItem.h"

#include <QString>
#include <vector>

namespace caf 
{

class PdmUiGroup;
class PdmFieldHandle;
class PdmObjectHandle;

//==================================================================================================
/// Class storing the order and grouping of fields and groups of fields etc. to be used in the Gui
//==================================================================================================

class PdmUiOrdering
{
public:
    PdmUiOrdering(): m_skipRemainingFields(false) { };
    virtual ~PdmUiOrdering();

    PdmUiOrdering(const PdmUiOrdering&) = delete;
    PdmUiOrdering& operator=(const PdmUiOrdering&) = delete;

    PdmUiGroup*                     addNewGroup(const QString& displayName);
    PdmUiGroup*                     addNewGroupWithKeyword(const QString& displayName, const QString& keyword);

    void                            add(const PdmFieldHandle* field);
    void                            add(const PdmObjectHandle* obj);

    void                            skipRemainingFields(bool doSkip = true);

    // Pdm internal methods

    const std::vector<PdmUiItem*>&  uiItems() const;
    bool                            contains(const PdmUiItem* item) const;
    bool                            isIncludingRemainingFields() const;

private:
    std::vector<PdmUiItem*>         m_ordering;            ///< The order of groups and fields
    std::vector<PdmUiGroup*>        m_createdGroups;       ///< Owned PdmUiGroups, for memory management only
    bool                            m_skipRemainingFields;
};

} // End of namespace caf

#include "cafPdmUiGroup.h"