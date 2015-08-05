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

switch ($state[0]) {
    case 'PLAYING':
    case 'PAUSED':
        if ($state[1] >= 0) {
            $playlist = db_get_playlist($state[1]);
            $score = db_get_score($state[2]);
            $name = $score ? $score['name'] : $tr['unknown'];
        } elseif ($state[3]) {
            $name = pathinfo($state[3], PATHINFO_FILENAME);
            echo $state[3];
            $playlist = null;
        }

        break;

    case 'STOPPED':
        $playlist = null;
        $name = $tr['stopped'];
}

$play_hidden = $state[0] == 'PLAYING' ? 'hidden' : '';
$pause_hidden = $state[0] == 'PLAYING' ? '' : 'hidden';
$skip_disabled = $playlist ? '' : 'disabled';
$play_disabled = $state[0] == 'STOPPED' ? 'disabled' : '';

echo <<< EOT
<section>
    <div id="player-control">
        <h3 id="score-name">$name</h3>
        <input type="button" class="player-button" id="bt-previous" title="{$tr['previuos']}" onclick="previous()" $skip_disabled>
        <input type="button" class="player-button $play_hidden" id="bt-play" title="{$tr['play']}" onclick="resume()" $play_disabled>
        <input type="button" class="player-button $pause_hidden" id="bt-pause" title="{$tr['pause']}" onclick="pause()"$play_disabled>
        <input type="button" class="player-button" id="bt-stop" title="{$tr['stop']}" onclick="stop()" $play_disabled>
        <input type="button" class="player-button" id="bt-next" title="{$tr['next']}" onclick="next()" $skip_disabled>
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
