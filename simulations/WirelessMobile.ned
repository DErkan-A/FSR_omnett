package fsr_omnett.simulations;

import inet.networklayer.configurator.ipv4.Ipv4NetworkConfigurator;
import inet.node.contract.INetworkNode;
import inet.physicallayer.wireless.common.contract.packetlevel.IRadioMedium;
import inet.visualizer.contract.IIntegratedVisualizer;
import inet.environment.common.PhysicalEnvironment;

network WirelessMobile
{
    parameters:
        int numHosts = default(10);              // changeable in .ini
        @display("bgb=1000,1000;bgg=1000,1000,grey95");
        //--- packets received indicator in upper right ---
        @figure[rcvdPkText](
            type=indicatorText;
            pos=995,5;
            anchor=ne;             
            font=,18;
            textFormat="packets received: %g";
            initialValue=0
        );
        @statistic[packetReceived](
            source=host[1].app[0].packetReceived;
            record=figure(count);
            targetFigure=rcvdPkText
        );


    submodules:
        visualizer: <default(firstAvailableOrEmpty("IntegratedCanvasVisualizer"))>
            like IIntegratedVisualizer {
            @display("p=900,500");
        }
        configurator: Ipv4NetworkConfigurator {
            @display("p=900,600");
        }
        radioMedium: <default("UnitDiskRadioMedium")>
            like IRadioMedium {
            @display("p=900,700");
        }
        physicalEnvironment: PhysicalEnvironment {
            @display("p=900,800");
        }
        host[numHosts]: <default("FsrHost")> like INetworkNode {
        @display("p=100,100");  // or omit if you don’t need a fixed display
    }
}
