<?php
/**
 * Created by PhpStorm.
 * User: Vikman
 * Date: 12/05/2015
 * Time: 12:39
 */

namespace manager;
require_once 'lib/templates.php';

if (!isset($_GET['action']))
    html_error('args');

switch ($_GET['action']) {
    case 'language':
        language();
        break;
    case 'shutdown':
        shutdown();
        break;
    case 'reboot':
        reboot();
        break;
    default:
        html_error('args');
}

function language() {
    global $translators;

    if (!isset($_GET['code']))
        html_error('args');

    foreach ($translators as $t) {
        if ($t->code == $_GET['code']) {
            $_SESSION['lang'] = $t->code;
            html_redirect('index.php');
        }
    }

    html_error('lang_not_found');
}

function shutdown() {
    global $tr;

    exec('/usr/bin/sudo /sbin/shutdown now 2>&1', $output, $retval);

    if ($retval == 0) {
        html_open('shutdown');

        echo <<< EOT
<section>
    <h2>{$tr->strings['shutting_down']}</h2>
</section>

EOT;


        html_footer();
        html_close();
    } else
        html_error('shutting_down');
}

function reboot() {
    global $tr;

    exec('/usr/bin/sudo /sbin/shutdown -r now 2>&1', $output, $retval);

    if ($retval == 0) {
        html_open('shutdown');

        echo <<< EOT
<section>
    <h2>{$tr->strings['rebooting']}</h2>
</section>

EOT;


        html_footer();
        html_close();
    } else
        html_error('shutting_down');
}