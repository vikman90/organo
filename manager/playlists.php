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

html_open('playlists');
html_header();
html_navigation('playlists');
html_footer();
html_close();
