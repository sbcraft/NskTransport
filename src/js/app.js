"use strict";

console.log("js started");

var xhrRequest = function (url, type, callback) {
    var xhr = new XMLHttpRequest();
    xhr.onload = function () {
        callback(this.responseText);
    };
    
    xhr.open(type, url);
    xhr.send();
}

function getNearestStop(pos) {
    var dictionary = {
        "long": pos.coords.longitude,
        "lat": pos.coords.latitude
    };
    console.log(typeof(pos.coords.longitude) );
    console.log(dictionary["long"]);
    console.log(dictionary["lat"]);
    
    Pebble.sendAppMessage(dictionary);
}

function locationSuccess(pos) {
    console.log(pos);
    getNearestStop(pos);
}

function locationError(err) {
    
}

function getLocation() {
    navigator.geolocation.getCurrentPosition(
        locationSuccess,
        locationError,
        { timeout: 15000, maximumAge: 60000 }
    );
}

Pebble.addEventListener("ready", 
                       function(e) {
                           console.log("PebbleKit JS ready!");
                           
                           // Get initial state
                           getLocation();
                       });

Pebble.addEventListener("appmessage",
                       function(e) {
                           console.log("message received!");
                       });

