/**
 * Created by Vikman on 15/06/2015.
 */

document.addEventListener('keydown', function(event) {
    if (event.keyCode == 27)
        closeModal();
});

function shutdown() {
    document.getElementById('dialog-shutdown').style.display = 'block';
}

function reboot() {
    document.getElementById('dialog-reboot').style.display = 'block';
}

function closeModal() {
    var dialogs = document.getElementsByClassName('modal');

    for (var i = 0; i < dialogs.length; i++)
        dialogs[i].style.display = 'none';
}