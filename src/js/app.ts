"use strict";

// var xhrRequest = function (url, type, callback) {
//     var xhr = new XMLHttpRequest();
//     xhr.onload = function () {
//         callback(this.responseText);
//     };
//
//     xhr.open(type, url);
//     xhr.send();
// }
//
// function calculateDistanceSqr(x1, y1, x2, y2) {
//     return (x2-x1)*(x2-x1) + (y2-y1)*(y2-y1);
// }
//
// function getNearestStop(pos) {
//     var closestStop = { id:null,
//         name:null,
//         distanceSqr:null
//     };
//
//     for (let i = 0; i < stops.length; i++) {
//         var stop = stops[i];
//
//         var distSqr = calculateDistanceSqr(stop.lat, stop.lng, pos.coords.latitude, pos.coords.longitude);
//         if (!closestStop.id || distSqr < closestStop.distanceSqr) {
//             closestStop = { id: stop.id, name: stop.n, distanceSqr: distSqr };
//         }
//     }
//
//     return "cur:["+pos.coords.latitude+","+pos.coords.longitude+"] " + closestStop.name;
// }

// function locationSuccess(pos) {
//     var nearestStop = getNearestStop(pos);
//     console.log(nearestStop);
//
//     var dictionary = {
//         "nearestStopName": nearestStop
// //        "longf": pos.coords.longitude,
// //        "latf": pos.coords.latitude,
//     };
//
//     Pebble.sendAppMessage(dictionary);
// }

// function locationError(err) {
//
// }

// function getLocation() {
//     navigator.geolocation.getCurrentPosition(
//         locationSuccess,
//         locationError,
//         { timeout: 15000, maximumAge: 60000 }
//     );
// }

class Vehicle {
    public idx: number = 0;
    public title: string = null;
    public arrivesIn: number;
}

class Stop {
    public stopName: string;
    public distance:number = 0;
    public vehicles: Vehicle[] = [];
    public idx: number = 0;
}

class Stops {
    public stops: Stop[] = [];
}

function getNearestStopsAndVehiclesInfo(): Stops {
    "use strict";
    var stops = new Stops();

    // stop A
    var stop = new Stop();
    stop.stopName = "A";
    stop.distance = 300;

    var vehicle1 = new Vehicle();
    vehicle1.title = "Tram 15";
    vehicle1.arrivesIn = 1;
    stop.vehicles.push(vehicle1);

    var vehicle2 = new Vehicle();
    vehicle2.title = "Tram 18";
    vehicle2.arrivesIn = 8;

    stop.vehicles.push(vehicle2);
    stops.stops.push(stop);

    // stop A
    var stop2 = new Stop();
    stop2.stopName = "B";
    stop2.distance = 350;

    var vehicle3 = new Vehicle();
    vehicle3.title = "Tram 1";
    vehicle3.arrivesIn = 3;
    stop2.vehicles.push(vehicle3);

    stops.stops.push(stop2);

    return stops;
}

function sendNearestStopsAndVehiclesInfo() {
    var info = getNearestStopsAndVehiclesInfo();
    var serialized = serialize(info);
    console.log(JSON.stringify(serialized));
    Pebble.sendAppMessage(serialized, () => {}, () => {});
}

function serialize(info: Stops){
    // { "stops": [101, 102],
    //    101: "Stop A|300m|id103|id104",
    //    102: "Stop B|350m|id105|id106",
    //    103: "Tram 15|3min",
    //    104: "Tram 18|5min"
    // }

    var startIdx = 101;
    var idx = startIdx;
    var res = { stops: []};

    // fill stops pointers into result
    info.stops.forEach(stop => {
        stop.idx = idx++;
        res.stops.push(stop.idx);
    });

    info.stops.forEach(stop => {
        stop.vehicles.forEach(veh => {
           veh.idx = idx++;
        });
        let serializedStop: string = `${stop.stopName}|${stop.distance}`;
        if (stop.vehicles.length > 0) {
            let vehiclesIdxs: string = stop.vehicles.map(veh => veh.idx).join("|");
            serializedStop = serializedStop.concat("|", vehiclesIdxs);
        }
        res[stop.idx] = serializedStop;

        stop.vehicles.forEach(veh => {
            res[veh.idx] = `${veh.title}|${veh.arrivesIn}`;
        });
    });

    return res;
}

Pebble.addEventListener("ready",
    function(e) {
        console.log("PebbleKit JS ready!");

        sendNearestStopsAndVehiclesInfo();
    });

Pebble.addEventListener("appmessage",
    function(e) {
        console.log("message received!");
    });
