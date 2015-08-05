/**
 * Created by Vikman on 08/06/2015.
 */

function resume() {
    var btPlay = document.getElementById('bt-play');
    var btPause = document.getElementById('bt-pause');
    var request = new XMLHttpRequest();

    request.onreadystatechange = function() {
        if (request.readyState == 4 && request.status == 200) {
            if (request.responseText == 'OK') {
                btPlay.style.display = 'none';
                btPause.style.display = 'inline';
            }
        }
    };

    request.open('GET', 'ajax/resume.php');
    request.send();
}

function pause() {
    var btPlay = document.getElementById('bt-play');
    var btPause = document.getElementById('bt-pause');
    var request = new XMLHttpRequest();

    request.onreadystatechange = function() {
        if (request.readyState == 4 && request.status == 200) {
            if (request.responseText == 'OK') {
                btPlay.style.display = 'inline';
                btPause.style.display = 'none';
            }
        }
    };

    request.open('GET', 'ajax/pause.php');
    request.send();
}

function stop() {
    var btPlay = document.getElementById('bt-play');
    var btPause = document.getElementById('bt-pause');
    var btStop = document.getElementById('bt-stop');
    var btPrevious = document.getElementById('bt-previous');
    var btNext = document.getElementById('bt-next');
    var request = new XMLHttpRequest();

    request.onreadystatechange = function() {
        if (request.readyState == 4 && request.status == 200) {
            if (request.responseText == 'OK') {
                btPlay.style.display = 'inline';
                btPause.style.display = 'none';
                btPlay.setAttribute('disabled', '');
                btPause.setAttribute('disabled', '');
                btStop.setAttribute('disabled', '');
                btPrevious.setAttribute('disabled', '');
                btNext.setAttribute('disabled', '');
            }
        }
    };

    request.open('GET', 'ajax/stop.php');
    request.send();
}

function previous() {
    var playlist = document.getElementById('current-playlist');
    var selected = null;
    var title = document.getElementById('score-name');
    var btPlay = document.getElementById('bt-play');
    var btPause = document.getElementById('bt-pause');

    for (var i = 0; i < playlist.children.length; i++) {
        if (playlist.children[i].className == 'selected') {
            playlist.children[i].className = '';
            selected = playlist.children[(i - 1 + playlist.children.length) % playlist.children.length];
            break;
        }
    }

    if (selected === null)
        selected = playlist.firstElementChild;

    selected.className = 'selected';
    title.innerHTML = selected.innerHTML;
}

function next() {
    var playlist = document.getElementById('current-playlist');
    var selected = null;
    var title = document.getElementById('score-name');
    var btPlay = document.getElementById('bt-play');
    var btPause = document.getElementById('bt-pause');

    for (var i = 0; i < playlist.children.length; i++) {
        if (playlist.children[i].className == 'selected') {
            playlist.children[i].className = '';
            selected = playlist.children[(i + 1) % playlist.children.length];
            break;
        }
    }

    if (selected === null)
        selected = playlist.firstElementChild;

    selected.className = 'selected';
    title.innerHTML = selected.innerHTML;
    btPlay.style.display = 'none';
    btPause.style.display = 'inline';
}

function select(selected) {
    var playlist = document.getElementById('current-playlist');
    var title = document.getElementById('score-name');
    var btPlay = document.getElementById('bt-play');
    var btPause = document.getElementById('bt-pause');

    for (var i = 0; i < playlist.children.length; i++) {
        if (playlist.children[i].className == 'selected') {
            playlist.children[i].className = '';
            break;
        }
    }

    selected.className = 'selected';
    title.innerHTML = selected.innerHTML;
    btPlay.style.display = 'none';
    btPause.style.display = 'inline';
}
