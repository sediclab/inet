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

#ifndef __INET_IEEE80211CANVASVISUALIZER_H
#define __INET_IEEE80211CANVASVISUALIZER_H

#include "inet/visualizer/base/Ieee80211VisualizerBase.h"
#include "inet/visualizer/networknode/NetworkNodeCanvasVisualizer.h"

namespace inet {

namespace visualizer {

class INET_API Ieee80211CanvasVisualizer : public Ieee80211VisualizerBase
{
  protected:
    class INET_API CanvasCacheEntry : public CacheEntry {
      public:
        cIconFigure *figure = nullptr;
        NetworkNodeCanvasVisualization *visualization = nullptr;

      public:
        CanvasCacheEntry(cIconFigure *figure, NetworkNodeCanvasVisualization *visualization, int nodeId, int interfaceId);
    };

  protected:
    double zIndex = NaN;
    NetworkNodeCanvasVisualizer *networkNodeVisualizer = nullptr;

  protected:
    virtual void initialize(int stage) override;

    virtual CacheEntry *createCacheEntry(cModule *networkNode, InterfaceEntry *interfaceEntry) override;
    virtual void addCacheEntry(CacheEntry *cacheEntry) override;
    virtual void removeCacheEntry(CacheEntry *cacheEntry) override;
};

} // namespace visualizer

} // namespace inet

#endif // ifndef __INET_IEEE80211CANVASVISUALIZER_H

