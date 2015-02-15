<?php
/**
 * User: Vikman
 * Date: 06/02/2015
 * Time: 15:51
 */

function head() {
    $lang = 'en';
    $charset = 'UTF-8';
    $title = 'Hola';

    echo '<!DOCTYPE html>';
    echo '<html>';
    echo '<head lang="$lang">';
    echo '<meta charset="UTF-8">';
    echo '<title>$title</title>';
    echo '</head>';
    echo '<body>';
}

function footer() {
    echo '</body>';
    echo '</html>';
}

?>