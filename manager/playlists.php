<?php
/**
 * Created by PhpStorm.
 * User: Vikman
 * Date: 14/05/2015
 * Time: 13:56
 */

namespace manager;
require_once 'lib/database.php';

global $tr;

test_auth();
set_page('playlists.php');

html_open('playlists');
html_script('playlists.js');
html_header();
html_navigation('playlists');

echo <<< EOT
<section>
    <div>
        <h2>
            <span>{$tr['playlists']}</span>
            <input class="action bt-add" type="button" title="{$tr['add_playlist']}" onclick="add()">
        </h2>
        <table>

EOT;

foreach (db_get_playlists() as $playlist) {
    if ($playlist['scores'] == 0)
        $scores = $tr['playlist_empty'];
    elseif ($playlist['scores'] == 1)
        $scores = '1 ' . $tr['score'];
    else
        $scores = $playlist['scores'] . ' ' . $tr['scores'];

    echo <<< EOT
            <tr data-idplaylist="{$playlist['id']}" onclick="list(this)">
                <td><a class="bt-play" href="control.php?action=play&idplaylist={$playlist['id']}" title="{$tr['play']}"></a></td>
                <td><strong>{$playlist['name']}</strong></td>
                <td>$scores</td>
            </tr>

EOT;
}

echo <<< EOT
        </table>
    </div>
</section>
<div class="modal" id="dialog" onclick="closeDialog()">
    <div class="box" onclick="event.stopPropagation()">
        <h2>{$tr['add_playlist']}</h2>
        <form action="control.php?action=new_playlist" method="post">
            <input id="dialog-name" type="text" name="name" placeholder="{$tr['name']}" maxlength="255" required>
            <div class="toolbar">
                <input class="action bt-ok" type="submit" value="{$tr['accept']}">
                <input class="action bt-cancel" type="button" value="{$tr['cancel']}" onclick="closeDialog()">
            </div>
        </form>
    </div>
</div>
EOT;


html_footer();
html_close();
