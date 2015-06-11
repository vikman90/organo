/**
 * Created by Vikman on 11/06/2015.
 */

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
