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

#ifndef __INET_ROUTINGTABLECANVASVISUALIZER_H
#define __INET_ROUTINGTABLECANVASVISUALIZER_H

#include "inet/common/geometry/common/CanvasProjection.h"
#include "inet/visualizer/base/RoutingTableVisualizerBase.h"

namespace inet {

namespace visualizer {

class INET_API RoutingTableCanvasVisualizer : public RoutingTableVisualizerBase
{
  protected:
    class INET_API CanvasRoute : public Route {
      public:
        cLineFigure *figure = nullptr;

      public:
        CanvasRoute(cLineFigure *figure, int nodeModuleId, int nextHopModuleId);
        virtual ~CanvasRoute();
    };

  protected:
    double zIndex = NaN;
    const CanvasProjection *canvasProjection = nullptr;
    cGroupFigure *routeGroup = nullptr;

  protected:
    virtual void initialize(int stage) override;

    virtual void addRoute(std::pair<int, int> nodeAndNextHop, const Route *route) override;
    virtual void removeRoute(const Route *route) override;

    virtual void setPosition(cModule *node, const Coord& position) const override;
    virtual const Route *createRoute(cModule *node, cModule *nextHop) const override;
};

} // namespace visualizer

} // namespace inet

#endif // ifndef __INET_ROUTINGTABLECANVASVISUALIZER_H

