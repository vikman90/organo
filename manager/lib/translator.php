<?php
/**
 * Created by PhpStorm.
 * User: Vikman
 * Date: 10/05/2015
 * Time: 18:46
 */

namespace manager;

class Translator {
    public $code = '';
    public $language = '';
    public $strings = [];

    public function __construct($xml) {
        $this->code = (string)$xml['code'];
        $this->language = (string)$xml['language'];

        foreach ($xml as $string)
            $this->strings[(string)$string['name']] = (string)$string;
    }
}

foreach (glob('translations/*.xml') as $file) {
    $tr = new Translator(simplexml_load_file($file));
    $translators[$tr->code] = $tr;
}
