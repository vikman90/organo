<?php
/**
 * User: Vikman
 * Date: 05/02/2015
 * Time: 17:55
 */

namespace manager;
require_once 'lib/database.php';
require_once 'lib/driver.php';

global $tr;

test_auth();
set_page('player.php');

html_open('player');
html_script('player.js');
html_header();
html_navigation('player');

$state = driver_status();

if (($state[0] == 'PLAYING' or $state[0] == 'PAUSED') and $state[1] >= 0 and $state[2] >= 0) {
    $playlist = db_get_playlist($state[1]);
    $score = db_get_score($state[2]);
} else
    $playlist = $score = null;


$name = $score ? $score['name'] : $tr['unknown'];
$play = $state[0] == 'PLAYING' ? 'hidden' : '';
$pause = $state[0] == 'PLAYING' ? '' : 'hidden';

echo <<< EOT
<section>
    <div id="player-control">
        <h3 id="score-name">$name</h3>
        <input type="button" class="player-button" id="bt-previuos" title="{$tr['previuos']}" onclick="previuos()">
        <input type="button" class="player-button $play" id="bt-play" title="{$tr['play']}" onclick="play()">
        <input type="button" class="player-button $pause" id="bt-pause" title="{$tr['pause']}" onclick="pause()">
        <input type="button" class="player-button" id="bt-stop" title="{$tr['stop']}" onclick="stop()">
        <input type="button" class="player-button" id="bt-next" title="{$tr['next']}" onclick="next()">
    </div>

EOT;

if ($playlist) {
    echo <<< EOT
    <div id="player-playlist">
        <h2>{$tr['current_playlist']}</h2>
        <ul id="current-playlist">

EOT;

    foreach ($playlist['scores'] as $score)
        echo <<< EOT
            <li data-idscore="{$score['id']}" onclick="select(this)">{$score['name']}</li>

EOT;

    echo <<< EOT
        </ul>
    </div>

EOT;
}

echo <<< EOT
</section>

EOT;

html_footer();
html_close();
