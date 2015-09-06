<?php
/**
 * Created by PhpStorm.
 * User: Vikman
 * Date: 12/05/2015
 * Time: 12:39
 */

namespace manager;

require_once 'lib/templates.php';
require_once 'lib/database.php';
require_once 'lib/driver.php';

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
        break;
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
    case 'rename_score':
        rename_score();
        break;
    case 'delete_score':
        delete_score();
        break;
    case 'set_shortcut':
        set_shortcut();
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
            set_language($t->code);
            html_redirect(last_page());
        }
    }

    html_error('lang_not_found');
}

function shutdown() {
    global $tr;

    test_auth();
    exec(EXEC_SHUTDOWN, $output, $retval);

    if ($retval == 0) {
        html_redirect('index.php?view=shutdown');
    } else
        html_error('shutting_down');
}

function reboot() {
    global $tr;

    test_auth();
    exec(EXEC_REBOOT, $output, $retval);

    if ($retval == 0) {
        html_redirect('index.php?view=reboot');
    } else
        html_error('shutting_down');
}

function login() {
    if (!isset($_POST['password']))
        html_error('args');

    $user = LINUX_USERNAME;
    $pass = escapeshellarg($_POST['password']);
    exec(EXEC_LOGIN . " $user $pass", $output, $retval);

    if (!$retval) {
        set_auth();
        html_redirect('player.php');
    } else
        html_redirect('index.php?error=1');
}

function play() {
    test_auth();

    if (!isset($_GET['idplaylist']))
        html_error('args');

    $playlist = db_get_playlist($_GET['idplaylist']);

    if (!$playlist)
        html_error('args');

    if (count($playlist['scores']) < 1)
        html_error('playlist_empty');

    if (isset($_GET['idscore'])) {
        for ($i = 0; $i < count($playlist['scores']); $i++) {
            if ($playlist['scores'][$i]['id'] == $_GET['idscore']) {
                break;
            }
        }

        if ($i == count($playlist['scores']))
            html_error('args');
    } else {
        $i = 0;
    }

    if (driver_play($playlist, $i))
        html_redirect('player.php');
    else
        html_error('driver');
}

function new_playlist() {
    test_auth();

    if (!isset($_POST['name']))
        html_error('args');

    $id = db_insert_playlist($_POST['name']);
    html_redirect("playlist.php?idplaylist=$id");
}

function rename_playlist() {
    test_auth();

    if (!(isset($_POST['name']) and isset($_POST['idplaylist'])))
        html_error('args');

    if (!db_get_playlist($_POST['idplaylist']))
        html_error('args');

    db_rename_playlist($_POST['idplaylist'], $_POST['name']);
    html_redirect('playlist.php?idplaylist=' . $_POST['idplaylist']);
}

function delete_playlist() {
    test_auth();

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
    test_auth();

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

    $score = db_insert_score($_POST['idplaylist'], pathinfo($file['name'], PATHINFO_FILENAME));
    $path = SCORE_DIR . '/' . $score['source'];
    move_uploaded_file($file['tmp_name'], $path);

    exec("/usr/bin/organ-midinfo --duration $path", $output, $retval);
    db_set_score_duration($score['id'], $output[0]);

    html_redirect("playlist.php?idplaylist={$_POST['idplaylist']}");
}

function rename_score() {
    test_auth();

    if (!(isset($_POST['name']) and isset($_POST['idscore'])))
        html_error('args');

    if (!db_get_score($_POST['idscore']))
        html_error('args');

    db_rename_score($_POST['idscore'], $_POST['name']);
    html_redirect(last_page());
}

function delete_score() {
    test_auth();

    if (!isset($_POST['idscore']))
        html_error('args');

    $score = db_get_score($_POST['idscore']);

    if (!$score)
        html_error('args');

    unlink(SCORE_DIR . '/' . $score['source']);
    db_delete_score($score['id']);
    html_redirect(last_page());
}

function set_shortcut() {
    test_auth();

    if (!(isset($_POST['idshortcut']) && isset($_POST['idplaylist'])))
        html_error('args');

    $idshortcut = $_POST['idshortcut'];
    $idplaylist = ($_POST['idplaylist']) ? $_POST['idplaylist'] : null;

    if (!db_set_shortcut($idshortcut, $idplaylist))
        html_error('args');
    
    html_redirect('remote.php');
}
