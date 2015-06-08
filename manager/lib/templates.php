<?php
/**
 * User: Vikman
 * Date: 06/02/2015
 * Time: 15:51
 */

namespace manager;

require_once 'lib/session.php';
require_once 'lib/translator.php';
require_once 'lib/values.php';

global $translators;

$lang = isset($_SESSION['lang']) ? $_SESSION['lang'] : DEFAULT_LANGUAGE;
$tr = $translators[$lang]->strings;

function html_open($id) {
    global $tr, $lang;

    echo <<< EOT
<!DOCTYPE html>
<html lang="$lang">
<meta charset="UTF-8">
<title>{$tr['title']}</title>
<link rel="shortcut icon" href="images/icon.png" type="image/png" />
<link rel="stylesheet" type="text/css" href="styles/styles.css" />
<body id="$id">

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
        <li id="header-control">
            <a href="" title="{$tr['control']}"></a>
            <ul>
                <li id="header-shutdown">
                    <a href="control.php?action=shutdown" onclick="return confirm('{$tr['shutdown_confirm']}')">{$tr['shutdown']}</a>
                </li>
                <li id="header-reboot">
                    <a href="control.php?action=reboot" onclick="return confirm('{$tr['reboot_confirm']}')">{$tr['reboot']}</a>
                </li>
            </ul>
        </li>

EOT;

    echo <<< EOT
        <li id="header-lang">
            <a href="" title="{$tr['language']}"></a>
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

}

function html_navigation($selected = null) {
    global $tr;

    $player = $selected == 'player' ? 'selected' : '';
    $playlists = $selected == 'playlists' ? 'selected' : '';
    $remote = $selected == 'remote' ? 'selected' : '';
    $settings = $selected == 'settings' ? 'selected' : '';

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
        <li class="$settings" id="nav-settings">
            <a href="settings.php">{$tr['settings']}</a>
        </li>
    </ul>
</nav>

EOT;
}

function html_footer() {
    global $tr;

    /*echo <<< EOT
<footer>
    <p>{$tr['footer']}</p>
</footer>

EOT;*/

    $time = (string)(microtime(true) - $_SERVER['REQUEST_TIME_FLOAT']) . ' s.';

    echo <<< EOT
<footer>
    <p>$time</p>
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
