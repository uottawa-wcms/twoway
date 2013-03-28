<?php

  /**
   * Verifies the current request as a valid two-way request and then executes it.
   * @param array $service_entry The service entry
   * @return array The response array (error or success) that will be sent to the
   *   client.
   */
  function twoway_invocation($service_entry) {
    $handler = new TwoWayInvocationHandler($service_entry);
    return $handler->invoke();
  }

  /**
   * Invokes a two-way method
   *
   * @param string $message The message object (see the TwoWayService class for
   *   easy construction)
   * @param int $mode The mode (TWOWAY_RESPONSE OR TWOWAY_NO_RESPONSE
   * @return mixed If TWOWAY_RESPONSE was set as the $mode, will return the
   *   response value returned by the remotely invoked function. Otherwise,
   *   returns true.
   * @throws TwoWayException If any exception occured during transfer.
   */
  function twoway_invoke(TwoWayService $message, $mode = TWOWAY_RESPONSE) {
    $engine = new TwoWayRequestEngine($message, $mode);
    $response = $engine->doRequest();
    return $response;
  }

  function twoway_check_service_access(array $service_entry, $request) {
    if (isset($service_entry['access_callback']) && is_callable($service_entry['access_callback'])) {
      $args = $service_entry['access_arguments'];
      array_unshift($args, $request);
      if (!call_user_func_array($service_entry['access_callback'], $args)) {
        return false;
      }
    } else if ($service_entry['access_callback'] === FALSE) {
      return false;
    }
    return true;
  }




