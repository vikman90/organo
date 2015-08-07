<?php
/**
 * Created by PhpStorm.
 * User: Vikman
 * Date: 03/08/2015
 * Time: 13:03
 */

namespace manager;
require_once __DIR__ . '/database.php';

$sock = socket_create(AF_UNIX, SOCK_STREAM, 0);

if (!$sock or !socket_connect($sock, SOCKET_PATH))
    html_error('socket');

function driver_play($playlist, $ifirst) {
    global $sock;
    $n = count($playlist['scores']);
    $buffer = "PLAYLOOP " . $n;

    for ($i = 0; $i < $n; $i++) {
        $buffer .= ' ' . SCORE_DIR . '/' . $playlist['scores'][($i + $ifirst) % $n]['source'];
    }

    socket_send($sock, $buffer, strlen($buffer), 0);
    socket_recv($sock, $buffer, BUFFER_MAX, 0);

    return ($buffer == "OK" ? true : false);
}

function driver_pause() {
    global $sock;
    $buffer = "PAUSE";

    socket_send($sock, $buffer, strlen($buffer), 0);
    socket_recv($sock, $buffer, BUFFER_MAX, 0);

    return ($buffer == "OK" ? true : false);
}

function driver_resume() {
    global $sock;
    $buffer = "RESUME";

    socket_send($sock, $buffer, strlen($buffer), 0);
    socket_recv($sock, $buffer, BUFFER_MAX, 0);

    return ($buffer == "OK" ? true : false);
}

function driver_stop() {
    global $sock;
    $buffer = "STOP";

    socket_send($sock, $buffer, strlen($buffer), 0);
    socket_recv($sock, $buffer, BUFFER_MAX, 0);

    return ($buffer == "OK" ? true : false);
}

function driver_status() {
    global $sock;
    $buffer = "STATUS";

    socket_send($sock, $buffer, strlen($buffer), 0);
    socket_recv($sock, $buffer, BUFFER_MAX, 0);
    return sscanf($buffer, "%s %s");
}
