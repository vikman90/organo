/**
 * Created by Vikman on 10/06/2015.
 */

document.addEventListener('keydown', function(event) {
    if (event.keyCode == 27)
        closeDialog();
});

function add() {
    document.getElementById('dialog').style.display = 'block';
    document.getElementById('dialog-name').value = '';
}

function list(tr) {
    window.location = 'playlist.php?idplaylist=' + tr.getAttribute('data-idplaylist');
}

function closeDialog() {
    console.log('enro');
    document.getElementById('dialog').style.display = 'none';
}