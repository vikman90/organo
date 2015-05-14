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

html_open('settings');
html_header();
html_navigation('settings');
html_footer();
html_close();
