<?php
/**
 * User: Vikman
 * Date: 06/02/2015
 * Time: 15:51
 */

namespace manager;

require_once __DIR__ . '/session.php';
require_once __DIR__ . '/translator.php';
require_once __DIR__ . '/values.php';

global $translators;

$lang = isset($_SESSION['lang']) ? $_SESSION['lang'] : DEFAULT_LANGUAGE;
$tr = $translators[$lang]->strings;

function html_open($id, $refresh = null) {
    global $tr, $lang;
    $meta = $refresh ? "<meta http-equiv=\"refresh\" content=\"$refresh\">" : '';

    echo <<< EOT
<!DOCTYPE html>
<html id="$id" lang="$lang">
<meta charset="UTF-8">
<title>{$tr['title']}</title>
<link rel="shortcut icon" href="images/icon.png" type="image/png" />
<link rel="stylesheet" type="text/css" href="styles/styles.css" />
$meta
<body>

EOT;
}

function html_close() {
    echo <<< EOT
</body>
</html>

EOT;
}

function html_header($full = true) {
    global $tr;
    global $translators;

    echo <<< EOT
<header>
    <h1><a href="index.php">{$tr['title']}</a></h1>
    <ul>

EOT;

    if ($full)
        echo <<< EOT
        <li id="header-logout">
            <a href="control.php?action=logout" title="{$tr['logout']}"></a>
        </li>
        <li id="header-control">
            <a title="{$tr['control']}"></a>
            <ul>
                <li id="header-shutdown">
                    <a onclick="shutdown()">{$tr['shutdown']}</a>
                </li>
                <li id="header-reboot">
                    <a onclick="reboot()">{$tr['reboot']}</a>
                </li>
            </ul>
        </li>

EOT;

    echo <<< EOT
        <li id="header-lang">
            <a title="{$tr['language']}"></a>
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
</header>

EOT;

    if ($full) {
        html_script('header.js');

        echo <<< EOT
<div class="modal" id="dialog-shutdown" onclick="closeModal()">
    <div class="box" onclick="event.stopPropagation()">
        <h2>{$tr['shutdown']}</h2>
        <form action="control.php?action=shutdown" method="post">
            <p>{$tr['shutdown_confirm']}</p>
            <div class="toolbar">
                <input class="action bt-ok" type="submit" value="{$tr['accept']}">
                <input class="action bt-cancel" type="button" value="{$tr['cancel']}" onclick="closeModal()">
            </div>
        </form>
    </div>
</div>
<div class="modal" id="dialog-reboot" onclick="closeModal()">
    <div class="box" onclick="event.stopPropagation()">
        <h2>{$tr['reboot']}</h2>
        <form action="control.php?action=reboot" method="post">
            <p>{$tr['reboot_confirm']}</p>
            <div class="toolbar">
                <input class="action bt-ok" type="submit" value="{$tr['accept']}">
                <input class="action bt-cancel" type="button" value="{$tr['cancel']}" onclick="closeModal()">
            </div>
        </form>
    </div>
</div>

EOT;
    }
}

function html_navigation($selected = null) {
    global $tr;

    $player = $selected == 'player' ? 'selected' : '';
    $playlists = $selected == 'playlists' ? 'selected' : '';
    $remote = $selected == 'remote' ? 'selected' : '';

    echo <<< EOT
<nav>
    <ul>
        <li class="$player" id="nav-player">
            <a href="player.php">{$tr['player']}</a>
        </li>
        <li class="$playlists" id="nav-playlists">
            <a href="playlists.php">{$tr['playlists']}</a>
        </li>
        <li class="$remote" id="nav-remote">
            <a href="remote.php">{$tr['remote']}</a>
        </li>
    </ul>
</nav>

EOT;
}

function html_footer() {
    global $tr;

    echo <<< EOT
<footer>
    <p>{$tr['footer']}</p>
</footer>

EOT;
}

function html_error($type) {
    global $tr;
    html_open('error');
    html_header();

    echo <<< EOT
<section>
    <h2>{$tr['error']}</h2>
    <p>{$tr['error_' . $type]}</p>
</section>

EOT;

    html_footer();
    html_close();
    exit();
}

function html_redirect($target) {
    echo <<< EOT
<!DOCTYPE html>
<html>
    <meta http-equiv="refresh" content="0; $target">
</html>

EOT;

    exit();
}

function html_script($src) {
    echo "<script src=\"scripts/$src\"></script>\n";
}
