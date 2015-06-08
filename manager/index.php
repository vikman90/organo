<?php
/**
 * Created by PhpStorm.
 * User: Vikman
 * Date: 13/05/2015
 * Time: 15:07
 */

namespace manager;
require_once 'lib/templates.php';
global $tr;

if (isset($_GET['view']) and ($_GET['view'] == 'shutdown' or $_GET['view'] == 'reboot')) {
    html_open('shutdown');
    html_header(false);

    echo <<< EOT
<section>
    <h2>{$tr[($_GET['view'] == 'shutdown') ? 'shutting_down' : 'rebooting']}</h2>
</section>

EOT;

    html_footer();
    html_close();
} else {
    if (isset($_SESSION['auth']))
        html_redirect('player.php');

    html_open('login');
    html_header(false);

    echo <<< EOT
<section>
    <form action="control.php?action=login" method="post">
        <h2>{$tr['login']}</h2>
        <input type=password required name="password" placeholder="{$tr['password']}">
        <input type="submit" value="{$tr['enter']}">

EOT;

    if (isset($_GET['error']))
        echo <<< EOT
        <p>{$tr['wrong_password']}</p>

EOT;

    echo <<< EOT
    </form>
</section>

EOT;

    html_footer();
    html_close();
}

