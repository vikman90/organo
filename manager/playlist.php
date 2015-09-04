<?php
/**
 * Created by PhpStorm.
 * User: Vikman
 * Date: 11/06/2015
 * Time: 11:11
 */

namespace manager;
require_once 'lib/database.php';
require_once 'lib/templates.php';

global $tr;

test_auth();

$playlist = null;

if (isset($_GET['idplaylist']))
    $playlist = db_get_playlist($_GET['idplaylist']);

if ($playlist === null)
    html_error('args');

set_page("playlist.php?idplaylist={$_GET['idplaylist']}");

html_open('playlists');
html_script('playlist.js');
html_header();
html_navigation('playlists');

echo <<< EOT
<section>
    <div ondragover="drag(event)" ondragleave="dragstop()" ondrop="drop(event, this)" id="playlist" data-idplaylist="{$playlist['id']}">
        <h2>
            <span id="plname">{$playlist['name']}</span>
            <input class="action bt-add" type="button" title="{$tr['add_score']}" onclick="add()">
        </h2>
        <div class="toolbar">
            <input type="button" class="action bt-rename" value="{$tr['rename']}" onclick="renamePlaylist()">
            <input type="button" class="action bt-delete" value="{$tr['delete']}" onclick="deletePlaylist()">
        </div>
        <form action="control.php?action=new_score" method="post" enctype="multipart/form-data">
            <input type="hidden" name="idplaylist" value="{$playlist['id']}">
            <input type="file" name="score" id="input-score" onchange="submit()" accept="audio/mid">
        </form>
        <table>

EOT;

foreach ($playlist['scores'] as $score) {
    $name = $score['name'] === null ? $score['source'] : $score['name'];
    $duration = date('i:s', mktime(0, 0, $score['duration']));
    $link = DOWNLOADS_DIR . '/' . $score['source'];
    $download = $name . '.mid';

    echo <<< EOT
            <tr data-idscore="{$score['id']}" onclick="play(this)">
                <td class="icon"><a onclick="event.stopPropagation()" class="bt-download" href="$link" download="$download" title="{$tr['play']}"></a></td>
                <td>$name</td>
                <td>$duration</td>
                <td class="icon"><a class="bt-rename" onclick="renameScore(event, this)" title="{$tr['rename']}"></a></td>
                <td class="icon"><a class="bt-delete" onclick="deleteScore(event, this)" title="{$tr['delete']}"></td></td>
            </tr>

EOT;
}

echo <<< EOT
        </table>
        <p>{$tr['tip_dragdrop']}</p>
    </div>
</section>
<div class="modal" id="dialog-rename-playlist" onclick="closeModal()">
    <div class="box" onclick="event.stopPropagation()">
        <h2>{$tr['rename_playlist']}</h2>
        <form action="control.php?action=rename_playlist" method="post">
            <input id="input-idplaylist" type="hidden" name="idplaylist" value="{$playlist['id']}">
            <input id="input-plname" type="text" name="name" placeholder="{$tr['name']}" maxlength="255" required>
            <div class="toolbar">
                <input class="action bt-ok" type="submit" value="{$tr['accept']}">
                <input class="action bt-cancel" type="button" value="{$tr['cancel']}" onclick="closeModal()">
            </div>
        </form>
    </div>
</div>
<div class="modal" id="dialog-delete-playlist" onclick="closeModal()">
    <div class="box" onclick="event.stopPropagation()">
        <h2>{$tr['delete_playlist']}</h2>
        <form action="control.php?action=delete_playlist" method="post">
            <input id="input-idplaylist" type="hidden" name="idplaylist" value="{$playlist['id']}">
            <p>{$tr['confirm_delete_playlist']}</p>
            <div class="toolbar">
                <input class="action bt-ok" type="submit" value="{$tr['accept']}">
                <input class="action bt-cancel" type="button" value="{$tr['cancel']}" onclick="closeModal()">
            </div>
        </form>
    </div>
</div>
<div class="modal" id="dialog-rename-score" onclick="closeModal()">
    <div class="box" onclick="event.stopPropagation()">
        <h2>{$tr['rename_score']}</h2>
        <form action="control.php?action=rename_score" method="post">
            <input id="input-rename-idscore" type="hidden" name="idscore">
            <input id="input-scorename" type="text" name="name" placeholder="{$tr['name']}" maxlength="255" required>
            <div class="toolbar">
                <input class="action bt-ok" type="submit" value="{$tr['accept']}">
                <input class="action bt-cancel" type="button" value="{$tr['cancel']}" onclick="closeModal()">
            </div>
        </form>
    </div>
</div>
<div class="modal" id="dialog-delete-score" onclick="closeModal()">
    <div class="box" onclick="event.stopPropagation()">
        <h2>{$tr['delete_score']}</h2>
        <form action="control.php?action=delete_score" method="post">
            <input id="input-delete-idscore" type="hidden" name="idscore">
            <p>{$tr['confirm_delete_score']}</p>
            <div class="toolbar">
                <input class="action bt-ok" type="submit" value="{$tr['accept']}">
                <input class="action bt-cancel" type="button" value="{$tr['cancel']}" onclick="closeModal()">
            </div>
        </form>
    </div>
</div>
<div class="modal" id="dialog-uploading"">
    <div class="box">
        <h2>{$tr['wait']}</h2>
        <p>{$tr['uploading']}</p>
        <p id="current_file"></p>
    </div>
</div>

EOT;


html_footer();
html_close();
