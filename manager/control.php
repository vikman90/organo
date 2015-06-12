<?php
/**
 * Created by PhpStorm.
 * User: Vikman
 * Date: 12/05/2015
 * Time: 12:39
 */

namespace manager;
require_once 'lib/database.php';

if (!isset($_GET['action']))
    html_error('args');

switch ($_GET['action']) {
    case 'logout':
        logout();
        break;
    case 'language':
        language();
        break;
    case 'shutdown':
        shutdown();
        break;
    case 'reboot':
        reboot();
        break;
    case 'login':
        login();
        break;
    case 'play':
        play();
    case 'new_playlist':
        new_playlist();
        break;
    case 'rename_playlist':
        rename_playlist();
        break;
    case 'delete_playlist':
        delete_playlist();
        break;
    case 'new_score':
        new_score();
        break;
    default:
        html_error('args');
}

function logout() {
    unset_auth();
    html_redirect('index.php');
}

function language() {
    global $translators;

    if (!isset($_GET['code']))
        html_error('args');

    foreach ($translators as $t) {
        if ($t->code == $_GET['code']) {
            $_SESSION['lang'] = $t->code;
            html_redirect(last_page());
        }
    }

    html_error('lang_not_found');
}

function shutdown() {
    global $tr;

    test_auth();
    exec('/usr/bin/sudo /sbin/shutdown now 2>&1', $output, $retval);

    if ($retval == 0) {
        html_redirect('index.php?view=shutdown');
    } else
        html_error('shutting_down');
}

function reboot() {
    global $tr;

    test_auth();
    exec('/usr/bin/sudo /sbin/shutdown -r now 2>&1', $output, $retval);

    if ($retval == 0) {
        html_redirect('index.php?view=reboot');
    } else
        html_error('shutting_down');
}

function login() {
    if (!isset($_POST['password']))
        html_error('args');

    $user = LINUX_USERNAME;
    $pass = $_POST['password'];

    exec("/usr/bin/sudo /usr/bin/python3 /home/pi/player/login.py $user $pass", $output, $retval);

    if (!$retval) {
        set_auth();
        html_redirect('player.php');
    } else
        html_redirect('index.php?error=1');
}

function play() {
    html_redirect('player.php');
}

function new_playlist() {
    if (!isset($_POST['name']))
        html_error('args');

    db_insert_playlist($_POST['name']);
    html_redirect('playlists.php');
}

function rename_playlist() {
    if (!(isset($_POST['name']) and isset($_POST['idplaylist'])))
        html_error('args');

    if (!db_get_playlist($_POST['idplaylist']))
        html_error('args');

    db_rename_playlist($_POST['idplaylist'], $_POST['name']);
    html_redirect('playlist.php?idplaylist=' . $_POST['idplaylist']);
}

function delete_playlist() {
    if (!isset($_POST['idplaylist']))
        html_error('args');

    $playlist = db_get_playlist($_POST['idplaylist']);

    if (!$playlist)
        html_error('args');

    foreach ($playlist['scores'] as $score) {
        db_delete_score($score['id']);
        unlink(SCORE_DIR . '/' . $score['source']);
    }

    db_delete_playlist($playlist['id']);
    html_redirect('playlists.php');
}

function new_score() {
    if (!isset($_POST['idplaylist']))
        html_error('args');
    
    if (!isset($_FILES['score']))
        html_error('file_type');

    $file = $_FILES['score'];
    $ext = pathinfo($file['name'], PATHINFO_EXTENSION);

    if (!db_get_playlist($_POST['idplaylist']))
        html_error('args');

    if (!($ext == 'mid' or $ext == 'midi'))
        html_error('file_type');

    if ($file['size'] > FILE_MAXSIZE)
        html_error('file_size');

    $score = db_insert_score($_POST['idplaylist'], $file['name']);
    move_uploaded_file($file['tmp_name'], SCORE_DIR . '/' . $score['source']);
    html_redirect("playlist.php?idplaylist={$_POST['idplaylist']}");
}
