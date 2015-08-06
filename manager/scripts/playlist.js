/**
 * Created by Vikman on 11/06/2015.
 */

document.addEventListener('keydown', function(event) {
    if (event.keyCode == 27)
        closeModal();
});

function add() {
    document.getElementById('input-score').click()
}

function submit() {
    document.getElementById('form-score').submit();
}

function drag(event) {
    event.preventDefault();
    document.getElementById('playlist').className = 'dragging';
}

function dragstop() {
    document.getElementById('playlist').className = '';
}

function drop(event) {
    var input = document.getElementById('input-score');
    var form = document.getElementById('form-score');

    event.preventDefault();
    input.files = event.dataTransfer.files;
    form.submit();
    dragstop();
}

function play(score) {
    var playlist = document.getElementById('playlist');
    window.location = 'control.php?action=play&idplaylist=' + playlist.getAttribute('data-idplaylist') + '&idscore=' + score.getAttribute('data-idscore');
}

function renamePlaylist() {
    var dialog = document.getElementById('dialog-rename-playlist');
    var name = document.getElementById('plname');
    var input = document.getElementById('input-plname');

    input.value = name.innerHTML;
    dialog.style.display = 'block';
}

function deletePlaylist() {
    var dialog = document.getElementById('dialog-delete-playlist');
    dialog.style.display = 'block';
}

function renameScore(id, name) {
    var dialog = document.getElementById('dialog-rename-score');
    var idscore = document.getElementById('input-rename-idscore');
    var scorename = document.getElementById('input-scorename');
    idscore.value = id;
    scorename.value = name;
    dialog.style.display = 'block';
}

function deleteScore(id) {
    var dialog = document.getElementById('dialog-delete-score');
    var idscore = document.getElementById('input-delete-idscore');
    idscore.value = id;
    dialog.style.display = 'block';
}

function closeModal() {
    var dialogs = document.getElementsByClassName('modal');

    for (var i = 0; i < dialogs.length; i++)
        dialogs[i].style.display = 'none';
}
