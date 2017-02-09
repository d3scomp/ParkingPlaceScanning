//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
//

package cz.cuni.d3s.parkingplacescanning.modules.world;

import org.car2x.veins.base.connectionManager.ConnectionManager;
import org.car2x.veins.base.modules.BaseWorldUtility;
import org.car2x.veins.modules.mobility.traci.TraCIScenarioManagerForker;
import org.car2x.veins.modules.obstacle.ObstacleControl;
import org.car2x.veins.modules.world.annotations.AnnotationManager;

import inet.networklayer.autorouting.ipv4.IPv4NetworkConfigurator;
import inet.nodes.inet.AdhocHost;
import inet.world.radio.ChannelControl;
import inet.nodes.inet.Router;
import inet.nodes.inet.StandardHost;
import inet.nodes.ethernet.Eth10G;
import inet.networklayer.ipv4.RoutingTableRecorder;

import lte.corenetwork.binder.*;
import lte.corenetwork.nodes.*;

network ParkingPlaceScanningScenario {
  parameters:
        double playgroundSizeX @unit(m); // x size of the area the nodes are in (in meters)
        double playgroundSizeY @unit(m); // y size of the area the nodes are in (in meters)
        double playgroundSizeZ @unit(m); // z size of the area the nodes are in (in meters)
        @display("bgb=2600,2000");
        int numUe = default(100);

    submodules:
        obstacles: ObstacleControl {
            @display("p=261,35");
        }
        annotations: AnnotationManager {
            @display("p=485,35");
        }
        connectionManager: ConnectionManager {
            parameters:
                @display("p=146,31;i=abstract/multicast");
        }
        world: BaseWorldUtility {
            parameters:
                playgroundSizeX = playgroundSizeX;
                playgroundSizeY = playgroundSizeY;
                playgroundSizeZ = playgroundSizeZ;
                @display("p=34,31;i=misc/globe");
        }
        manager: TraCIScenarioManagerForker {
            parameters:
                @display("p=34,206");
        }

        // needed for INET
        channelControl: ChannelControl {
            @display("p=361,34");
        }
        configurator: IPv4NetworkConfigurator {
            @display("p=34,97");
        }
        // LTE stuff
        binder: LteBinder {
            @display("p=34,152;is=s");
        }
        eNodeB1: eNodeB {
            @display("p=1490,950;is=vl");
        }
//        eNodeB2: eNodeB {
//            @display("p=1240,850;is=vl");
//        }
//        eNodeB3: eNodeB {
//            @display("p=1240,1000;is=vl");
 //       }
        server: StandardHost {
            @display("p=1460,940;is=n;i=device/server");
        }
//        server2: StandardHost {
//            @display("p=1160,840;is=n;i=device/server");
//       }
//        server3: StandardHost {
//            @display("p=1260,1140;is=n;i=device/server");
//        }
        routingRecorder: RoutingTableRecorder {
            @display("p=34,274;is=s");
        }
    connections allowunconnected:
        server.pppg++ <--> Eth10G <--> eNodeB1.ppp;
//		server2.pppg++ <--> Eth10G <--> eNodeB2.ppp;
//		server3.pppg++ <--> Eth10G <--> eNodeB3.ppp;
}
