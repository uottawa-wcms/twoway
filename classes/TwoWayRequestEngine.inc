<?php

class TwoWayRequestEngine {

  private $request = NULL;
  private $mode = TWOWAY_RESPONSE;
  private $destination = '';

  public function doRequest() {
    $c = curl_init();
    curl_setopt_array($c, array(
      CURLOPT_URL => $this->destination,
      CURLOPT_POSTFIELDS => $this->getRequestData(),
      CURLOPT_FOLLOWLOCATION => TRUE,
      CURLOPT_FRESH_CONNECT => TRUE,
      CURLOPT_POST => TRUE,
      CURLOPT_RETURNTRANSFER => TRUE,
      CURLOPT_TIMEOUT => $this->getRequestTimeout(),
    ));
    $response_obj = curl_exec($c);
    if (($code = curl_getinfo($c, CURLINFO_HTTP_CODE)) >= 400) {
      throw new TwoWayException($this->lookupHTTPErrorCode($code), 'HTTP-' . $code);
    }
    if (curl_errno($c)) {
      throw new TwoWayException(curl_error($curl), "CURL-" . curl_errno($curl));
    }
    $answer = TRUE;
    if ($this->mode == TWOWAY_RESPONSE) {
      $answer = $this->processResponse($response_obj);
    }
    curl_close($c);
    return $answer;
  }

  private function processResponse($curl_response) {
    $obj = $this->buildResponse($curl_response);
    return $obj->getData();
  }

  private function buildResponse($response) {
    if (empty($response)) {
      return FALSE;
    }
    $response_object = new TwoWayRemoteResponse($response);
    return $response_object;
  }

  private function lookupHTTPErrorCode($code) {
    switch ($code) {
      case 400: return 'Bad Request';
      case 401: return 'Unauthorized';
      case 403: return 'Forbidden';
      case 404: return 'Not Found';
      case 405: return 'Method Not Allowed';
      case 406: return 'Not Acceptable';
      case 407: return 'Proxy Authentication Required';
      case 408: return 'Request Timeout';
      case 409: return 'Conflict';
      case 410: return 'Gone';
      case 411: return 'Length Required';
      case 412: return 'Precondition Failed';
      case 413: return 'Request Entity Too Large';
      case 414: return 'Request-URI Too Long';
      case 415: return 'Unsupported Media Type';
      case 416: return 'Requested Range Not Satisfiable';
      case 417: return 'Expectation Failed';
      case 500: return 'Internal Server Error';
      case 501: return 'Not Implemented';
      case 502: return 'Bad Gateway';
      case 503: return 'Service Unavailable';
      case 504: return 'Gateway Timeout';
      case 505: return 'HTTP Version Not Supported';
      default: return 'Unknown HTTP Error - ' . $code;
    }
  }

  private function getRequestTimeout() {
    $timeout = $this->request->getLocalVar('timeout');
    if (empty($timeout)) {
      $timeout = 30;
    }
    return $timeout;
  }

  private function getRequestData() {
    $data = $this->request->compactForTransport();
    $stage2 = array();
    foreach ($data as $key => $val) {
      $stage2[] = $key . '=' . urlencode($val);
    }
    return implode('&', $stage2);
  }

  public function __construct(TwoWayService $service, $mode = TWOWAY_RESPONSE) {
    $this->mode = $mode;
    $this->request = TwoWayRequest::constructFromService($service);
    $this->destination = $this->request->getDestination();
  }

}