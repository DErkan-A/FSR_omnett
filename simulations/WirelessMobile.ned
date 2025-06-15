package fsr_omnett.simulations;

import inet.networklayer.configurator.ipv4.Ipv4NetworkConfigurator;
import inet.node.contract.INetworkNode;
import inet.physicallayer.wireless.common.contract.packetlevel.IRadioMedium;
import inet.visualizer.contract.IIntegratedVisualizer;
import inet.environment.common.PhysicalEnvironment;

import fsr_omnett.FsrHost;

network WirelessMobile
{
    parameters:
        int numHosts = default(10);              // changeable in .ini
        @display("bgb=1000,1000;bgg=1000,1000,grey95");
        //--- packets received indicator in upper right ---

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
        }
        
        hostA: <default("FsrHost")> like INetworkNode {
        }
        hostB: <default("FsrHost")> like INetworkNode {
        }
}
