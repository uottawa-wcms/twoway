<?php


class TwoWayResponse {

  private $response_data = NULL;
  private $headers = array();
  private $vectors = array();

  public function __construct(TwoWayInvocation $invocation, array $response, $service_entry) {
    $this->response_data = $response;
    $this->vectors = array(
      'service_name' => $service_entry['service_name'],
      'module_name' => $service_entry['module_name'],
      'origin' => $GLOBALS['base_url'],
      'timestamp' => time(),
    );
    twoway_get_security_profile()->prepareResponse($this, $invocation->getMessage());
  }

  public function getHeader($key) { return $this->headers[$key]; }

  public function setHeader($key, $value) {
    $this->headers[$key] = $value;
  }

  public function getVector($key) { return $this->vectors[$key]; }

  public function setVector($key, $value) {
    $this->vectors[$key] = $value;
  }

  public function getVectors() { return $this->vectors; }

  public function compactForTransport() {
    $inner_package = array(
      'data' => $this->response_data,
      'vectors' => $this->vectors,
      'headers' => $this->headers,
    );
    $compacted = serialize($inner_package);
    $encrypted = twoway_get_security_profile()->encryptResponse($compacted, $this);
    $hmac = twoway_get_security_profile()->generateHMAC($compacted);
    $package = array(
      'encrypted_package' => $encrypted,
      'package_verification' => $hmac,
      'is_response' => 'yes',
      'package_headers' => $this->vectors,
    );
    return twoway_get_security_profile()->obfuscateResponsePackage($package);
  }

}

?>
