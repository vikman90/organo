<?php
/**
 * Created by PhpStorm.
 * User: Vikman
 * Date: 14/05/2015
 * Time: 13:48
 */

namespace manager;

require_once __DIR__ . '/values.php';

session_start();

function test_auth() {
    if (!isset($_SESSION['auth']))
        html_redirect('index.php');
}

function set_auth() {
    $_SESSION['auth'] = 1;
}

function get_auth() {
    return $_SESSION['auth'];
}

function unset_auth() {
    unset($_SESSION['auth']);
}

function set_page($file) {
    $_SESSION['last'] = $file;
}

function last_page() {
    if (isset($_SESSION['last']))
        return $_SESSION['last'];
    else
        return 'index.php';
}

function set_language($code) {
    $_SESSION['lang'] = $code;
}

function get_language() {
    return isset($_SESSION['lang']) ? $_SESSION['lang'] : DEFAULT_LANGUAGE;
}