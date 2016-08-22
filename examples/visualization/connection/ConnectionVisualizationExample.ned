//
// This program is property of its copyright holder. All rights reserved.
//

package inet.examples.visualization.connection;

import inet.visualizer.integrated.IntegratedVisualizer;

network ConnectionVisualizationExample
{
    parameters:
        @figure[description](type=text; pos=300,10; font=Arial,20; text="TODO");
    submodules:
        visualizer: IntegratedVisualizer {
            parameters:
                @display("p=100,50");
        }
}
