<?php
/**
 * Created by PhpStorm.
 * User: Vikman
 * Date: 13/05/2015
 * Time: 15:07
 */

namespace manager;

require_once 'lib/templates.php';

if (isset($_SESSION['auth']))
    html_redirect('player.php');

html_open('login');
html_header(false);

echo <<< EOT
<section>
    <form action="control.php?action=login" method="post">
        <h2>{$tr->strings['login']}</h2>
        <input type=password required name="password" placeholder="{$tr->strings['password']}">
        <input type="submit" value="{$tr->strings['enter']}">

EOT;

if (isset($_GET['error']))
    echo <<< EOT
        <p>{$tr->strings['wrong_password']}</p>

EOT;

echo <<< EOT
    </form>
</section>

EOT;

html_footer();
html_close();
