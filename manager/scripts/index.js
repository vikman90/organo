/**
 * Created by Vikman on 11/06/2015.
 */

IMAGES = 2;
TIME = 10000;
PREFIX = 'url(images/background-';
SUFFIX = '.jpg)';

var i = 0;
setTimeout('toggle()', TIME);

function toggle() {
    i = (i + 1) % IMAGES;
    document.getElementById('login').style.backgroundImage = PREFIX + (i + 1) + SUFFIX;
    setTimeout('toggle()', TIME);
}
