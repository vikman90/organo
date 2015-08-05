<?php
/**
 * Created by PhpStorm.
 * User: Vikman
 * Date: 05/08/2015
 * Time: 19:51
 */

namespace manager;

require_once '../lib/driver.php';
require_once '../lib/session.php';

if (get_auth())
    echo driver_pause() ? 'OK' : 'ERROR';
else
    echo 'ERROR';
