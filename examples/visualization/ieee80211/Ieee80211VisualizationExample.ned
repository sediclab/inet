package inet.examples.visualization.ieee80211;

import inet.visualizer.integrated.IntegratedVisualizer;

network Ieee80211VisualizationExample
{
    parameters:
        @figure[description](type=text; pos=300,10; font=Arial,20; text="TODO");
    submodules:
        visualizer: IntegratedVisualizer {
            parameters:
                @display("p=100,50");
        }
}
