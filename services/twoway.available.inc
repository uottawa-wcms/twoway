<?php

  function twoway_get_available_services($args, $request) {
    $services = twoway_get_services();
    $response = array();
    foreach ($services as $module_name => $list) {
      foreach ($list as $service_name => $service_info) {
        if (twoway_check_enabled($module_name, $service_name)) {
          if (twoway_check_service_access($service_info, $request)) {
            $response[] = array(
              'module_name' => $module_name,
              'service_name' => $service_name,
              'description' => $service_info['description'],
            );
          }
        }
      }
    }
    return $response;
  }

  function twoway_invoke_available($remote_server, $errors = FALSE, $module = NULL, $service = NULL) {
    module_load_include('inc', 'twoway', 'twoway.invoke');
    $service = new TwoWayService('twoway', 'available', $remote_server);
    try {
      $result = $service->invoke();
      if ($result !== false) {
        if (!empty($module) && !empty($service)) {
          return isset($result[$module]) && isset($result[$module][$service]) ? $result[$module][$service] : false;
        }
        return $result;
      }
      return array();
    } catch (TwoWayException $e) {
      if ($errors) {
        $e->dsm();
      }
      return array();
    }
  }