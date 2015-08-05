<?php
/**
 * Created by PhpStorm.
 * User: Vikman
 * Date: 05/08/2015
 * Time: 23:19
 */

namespace manager;

require_once '../lib/session.php';
require_once '../lib/driver.php';

if (get_auth())
    echo driver_resume() ? 'OK' : 'ERROR';
else
    echo 'ERROR';
