<?php
/**
 * User: Vikman
 * Date: 06/02/2015
 * Time: 15:51
 */


namespace manager;
require_once 'lib/translator.php';
require_once 'lib/values.php';

session_start();
$tr = $translators[(isset($_SESSION['lang']) ? $_SESSION['lang'] : DEFAULT_LANGUAGE)];

function html_open($id, $title) {
    global $tr;
    $lang = $tr->code;

    echo <<< EOT
<!DOCTYPE html>
<html lang="$lang">
<meta charset="UTF-8">
<title>$title</title>
<link rel="shortcut icon" href="images/icon.png" type="image/png" />
<link rel="stylesheet" type="text/css" href="styles/styles.css" />
<body>

EOT;
}

function html_close() {
    echo <<< EOT
</body>
</html>

EOT;
}

function html_header() {
    global $tr;
    global $translators;

    echo <<< EOT
<header>
    <ul>
        <li id="header-control">
            <a href="" title="{$tr->strings['control']}"></a>
            <ul>
                <li id="header-shutdown">
                    <a href="control.php?action=shutdown">{$tr->strings['shutdown']}</a>
                </li>
                <li id="header-reboot">
                    <a href="control.php?action=reboot">{$tr->strings['reboot']}</a>
                </li>
            </ul>
        </li>
        <li id="header-lang">
            <a href="" title="{$tr->strings['language']}"></a>
            <ul>

EOT;

    foreach ($translators as $t)
        echo <<< EOT
                <li>
                    <a href="control.php?action=language&code=$t->code">$t->language</a>
                </li>

EOT;

    echo <<< EOT
            </ul>
        </li>
    </ul>
    <h1>{$tr->strings['title']}</h1>
</header>

EOT;

}

function html_navigation() {
    global $tr;

    echo <<< EOT
<nav>
    <ul>
        <li class="selected" id="nav-player">
            <a href="">{$tr->strings['player']}</a>
        </li>
        <li id="nav-playlists">
            <a href="">{$tr->strings['playlists']}</a>
        </li>
        <li id="nav-remote">
            <a href="">{$tr->strings['remote']}</a>
        </li>
        <li id="nav-settings">
            <a href="">{$tr->strings['advanced']}</a>
        </li>
    </ul>
</nav>

EOT;
}

function html_footer() {
    global $tr;

    echo <<< EOT

<footer>
    <p>{$tr->strings['footer']}</p>
</footer>

EOT;
}
