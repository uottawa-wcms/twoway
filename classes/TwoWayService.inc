<?php

/**
  * Used to generate an invocation of a remote method.
  *
  * This is the workhorse of how clients will use this package. All calls to
  *   remote methods should use this class to execute them.
  */
class TwoWayService {

  public $module;
  public $service;
  public $remote;
  public $username;
  public $password;
  public $timeout;
  public $arguments = array();
  public $return = true;

  public function __construct($module, $service_name, $remote_server) {
    $this->module = $module;
    $this->service = $service_name;
    $this->remote = $remote_server;
  }

  public function invoke(array $func_args = array()) {
    $this->arguments = $func_args;
    module_load_include('inc', 'twoway', 'twoway.api');
    return twoway_invoke($this, $this->return ? TWOWAY_RESPONSE : TWOWAY_NO_RESPONSE);
  }

  // leave mutators in for backwards compatibility
  public function setTimeout($timeout) {
    $this->timeout = $timeout;
  }

  public function setUsername($username) {
    $this->username = $username;
  }

  public function setPassword($password) {
    $this->password = $password;
  }

  public function getReturnValue($check) {
    $this->return = $check;
  }
}