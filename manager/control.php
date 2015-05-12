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