<?php

class TwoWayRequest {

  public static function constructFromService(TwoWayService $service) {
    $request = new TwoWayRequest();
    $request->loadFromService($service);
    $request->initRequest();
    $request->resolveServer();
    $request->secureData();
    return $request;
  }

  public static function constructFromInvocation($request_info) {
    $request = new TwoWayRequest();
    $request->loadFromInvocation($request_info);
    return $request;
  }

  // these are not to be sent to the remote host
  private $local_vars = array();

  // headers that are not part of the message nor are they encryption vectors
  private $headers = array();

  // elements of the message used as vectors for encryption
  private $message_vars = array();

  // elements of the message not needed for encryption
  private $message = array();

  public function __construct() {}

  public function compactForTransport() {
    $pieces = (object) array(
      'headers' => $this->headers,
      'message_vars' => $this->message_vars,
      'message' => $this->message,
    );
    $transport_body = json_encode($pieces);
    $message_data = array(
      'encrypted_package' => twoway_get_security_profile()->encryptRequest(
        $transport_body,
        $this
      ),
      'package_verification' => twoway_get_security_profile()->generateHMAC(
          $transport_body
      ),
      'package_headers' => json_encode($this->message_vars),
    );
    return twoway_get_security_profile()->obfuscateRequestPackage($message_data);
  }

  public function setLocalVar($key, $val) {
    $this->local_vars[$key] = $val;
  }

  public function getLocalVar($key) {
    if (isset($this->local_vars[$key])) {
      return $this->local_vars[$key];
    }
    return NULL;
  }

  public function setHeader($key, $val) {
    $this->headers[$key] = $val;
  }

  public function getHeader($key) {
    if (isset($this->headers[$key])) {
      return $this->headers[$key];
    }
    return NULL;
  }

  public function setMessageVar($key, $val) {
    $this->message_vars[$key] = $val;
  }

  public function getMessageVar($key) {
    if (isset($this->message_vars[$key])) {
      return $this->message_vars[$key];
    }
    return NULL;
  }

  private function loadFromInvocation($request) {
    $this->headers = (array) $request['headers'];
    $this->message_vars = (array) $request['message_vars'];
    $this->message = (array) $request['message'];
  }

  public function getDestination() {
    return $this->local_vars['remote_server'] . '?q=twoway-endpoint/' .
        $this->message_vars['module_name'] . '/' .
        $this->message_vars['service_name'];
  }

  public function getCallbackArguments() {
    return (array) $this->message['arguments'];
  }

  private function loadFromService(TwoWayService $service) {
    $this->message['arguments'] = $service->arguments;
    $this->message_vars += array(
      'module_name' => $service->module,
      'service_name' => $service->service,
    );
    $this->local_vars += array(
      'remote_server' => $service->remote,
      'timeout' => $service->timeout,
    );
    $this->headers += array(
      'username' => $service->username,
      'password' => $service->password,
    );
  }

  private function initRequest() {
    $this->message_vars += array(
      'origin' => twoway_get_server_path(),
      'state' => 'request',
      'timestamp' => time(),
    );
  }

  private function resolveServer() {
    $lookup = twoway_server_load($this->local_vars['remote_server']);
    if (!empty($lookup)) {
      $this->local_vars['remote_server'] = $lookup['server_connection'];
      if (empty($this->headers['username']) && !empty($lookup['username'])) {
        $this->headers['username'] = $lookup['username'];
        $this->headers['password'] = twoway_get_security_profile()->decryptServerPassword(
            $lookup['password'],
            $lookup
        );
      }
    }
    $this->local_vars['remote_server'] = str_replace('\\', '/', $this->local_vars['remote_server']);
    if (substr($this->local_vars['remote_server'], -1) != '/') {
      $this->local_vars['remote_server'] .= '/';
    }
  }

  private function secureData() {
    twoway_get_security_profile()->secureRequest($this);
  }

}