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
    initVU();
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
    document.getElementById('led_dir_1').className = data.led1;
    document.getElementById('led_dir_2').className = data.led2;
    document.getElementById('led_dir_3').className = data.led3;
    document.getElementById('led_dir_4').className = data.led4;
    document.getElementById('led_dir_5').className = data.led5;
    document.getElementById('led_dir_6').className = data.led6;
    document.getElementById('led_dir_7').className = data.led7;
    document.getElementById('led_dir_8').className = data.led8;
    document.getElementById('reflected').setAttribute('data-val', data.stat_vu);
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
