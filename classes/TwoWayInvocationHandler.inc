<?php

class TwoWayInvocationHandler {

    private $invocation = NULL;
    private $service_entry = NULL;

    public function __construct($service_entry) {
      $this->service_entry = $service_entry;
      $this->invocation = new TwoWayInvocation($service_entry, $_POST);
    }

    public function invoke() {
      $response = new TwoWayResponse($this->invocation, $this->getResponse(), $this->service_entry);
      $result = $response->compactForTransport();
      return $result;
    }

    private function getResponse() {
      if ($error_code = $this->invocation->checkError()) {
        return $this->generateError($error_code);
      }
      $args = $this->service_entry['callback_arguments'];
      array_unshift($args,
          $this->invocation->getMessage()->getCallbackArguments(),
          $this->invocation->getMessage()
      );
      try {
        $results = call_user_func_array($this->service_entry['callback'], $args);
        if (!(is_array($results) && !empty($results['state']) && $results['state'] == 'error')) {
          $message = $this->generateResponseMessage($results);
          return $message;
        } else {
          return $results;
        }
      } catch (Exception $e) {
        return TwoWayInvocationHandler::createErrorResponse($e->getMessage(), 'EXEC', $e->getTrace());
      }
    }

    public function generateResponseMessage($results) {
      return array(
        'state' => 'success',
        'result' => $results,
      );
    }

    private function generateError($code) {
      return TwoWayInvocationHandler::createErrorResponse(
          $this->getInvocationErrorMessage($code),
          'PROC-' . $code
      );
    }

    private function getInvocationErrorMessage($code) {
      switch ($code) {
        case 'invalid-username': return 'Invalid username and/or password';
        case 'callback-not-callable': return 'Invalid callback method';
        case 'no-callback': return 'Callback method not found';
        case 'request-decayed': return 'Request has decayed';
        case 'throttled': return 'Request throttled';
        case 'requires-ssl': return 'SSL is required for this connection';
        case 'invalid-hash': return 'Invalid HMAC';
        case 'encrypt-error': return 'Decryption of message failed';
        case 'repeated-message': return 'Cannot replay old messages';
        case 'service-disabled': return 'Service has been disabled';
        case 'no-service-found': return 'Service could not be located';
        case 'service-forbidden': return 'Access denied';
        default: return 'Unknown Error - ' . $type;
      }
    }

    public static function createErrorResponse($message, $code, $debug = NULL) {
      return array(
        'state' => 'error',
        'error_message' => $message,
        'debug' => $debug,
        'error_code' => $code,
      );
    }

}
