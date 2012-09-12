<?php

class TwoWayInvocation {

  private $service_entry;
  private $error = '';
  private $request = array();
  private $message = array();
  private $compacted_message = array();
  private $accessing_user = 0;

  public function __construct($service, $data) {
    $this->service_entry = $service;
    $this->validateService();

    if ($this->checkError()) { return; }
    $this->includeAPI();

    if ($this->checkError()) { return; }
    $this->assembleRequest($data);

    if ($this->checkError()) { return; }
    $this->decryptMessage();

    if ($this->checkError()) { return; }
    $this->validateMessage();

    if ($this->checkError()) { return; }
    $this->authenticate();

    if ($this->checkError()) { return; }
    $this->authorize();

    if ($this->checkError()) { return; }
    $this->finalValidation();
  }

  public function checkError() {
    if (empty($this->error)) {
      return FALSE;
    }
    return $this->error;
  }

  public function getMessage() {
    return $this->message;
  }

  private function finalValidation() {
    $code = twoway_get_security_profile()->finalInvalidation($this->message);
    if (!empty($code)) {
      $this->error = $code;
    }
  }

  private function authenticate() {
    twoway_get_security_profile()->unsecureRequest($this->message);
    $username = $this->message->getHeader('username');
    if (!empty($username)) {
      $user = user_load_by_name($this->message->getHeader('username'));
      if (empty($user)) {
        $this->error = 'invalid-username';
      } else {
        include DRUPAL_ROOT . '/includes/password.inc';
        if (!user_check_password($this->message->getHeader('password'), $user)) {
          $this->error = 'invalid-username';
        } else {
          $this->accessing_user = $user->uid;
        }
      }
    }
    $this->message->setHeader('password', '');
    $this->message->setHeader('username', '');
    $this->message->setLocalVar('user_id', $this->accessing_user);
  }

  private function authorize() {
    if (!twoway_check_service_access($this->service_entry, $this->message)) {
      $this->error = 'access-denied';
    }
  }

  private function validateMessage() {
    if ($this->message->getMessageVar('state') !== 'request') {
      $this->error = 'encrypt-error';
      return;
    }
    $ihash = $this->request['package_verification'];
    $chash = twoway_get_security_profile()->generateHMAC($this->compacted_message);
    if ($ihash != $chash) {
      $this->error = 'invalid-hash';
      return;
    }
    $code = twoway_get_security_profile()->invalidateRequest($this->message, $this->request);
    if (!empty($code)) {
      $this->error = $code;
    }
  }

  private function decryptMessage() {
    $this->compacted_message = twoway_get_security_profile()->decryptRequest(
        $this->request['encrypted_package'],
        $this->request
    );
    $json = json_decode($this->compacted_message);
    if (empty($json)) {
      $this->error = 'json-decode-error';
    } else {
      $this->message = TwoWayRequest::constructFromInvocation((array) $json);
    }
  }

  private function assembleRequest($data) {
    $this->request = array();
    foreach ($data as $key => $value) {
      $this->request[$key] = $value;
    }
    $this->request = twoway_get_security_profile()->deobfuscateRequestPackage($this->request);
    if (isset($this->request['package_headers'])) {
      $this->request['package_headers'] = (array) json_decode($this->request['package_headers']);
    }
  }

  private function includeAPI() {
    if (!empty($this->service_entry['service_file'])) {
      $include_path = drupal_get_path('module', $this->service_entry['module_name']);
      $include_path .= '/' . $this->service_entry['service_file'];
      @include_once $include_path;
    }
    if (!is_callable($this->service_entry['callback'])) {
      $this->error = 'callback-not-callable';
    }
  }

  private function validateService() {
    if (empty($this->service_entry)) {
      $this->error = 'no-service-found';
    } elseif (!twoway_check_enabled($this->service_entry['module_name'], $this->service_entry['service_name'])) {
      $this->error = 'service-disabled';
    } elseif (!isset($this->service_entry['callback'])) {
      $this->error = 'no-callback';
    } elseif ((!empty($this->service_entry['require_ssl'])) && (!$_SERVER['HTTPS'])) {
      $this->error = 'requires-ssl';
    }
}

}