var socket = new WebSocket('ws://' + window.location.hostname + ':81/');
var isRelayOn = false;

document.getElementById('toggleButton').addEventListener('click', function() {
    isRelayOn = !isRelayOn;
    socket.send(isRelayOn ? '1' : '0');
    console.log(isRelayOn ? '1' : '0')
});

socket.onmessage = function(event) {
    console.log('Message from server:', event.data);
};