/**
 * ----------------------------------------------------------------------------
 * ESP32 Remote Control with WebSocket
 * ----------------------------------------------------------------------------
 * © 2020 Stéphane Calderoni
 * ----------------------------------------------------------------------------
 */

//var gateway = `ws://${window.location.hostname}/ws`;
var gateway = `ws://10.101.29.204/ws`;
var websocket;

// ----------------------------------------------------------------------------
// Initialization
// ----------------------------------------------------------------------------

window.addEventListener('load', onLoad);

function onLoad(event) {
    initWebSocket();
    initButton();
    //initVU();
}

// ----------------------------------------------------------------------------
// WebSocket handling
// ----------------------------------------------------------------------------

function initWebSocket() {
    console.log('Trying to open a WebSocket connection...');
    websocket = new WebSocket(gateway);
    websocket.onopen    = onOpen;
    websocket.onclose   = onClose;
    websocket.onmessage = onMessage;
}

function onOpen(event) {
    console.log('Connection opened');
}

function onClose(event) {
    console.log('Connection closed');
    setTimeout(initWebSocket, 2000);
}

function onMessage(event) {
    let data = JSON.parse(event.data);
    document.getElementById('led').className = data.stat;

    //let index = 4;
    //document.getElementById('led_dir_'+index).className = "on";
    for (let index = 1; index <= 8; index++) {
        if (index == data.dir) {
            document.getElementById('led_dir_'+index).className = "on";
        } else {
            document.getElementById('led_dir_'+index).className = "off";
        }
    }
}

// ----------------------------------------------------------------------------
// Button handling
// ----------------------------------------------------------------------------

function initButton() {
    document.addEventListener("click", onToggle, false);
}

function onToggle(event) {
    event = event || window.event;
    event.target = event.target || event.srcElement;
    event.text = event.target.textContent || event.target.innerText;
    websocket.send(JSON.stringify({'action':event.text}));
}
