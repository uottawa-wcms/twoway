<?php

  function twoway_services_enabled() {
    return 'yes';
  }

  function twoway_invoke_enabled($remote_server) {
    module_load_include('inc', 'twoway', 'twoway.invoke');
    $service = new TwoWayService('twoway', 'enabled', $remote_server);
    $service->setTimeout(5);
    try {
      $result = $service->invoke();
      if ($result === 'yes') {
        return 'success';
      }
      return false;
    } catch (TwoWayException $e) {
      return $e->getMessage();
    }
  }

