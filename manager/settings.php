<?php
/**
 * Created by PhpStorm.
 * User: Vikman
 * Date: 14/05/2015
 * Time: 13:57
 */

namespace manager;
require_once 'lib/templates.php';

global $tr;

test_auth();
set_page('settings.php');

html_open('settings');
html_header();
html_navigation('settings');

echo <<< EOT
<section>
    <div>
        <h2>{$tr['settings']}</h2>
        <p>Página no implementada.</p>
    </div>
</section>

EOT;

html_footer();
html_close();
