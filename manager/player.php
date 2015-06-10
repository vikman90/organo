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
html_script('player.js');
html_header();
html_navigation('player');

echo <<< EOT
<section>
    <div id="player-control">
        <h3 id="score-name">-</h3>
        <input type="button" class="player-button" id="bt-previuos" title="{$tr['previuos']}" onclick="previuos()">
        <input type="button" class="player-button" id="bt-play" title="{$tr['play']}" onclick="play()">
        <input type="button" class="player-button" id="bt-pause" title="{$tr['pause']}" onclick="pause()">
        <input type="button" class="player-button" id="bt-stop" title="{$tr['stop']}" onclick="stop()">
        <input type="button" class="player-button" id="bt-next" title="{$tr['next']}" onclick="next()">
    </div>
    <div id="player-playlist">
        <h2>{$tr['current_playlist']}</h2>
        <ul id="current-playlist">
            <li data-idscore="1" onclick="select(this)">Tengo un tractor amarillo, pero soy daltónico</li>
            <li data-idscore="2" onclick="select(this)">Pieza con un nombre extremadamente largo y sin apellidos, Fulgencio Marjangen Op. 34 nº 75</li>
            <li data-idscore="3" onclick="select(this)">Toccata e fugue</li>
            <li data-idscore="4" onclick="select(this)">Marcha fúnebre para homilía</li>
            <li data-idscore="5" onclick="select(this)">Hallelujah (la de Alexandra Burke)</li>
        </ul>
    </div>
</section>

EOT;

html_footer();
html_close();
