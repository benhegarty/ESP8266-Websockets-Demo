// Connect to the socket. Same URL, port 81.
var Socket = new WebSocket('ws://' + window.location.hostname + ':81');

// When a new websockets message is recieved, redraw the dial with the updated value
Socket.onmessage = function (evt) {
  var thePercentage = parseInt(evt.data);
  drawDial(thePercentage, '#ab194f');
};

// Send degrees when the slider is slid
// NOTE: Logic below also prevents sending more than 1 message every 10ms. The ESP hates if you blast it with updates.
var lastSend = 0;
function sendDegrees(degrees) {
  var now = (new Date).getTime();
  if (lastSend > now - 20) return;
  lastSend = now;
  Socket.send(degrees);
}

// Draw the dial initially at 0
drawDial(0, '#ab194f');

// This function draws the dial based on a given percventage and color
function drawDial(percentage, color) {
  
  // First, we get a reference to the div in the HTML which we will draw the dial in
  var dialCanv = document.getElementById('dial');
  var ctx = dialCanv.getContext("2d");
  dialCanv.height = dialCanv.offsetHeight * 2;
  dialCanv.width = dialCanv.offsetWidth * 2;

  // Calculate the center of the div
  var centerX = dialCanv.width / 2;
  var centerY = dialCanv.height / 2;

  // Draw the colored arc showing the value
  ctx.beginPath();
  ctx.fillStyle = color;
  ctx.moveTo(centerX, centerY.height / 2);
  ctx.arc(centerX, centerY, centerY*0.8, Math.PI * 1.5, (Math.PI * 2 * (percentage / 100)) + (Math.PI * 1.5), false);
  ctx.lineTo(centerX, centerY);
  ctx.fill();
  ctx.closePath();

  // Draw the white background circle
  ctx.beginPath();
  ctx.fillStyle = "white";
  ctx.arc(centerX, centerY, centerY*0.65, 0, Math.PI * 2, false);
  ctx.fill();
  ctx.closePath();

  // Add label
  ctx.font = "bold 90px sans-serif";
  ctx.fillStyle = color;
  ctx.textBaseline = "center";
  ctx.textAlign = "center";
  ctx.fillText(parseInt(percentage), centerX, centerY * 1.1);
}