<?php

class TwoWayRemoteResponse {

  private $message = array();

  private $headers = array();
  private $vectors = array();
  private $response_data = array();

  function __construct($encoded_message) {
    if (empty($encoded_message)) {
      throw new TwoWayException('Empty return message', 'CLIERR-blank');
    }
    $this->message = twoway_get_security_profile()->deobfuscateResponsePackage((array) json_decode($encoded_message));
    if (!(isset($this->message['is_response']) &&
        $this->message['is_response'] == 'yes')) {
      throw new TwoWayException('Debugging detected: ' . var_export($encoded_message, TRUE), 'DEBUG-Debugging');
    }
    if (!(
        isset($this->message['package_verification']) ||
        isset($this->message['encrypted_package'])
    )) {
      throw new TwoWayException('Invalid response format', 'CLIERR-tamperformat');
    }
    $decoded = twoway_get_security_profile()->decryptResponse($this->message['encrypted_package'], $this->message['package_headers']);
    $inc_hash = twoway_get_security_profile()->generateHMAC($decoded);
    if ($inc_hash != $this->message['package_verification']) {
      throw new TwoWayException('Tampering detected - HMAC', 'CLIERR-tamperhmac');
    }
    $results = (array) json_decode($decoded);
    if (!(isset($results['headers']) && isset($results['vectors']) && isset($results['data']))) {
      throw new TwoWayException('Tampering detected - Invalid Format', 'CLIERR-tamperenc');
    }
    $this->headers = (array) $results['headers'];
    $this->vectors = (array) $results['vectors'];
    $this->response_data = (array) $results['data'];
    twoway_get_security_profile()->validateResponse($this, $this->vectors);
    if ($this->response_data['state'] == 'error') {
      throw new TwoWayException(
          $this->response_data['error_message'],
          $this->response_data['error_code'],
          $this->response_data['debug']
      );
    }
    if (!isset($this->response_data['result'])) {
      throw new TwoWayException('Missing response results', 'CLIERR-noresults');
    }
  }

  public function getData() {
    return $this->response_data['result'];
  }
}