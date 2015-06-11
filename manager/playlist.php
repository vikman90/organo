<?php
/**
 * Created by PhpStorm.
 * User: Vikman
 * Date: 11/06/2015
 * Time: 11:11
 */

namespace manager;
require_once 'lib/database.php';

global $tr;

test_auth();

$playlist = null;

if (isset($_GET['idplaylist']))
    $playlist = db_get_playlist($_GET['idplaylist']);

if ($playlist === null)
    html_error('args');

set_page("playlists.php?idplaylist={$_GET['idplaylist']}");

html_open('playlists');
html_script('playlist.js');
html_header();
html_navigation('playlists');

echo <<< EOT
<section>
    <div ondragover="drag(event)" ondragleave="dragstop()" ondrop="drop(event)" id="playlist">
        <h2>
            <span>{$playlist['name']}</span>
            <input class="action bt-add" type="button" title="{$tr['add_score']}" onclick="add()">
        </h2>
        <form id="form-score" action="control.php?action=new_score" method="post" enctype="multipart/form-data">
            <input type="hidden" name="idplaylist" value="{$playlist['id']}">
            <input type="file" name="score" id="input-score" onchange="submit()" accept="audio/mid">
        </form>
        <table>

EOT;

foreach ($playlist['scores'] as $score) {
    $name = $score['name'] === null ? $score['source'] : $score['name'];

    echo <<< EOT
            <tr data-idscore="{$score['id']}">
                <td>$name</td>
            </tr>
EOT;
}

echo <<< EOT
        </table>
    </div>
</section>
EOT;


html_footer();
html_close();
