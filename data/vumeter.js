// https://github.com/tomnomnom/vumeter

function vumeter(elem, config){

    // Settings
    var max             = config.max || 4095;
    var boxCount        = config.boxCount || 15;
    var boxCountRed     = config.boxCountRed || 2;
    var boxCountYellow  = config.boxCountYellow || 3;
    var boxGapFraction  = config.boxGapFraction || 0.25;

    // Colours
    var redOn     = 'rgba(255,47,30,0.9)';
    var redOff    = 'rgba(64,12,8,0.9)';
    var yellowOn  = 'rgba(255,215,5,0.9)';
    var yellowOff = 'rgba(64,53,0,0.9)';
    var greenOn   = 'rgba(53,255,30,0.9)';
    var greenOff  = 'rgba(13,64,8,0.9)';

    // Derived and starting values
    var width = elem.width;
    var height = elem.height;
    var curVal = 0;

    // Gap between boxes and box height
    var boxWidth = width / (boxCount + (boxCount+1)*boxGapFraction);
    var boxGapX = boxWidth * boxGapFraction;

    var boxHeight = height - (boxGapX*2);
    var boxGapY = (height - boxHeight) / 2;

    // Canvas starting state
    var c = elem.getContext('2d');

    // Main draw loop
    var draw = function(){

        curVal = parseInt(elem.dataset.val, 10);
        c.save();
        c.beginPath();
        c.rect(0, 0, width, height);
        c.fillStyle = 'rgb(32,32,32)';
        c.fill();
        c.restore();
        drawBoxes(c, curVal);

        requestAnimationFrame(draw);
    };

    // Draw the boxes
    function drawBoxes(c, val){
        c.save(); 
        c.translate(boxGapX, boxGapY);
        for (var i = 0; i < boxCount; i++){
            
            c.beginPath();
            if (isOn(i, val)){
                c.shadowBlur = 10;
                c.shadowColor = getBoxColor(i+1, val);
            }
            c.rect(0, 0, boxWidth, boxHeight);
            c.fillStyle = getBoxColor(i+1, val);
            c.fill();
            c.translate(boxWidth + boxGapX, 0);
        }
        c.restore();
    }

    // Get the color of a box given it's ID and the current value
    function getBoxColor(id, val){
        // on colours
        if (id > boxCount - boxCountRed){
            return isOn(id, val)? redOn : redOff;
        }
        if (id > boxCount - boxCountRed - boxCountYellow){
            return isOn(id, val)? yellowOn : yellowOff;
        }
        return isOn(id, val)? greenOn : greenOff;
    }

    
    function isOn(id, val){
        // We need to scale the input value (0-max)
        // so that it fits into the number of boxes
        var maxOn = Math.ceil((val/max) * boxCount);
        return (id <= maxOn);
    }

    // Trigger the animation
    draw();
}
