/**
 * Created by Vikman on 08/06/2015.
 */

function play() {
    btPlay = document.getElementById('bt-play');
    btPause = document.getElementById('bt-pause');

    btPlay.style.display = 'none';
    btPause.style.display = 'inline';
}

function pause() {
    btPlay = document.getElementById('bt-play');
    btPause = document.getElementById('bt-pause');

    btPlay.style.display = 'inline';
    btPause.style.display = 'none';
}
