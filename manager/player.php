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
html_header();
html_navigation('player');
html_footer();
html_close();

?>

