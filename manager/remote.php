<?php
/**
 * Created by PhpStorm.
 * User: Vikman
 * Date: 14/05/2015
 * Time: 13:56
 */

namespace manager;
require_once 'lib/templates.php';

global $tr;

test_auth();
set_page('remote.php');

html_open('remote');
html_header();
html_navigation('remote');

echo <<< EOT
<section>
    <div>
        <h2>{$tr['remote']}</h2>
    </div>
</section>

EOT;

html_footer();
html_close();
