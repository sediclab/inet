//
// Copyright (C) 2014 OpenSim Ltd.
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

#ifndef __INET_MQAMMODULATION_H
#define __INET_MQAMMODULATION_H

#include "inet/physicallayer/base/MQAMModulationBase.h"

namespace inet {

namespace physicallayer {

/**
 * This class implements gray coded rectangular quadrature amplitude modulation
 * that arranges symbols evenly.
 *
 * http://en.wikipedia.org/wiki/Quadrature_amplitude_modulation#Rectangular_QAM
 */
class INET_API MQAMModulation : public MQAMModulationBase
{
    public:
        MQAMModulation(unsigned int codeWordSize);
        virtual ~MQAMModulation();

        virtual void printToStream(std::ostream &stream) const;
};

} // namespace physicallayer

} // namespace inet

#endif // ifndef __INET_MQAMMODULATION_H
