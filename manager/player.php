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

        if (pathinfo($state[1], PATHINFO_DIRNAME) == SCORE_DIR) {
            $cur_score = db_find_score(pathinfo($state[1], PATHINFO_BASENAME));

            if ($cur_score == null) {
                $playlist = null;
                $name = pathinfo($state[1], PATHINFO_FILENAME);
            } else {
                $playlist = db_get_playlist($cur_score['playlist']);
                $name = $cur_score['name'];
            }
        } else {
            $playlist = null;
            $name = pathinfo($state[1], PATHINFO_FILENAME);
        }

        break;

    default:
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
        <h3 class="hidden" id="title-stopped">{$tr['stopped']}</h3>
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
        <ul id="current-playlist" data-idplaylist="{$playlist['id']}">

EOT;

    foreach ($playlist['scores'] as $score) {
        $class = ($score['id'] == $cur_score['id']) ? 'class="selected"' : '';
        echo <<< EOT
            <li $class data-idscore="{$score['id']}" onclick="play(this)">{$score['name']}</li>

EOT;
    }

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
