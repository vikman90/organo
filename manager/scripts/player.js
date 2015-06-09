/**
 * Created by Vikman on 08/06/2015.
 */

function play() {
    var btPlay = document.getElementById('bt-play');
    var btPause = document.getElementById('bt-pause');
    var selected = null;

    for (var i = 0; i < playlist.children.length; i++) {
        if (playlist.children[i].className == 'selected') {
            playlist.children[i].className = '';
            selected = playlist.children[(i + 1) % playlist.children.length];
            break;
        }
    }

    btPlay.style.display = 'none';
    btPause.style.display = 'inline';
}

function pause() {
    var btPlay = document.getElementById('bt-play');
    var btPause = document.getElementById('bt-pause');

    btPlay.style.display = 'inline';
    btPause.style.display = 'none';
}

function stop() {
    var btPlay = document.getElementById('bt-play');
    var btPause = document.getElementById('bt-pause');
    btPlay.style.display = 'inline';
    btPause.style.display = 'none';
}

function previuos() {
    var playlist = document.getElementById('current-playlist');
    var selected = null;
    var title = document.getElementById('part-name');
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
    var title = document.getElementById('part-name');
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

function select(idpart) {
    var playlist = document.getElementById('current-playlist');
    var selected = document.getElementById('part-' + idpart);
    var title = document.getElementById('part-name');
    var btPlay = document.getElementById('bt-play');
    var btPause = document.getElementById('bt-pause');

    for (var i = 0; i < playlist.children.length; i++)
        playlist.children[i].className = '';

    selected.className = 'selected';
    title.innerHTML = selected.innerHTML;
    btPlay.style.display = 'none';
    btPause.style.display = 'inline';
}
