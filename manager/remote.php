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
set_page('remote.php');

$playlists = db_get_playlists();
$shortcuts = db_get_shortcuts();

html_open('remote');
html_script('remote.js');
html_header();
html_navigation('remote');

echo <<< EOT
<section>
    <div>
        <h2>{$tr['remote']}</h2>
        <table>

EOT;

foreach ($shortcuts as $sc) {
    $idsc = $sc['id'];
    $title = $tr['button'] . ' ' . $sc['id'];
    echo <<< EOT
            <tr>
                <td>$title</td>
                <td>
                    <form action="control.php?action=set_shortcut" method="post">
                        <input type="hidden" name="idshortcut" value="$idsc">
                        <select name="idplaylist" onchange="submit(this)">
                            <option></option>

EOT;

    foreach ($playlists as $pl) {
        $idpl = $pl['id'];
        $name = $pl['name'];
        $selected = ($idpl == $sc['playlist']) ? 'selected' : '';

        echo <<< EOT
                            <option value="$idpl" $selected>$name</option>

EOT;
    }

    echo <<< EOT
                        </select>
                    </form>
                </td>
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
