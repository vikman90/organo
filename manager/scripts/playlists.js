/**
 * Created by Vikman on 10/06/2015.
 */

document.addEventListener('keydown', function(event) {
    if (event.keyCode == 27)
        closeDialog();
});

function add() {
    var dialog = document.getElementById('dialog');
    var name = document.getElementById('dialog-name');
    dialog.style.display = 'block';
    name.value = '';
    name.focus();
}

function list(tr) {
    window.location = 'playlist.php?idplaylist=' + tr.getAttribute('data-idplaylist');
}

function closeDialog() {
    document.getElementById('dialog').style.display = 'none';
}