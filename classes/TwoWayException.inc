<?php

/**
 * Implementation of Exception for TwoWay exceptions
 */
class TwoWayException extends Exception {

  private $tw_code = '';
  private $remote_debug = array();

  public function __construct($message, $code, $debug = array()) {
    parent::__construct($message);
    $this->tw_code = $code;
    $this->remote_debug = $debug;
  }

  public function getRemoteDebug($string = TRUE) {
    if ($string) {
      $results = '';
      $k = 1;
      $debugs = array();
      if (count($this->remote_debug) > 0) {
        $debugs = $this->remote_debug;
      } else {
        $debugs = $this->getTrace();
      }
      foreach ($debugs as $debug) {
        if (!empty($debug->function)) {
          $results .= "<br />\r\n" . $k . ": " . $debug->function;
          if (!empty($debug->file)) {
            $results .=  ' in ' . $debug->file . ' (' . $debug->line . ')';
          }
          $results .=  "\r\n<br />";
          $k++;
        }
      }
      return $results;
    }
    return $this->remote_debug;
  }

  public function getTwoWayCode() {
    return $this->tw_code;
  }

  public function watchdog($o_module, $t_message, $t_args = array()) {
    $debug = $this->getRemoteDebug(true);
    watchdog($o_module, '<pre>' . $t_message . "; Two-Way exception dump: \r\nCode: !code\r\nMessage: !message\r\nDebug Trace:\r\n!debug" . '</pre>', array(
      '!code' => $this->getTwoWayCode(),
      '!message' => $this->getMessage(),
      '!debug' => $debug
    ) + $t_args, WATCHDOG_ERROR);


  }

  public function dsm() {
    drupal_set_message(t("Two-Way exception dump: \r\n<br />Code: !code\r\n<br />Message: !message\r\n<br />Debug Trace:\r\n<br />!debug",
      array(
        '!code' => $this->getTwoWayCode(),
        '!message' => $this->getMessage(),
        '!debug' => $this->getRemoteDebug(true)
    )), 'error');
  }

}