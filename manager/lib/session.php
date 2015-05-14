<?php
/**
 * Created by PhpStorm.
 * User: Vikman
 * Date: 14/05/2015
 * Time: 13:48
 */

namespace manager;

session_start();

function test_auth() {
    if (!isset($_SESSION['auth']))
        html_redirect('login.php');
}

function set_auth() {
    $_SESSION['auth'] = 1;
}