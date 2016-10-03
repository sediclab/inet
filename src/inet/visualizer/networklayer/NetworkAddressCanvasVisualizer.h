//
// Copyright (C) 2016 OpenSim Ltd.
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//

#ifndef __INET_NETWORKADDRESSCANVASVISUALIZER_H
#define __INET_NETWORKADDRESSCANVASVISUALIZER_H

#include "inet/common/figures/BoxedLabelFigure.h"
#include "inet/visualizer/base/NetworkAddressVisualizerBase.h"
#include "inet/visualizer/networknode/NetworkNodeCanvasVisualizer.h"

namespace inet {

namespace visualizer {

class INET_API NetworkAddressCanvasVisualizer : public NetworkAddressVisualizerBase
{
  protected:
    class INET_API CanvasCacheEntry : public CacheEntry {
      public:
        BoxedLabelFigure *figure = nullptr;

      public:
        CanvasCacheEntry(int nodeId, int interfaceId) : CacheEntry(nodeId, interfaceId) { }
    };

  protected:
    double zIndex = NaN;
    NetworkNodeCanvasVisualizer *networkNodeVisualizer = nullptr;

  protected:
    virtual void initialize(int stage) override;
    virtual CacheEntry *createCacheEntry(cModule *networkNode, InterfaceEntry *interfaceEntry) override;
    virtual void updateNetworkAddress(cModule *networkNode, InterfaceEntry *interfaceEntry, CacheEntry *cacheEntry) override;
};

} // namespace visualizer

} // namespace inet

#endif // ifndef __INET_NETWORKADDRESSCANVASVISUALIZER_H

