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
        <h3 id="part-name">Pieza con un nombre extremadamente largo y sin apellidos, Fulgencio Marjangen Op. 34 nº 75</h3>
        <input type="button" class="player-button" id="bt-previuos" title="{$tr['previuos']}" onclick="previuos()">
        <input type="button" class="player-button" id="bt-play" title="{$tr['play']}" onclick="play()">
        <input type="button" class="player-button" id="bt-pause" title="{$tr['pause']}" onclick="pause()">
        <input type="button" class="player-button" id="bt-stop" title="{$tr['stop']}" onclick="stop()">
        <input type="button" class="player-button" id="bt-next" title="{$tr['next']}" onclick="next()">
    </div>
    <div id="player-playlist">
        <h2>{$tr['current_playlist']}</h2>
        <ul>
            <li><a href="">Tengo un tractor amarillo, pero soy daltónico</a></li>
            <li class="selected"><a href="">Pieza con un nombre extremadamente largo y sin apellidos, Fulgencio Marjangen Op. 34 nº 75</a></li>
            <li><a href="">Toccata e fugue</a></li>
            <li><a href="">Marcha fúnebre para homilía</a></li>
            <li><a href="">Hallelujah (la de Alexandra Burke)</a></li>
        </ul>
    </div>
</section>

EOT;

html_footer();
html_close();
