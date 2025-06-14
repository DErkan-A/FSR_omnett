package fsr_omnett;

import fsr_omnett.Fsr;
import inet.node.inet.AdhocHost;

module FsrHost extends AdhocHost
{
    submodules:
        fsr: Fsr {
            @display("p=250,100");
        }
        
	connections:
        fsr.socketOut --> at.in++;
        fsr.socketIn <-- at.out++;
}
