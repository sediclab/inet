package inet.examples.visualization.networkaddress;

import inet.visualizer.integrated.IntegratedVisualizer;

network NetworkAddressVisualizationExample
{
    parameters:
        @figure[description](type=text; pos=300,10; font=Arial,20; text="TODO");
    submodules:
        visualizer: IntegratedVisualizer {
            parameters:
                @display("p=100,50");
        }
}
