var keyboardEventsArePresent = true;

var removeKeyboardEvents = function () {
    if (keyboardEventsArePresent) {
        window.removeEventListener("keydown", GLFW.onKeydown, true);
        window.removeEventListener("keypress", GLFW.onKeyPress, true);
        window.removeEventListener("keyup", GLFW.onKeyup, true);
        keyboardEventsArePresent = false;
    }
};

var addKeyboardEvents = function () {
    if (!keyboardEventsArePresent) {
        window.addEventListener("keydown", GLFW.onKeydown, true);
        window.addEventListener("keypress", GLFW.onKeyPress, true);
        window.addEventListener("keyup", GLFW.onKeyup, true);
        keyboardEventsArePresent = true;
    }
    // enforce several frames to toggle animation when popup closed
    for (var i = 0;i<500;i+=100)
        setTimeout(function () { Module.ccall('emsPostEmptyEvent', 'void', ['number'], [1]); }, i);
};

var createPopup = function (closeId, label, width, height, center = true) {
    var popup = document.createElement('div');

    popup.addEventListener('touchmove', function (event) {
        if (event.scale !== 1) { event.preventDefault(); }
    }, false);
    var lastTouchEnd = 0;
    popup.addEventListener('touchend', function (event) {
        var now = (new Date()).getTime();
        if (now - lastTouchEnd <= 300) {
            event.preventDefault();
        }
        lastTouchEnd = now;
    }, false);

    var positionStyle = 'top:50%;left:50%;transform:translate(-50%,-50%);';

    if (!center)
        positionStyle = 'top:100%;left:50%;transform:translate(-50%,-100%);border-top:solid 1px #ffffff33;border-left:solid 1px #ffffff33;border-right:solid 1px #ffffff33;';
    popup.setAttribute('style', 'width:' + width + 'px;height:' + height + 'px;border-radius: 4px;background:#1c1f24;position:absolute;' + positionStyle);


    var close = document.createElement('div');
    close.setAttribute('class', 'unselectable');
    close.innerHTML = '&#x2715;';
    close.setAttribute('style', 'margin-left:' + (width - 26) + 'px;width:24;height:24px;margin-top:7px;cursor:pointer;color:#eef0f5;display: inline-block;');
    close.setAttribute('onclick', 'document.getElementById(\'' + closeId + '\').remove();addKeyboardEvents()');

    popup.appendChild(close);

    if (label !== "") {
        var text = document.createElement('span');
        text.setAttribute('class', 'unselectable');
        text.innerHTML = label;
        text.setAttribute('style', 'font-size: 20px;  font-weight: bold;  font-stretch: normal;  font-style: normal;  line-height: normal;  letter-spacing: normal;  color: #fff;position:absolute;top:50%;left:50%;transform:translate(-50%,' + (-height / 2 + 2 * 20) + 'px);');
        popup.appendChild(text);
    }

    return popup;
}

var createOverlayPopup = function (id, label, width, height, center = true) {
    var overlay = document.createElement('div');
    overlay.setAttribute('style', 'position:absolute;top:0;right:0;bottom:0;left:0;background-color: rgba(0,0,0,0.8);z-index:9999;');
    overlay.setAttribute('id', id);
    overlay.addEventListener('touchmove', function (event) {
        if (event.scale !== 1) { event.preventDefault(); }
    }, false);
    var lastTouchEnd = 0;
    overlay.addEventListener('touchend', function (event) {
        var now = (new Date()).getTime();
        if (now - lastTouchEnd <= 300) {
            event.preventDefault();
        }
        lastTouchEnd = now;
    }, false);

    var popup = createPopup(id, label, width, height, center);
    overlay.appendChild(popup);

    return { overlay, popup };
}
