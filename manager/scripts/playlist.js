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
    var types = event.dataTransfer.types;

    for (var i = 0; i < types.length; i++) {
        if (types[i] == "Files") {
            event.preventDefault();
            document.getElementById('playlist').className = 'dragging';
            return;
        }
    }
}

function dragstop() {
    document.getElementById('playlist').className = '';
}

function drop(event, div) {
    var idplaylist = div.getAttribute("data-idplaylist");
    var dialog = document.getElementById('dialog-uploading');

    dialog.style.display = 'block';
    event.preventDefault();
    dragstop();

    send(event.dataTransfer.files, idplaylist, 0);
}

function send(files, idplaylist, i) {
    var curFile = document.getElementById("current_file");

    if (i == files.length) {
        location.reload();
    }
    else {
        var request = new XMLHttpRequest();
        var data = new FormData();

        data.append("idplaylist", idplaylist);
        data.append("score", files[i]);
        curFile.innerHTML = files[i].name;

        request.onreadystatechange = function() {
            if (request.readyState == 4 && request.status == 200)
                send(files, idplaylist, i + 1);
        }

        request.open("POST", "control.php?action=new_score");
        request.send(data);
    }
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
    input.focus();
}

function deletePlaylist() {
    var dialog = document.getElementById('dialog-delete-playlist');
    dialog.style.display = 'block';
}

function renameScore(event, td) {
    var dialog = document.getElementById('dialog-rename-score');
    var idscore = document.getElementById('input-rename-idscore');
    var scorename = document.getElementById('input-scorename');
    var row = td.parentElement.parentElement;
    var name = row.children[1].firstChild.nodeValue;

    idscore.value = row.getAttribute('data-idscore');
    scorename.value = name;
    dialog.style.display = 'block';
    scorename.focus();
    event.stopPropagation();
}

function deleteScore(event, td) {
    var dialog = document.getElementById('dialog-delete-score');
    var idscore = document.getElementById('input-delete-idscore');
    var row = td.parentElement.parentElement;

    idscore.value = row.getAttribute('data-idscore');
    dialog.style.display = 'block';
    event.stopPropagation();
}

function closeModal() {
    var dialogs = document.getElementsByClassName('modal');

    for (var i = 0; i < dialogs.length; i++)
        dialogs[i].style.display = 'none';
}
