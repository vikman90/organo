<?php
/**
 * Created by PhpStorm.
 * User: Vikman
 * Date: 12/05/2015
 * Time: 12:39
 */

namespace manager;
require_once 'lib/templates.php';

if (!isset($_GET['action']))
    html_error('args');

switch ($_GET['action']) {
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
    default:
        html_error('args');
}

function language() {
    global $translators;

    if (!isset($_GET['code']))
        html_error('args');

    foreach ($translators as $t) {
        if ($t->code == $_GET['code']) {
            $_SESSION['lang'] = $t->code;
            html_redirect('index.php');
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
