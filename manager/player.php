<?php
/**
 * User: Vikman
 * Date: 05/02/2015
 * Time: 17:55
 */

namespace manager;
require_once 'lib/templates.php';

global $tr;

test_auth();
html_open('player');

echo <<< EOT
<script src="scripts/player.js"></script>

EOT;

html_header();
html_navigation('player');

echo <<< EOT
<section>
    <div id="player-control">
        <h3 id="part-name">-</h3>
        <input type="button" class="player-button" id="bt-previuos" title="{$tr['previuos']}" onclick="previuos()">
        <input type="button" class="player-button" id="bt-play" title="{$tr['play']}" onclick="play()">
        <input type="button" class="player-button" id="bt-pause" title="{$tr['pause']}" onclick="pause()">
        <input type="button" class="player-button" id="bt-stop" title="{$tr['stop']}" onclick="stop()">
        <input type="button" class="player-button" id="bt-next" title="{$tr['next']}" onclick="next()">
    </div>
    <div id="player-playlist">
        <h2>{$tr['current_playlist']}</h2>
        <ul id="current-playlist">
            <li id="part-1" onclick="select(1)">Tengo un tractor amarillo, pero soy daltónico</li>
            <li id="part-2" onclick="select(2)">Pieza con un nombre extremadamente largo y sin apellidos, Fulgencio Marjangen Op. 34 nº 75</li>
            <li id="part-3" onclick="select(3)">Toccata e fugue</li>
            <li id="part-4" onclick="select(4)">Marcha fúnebre para homilía</li>
            <li id="part-5" onclick="select(5)">Hallelujah (la de Alexandra Burke)</li>
        </ul>
    </div>
</section>

EOT;

html_footer();
html_close();
