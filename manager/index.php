<?php
/**
 * User: Vikman
 * Date: 05/02/2015
 * Time: 17:55
 */

namespace manager;
require_once 'lib/templates.php';

html_open('player', $tr->strings['title']);
html_header();
html_navigation();
html_footer();
html_close();

?>

