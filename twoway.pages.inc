<?php


  function twoway_add_server_form_submit($form, &$form_state) {
    $insert = array(
      'server_display' => $form_state['values']['server_display'],
      'server_machine' => $form_state['values']['server_machine'],
      'server_connection' => $form_state['values']['server_connection'],
      'default_username' => $form_state['values']['default_username'],
    );
    $insert['default_password'] = twoway_get_security_profile()->encryptServerPassword(
        $form_state['values']['default_password'],
        $insert
    );
    db_insert('twoway_servers')->fields($insert)->execute();
    drupal_set_message('Your server was added successfully');
    $form_state['redirect'] = 'admin/services/servers';
  }

  function twoway_add_server_form($form, &$form_state) {
    return array(
      'header' => array(
        '#markup' => '<h2>' . t('Add Server') . '</h2>'
      ),
      'server_display' => array(
        '#title' => t('Display Name'),
        '#required' => true,
        '#type' => 'textfield',
        '#maxlength' => 512,
      ),
      'server_machine' => array(
        '#title' => t('Machine Name'),
        '#type' => 'machine_name',
        '#maxlength' => 64,
        '#required' => true,
        '#machine_name' => array(
          'exists' => 'twoway_server_load',
          'source' => array('server_display'),
        )
      ),
      'server_connection' => array(
        '#title' => t('Connection URL'),
        '#required' => true,
        '#type' => 'textfield',
        '#maxlength' => 256,
        '#description' => t('Enter the base URL for the remote Drupal installation'),
      ),
      'default_username' => array(
        '#title' => t('Default Username'),
        '#required' => false,
        '#type' => 'textfield',
        '#maxlength' => 128,
        '#description' => t('If no credentials are supplied in the API request,
          these will be used instead.')
      ),
      'default_password' => array(
        '#title' => t('Default Password'),
        '#required' => false,
        '#type' => 'password',
        '#maxlength' => 128,
      ),
      'submit' => array(
        '#type' => 'submit',
        '#value' => t('Save Server'),
      )
    );
  }

  function twoway_edit_server_form_submit($form, &$form_state) {
    $fields = array();
    $fields['server_display'] = $form_state['values']['server_display'];
    $fields['server_connection'] = $form_state['values']['server_connection'];
    if ($form_state['values']['update_username'] === 1) {
      $fields['default_username'] = $form_state['values']['default_username'];
      $fields['default_password'] = twoway_get_security_profile()->encryptServerPassword(
          $form_state['values']['default_password'],
          $fields
      );
    }
    db_update('twoway_servers')
    ->fields($fields)
    ->condition('server_machine', $form_state['values']['server_machine'])
    ->execute();
    drupal_set_message(t('Server configuration saved successfully'));
    $form_state['redirect'] = 'admin/services/servers';
  }

  function twoway_edit_server_form($form, &$form_state, $server) {
    $edit_form = twoway_add_server_form($form, $form_state);
    $edit_form['header']['#markup'] = '<h1>' . t('Edit Server') . '</h1>';
    $edit_form['server_display']['#default_value'] = $server['server_display'];
    $edit_form['server_machine']['#value'] = $server['server_machine'];
    $edit_form['server_machine']['#type'] = 'value';
    $edit_form['server_connection']['#default_value'] = $server['server_connection'];
    $edit_form['update_username'] = array(
      '#type' => 'checkbox',
      '#title' => t('Update username and/or password'),
      '#default_value' => false,
    );
    $edit_form['submit']['#weight'] = 10;
    return $edit_form;
  }

  function twoway_delete_server_form_submit($form, &$form_state) {
    db_delete('twoway_servers')->condition('server_machine', $form_state['values']['server'])->execute();
    drupal_set_message(t('The server [!machine] was removed successfully.', array('!machine' => $form_state['values']['server'])));
    $form_state['redirect'] = 'admin/services/servers';
  }

  function twoway_delete_server_form($form, &$form_state, $server) {
    $form = array(
      'server' => array(
        '#type' => 'value',
        '#value' => $server['server_machine']
      )
    );
    return confirm_form($form,
            t("Are you sure you want to remove this server?") .
              "<br />" . $server['server_display'] . ' [' . $server['server_machine'] . ']',
            "admin/services/servers",
            "",
            t("Delete"),
            t("Cancel"));
  }

  function twoway_all_services_form_submit($form, &$form_state) {
    if (!empty($form_state['values']['action'])) {
      $enable = $form_state['values']['action'] == 'disable' ? false : true;
      foreach ($form_state['values']['main_listing'] as $key => $check) {
        if ($check !== 0) {
          twoway_set_enabled($check, $enable);
        }
      }
      drupal_set_message(t('The selected services have been updated.'));
    }
  }

  function twoway_all_services_form($form, &$form_state) {
    drupal_add_css(drupal_get_path('module', 'twoway') . '/twoway_status.css');
    $form = array();
    $headers = array(
      'service_name' => t('Service'),
      'description' => t('Description'),
      'status' => t('Status'),
    );
    $rows = array();
    $services = twoway_get_services();
    ksort($services);
    foreach ($services as $module_name => $service_list) {
      ksort($service_list);
      foreach ($service_list as $service_name => $service_info) {
        $rows[$module_name . '.' . $service_name] = array(
          'service_name' => $module_name . '.' . $service_name,
          'description' => $service_info['description'],
          'status' => twoway_check_enabled($module_name, $service_name) ?
            '<span class="twoway_online twoway_service_entry">Available</span>' :
            '<span class="twoway_offline twoway_service_entry">Disabled</span>'
        );
      }
    }
    $form['header'] = array('#markup' => '<h2>' . t('Services') . '</h2>');
    $form['main_listing'] = array(
      '#type' => 'tableselect',
      '#header' => $headers,
      '#options' => $rows,
      '#empty' => t('No services were found'),
    );
    $form['action'] = array(
      '#title' => t('Action'),
      '#type' => 'select',
      '#options' => array(
        'enable' => t('Enable selected items'),
        'disable' => t('Disable selected items'),
      ),
      '#empty_value' => '',
      '#default_value' => '',
      '#empty_option' => t('None'),
    );
    $form['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Execute'),
    );
    return $form;
  }

  function twoway_all_servers_page() {
    drupal_add_library('system', 'drupal.ajax');
    drupal_add_js(drupal_get_path('module', 'twoway') . '/twoway_status.js');
    drupal_add_css(drupal_get_path('module', 'twoway') . '/twoway_status.css');
    $headers = array(
      'title' => array('data' => t('Server Name'),),
      'path' => array('data' => t('Location'),),
      'credentials' => array('data' => t('Has Credentials'),),
      'status' => array('data' => t('Status'),),
      'services' => array('data' => t('Services Available'),),
      'ops' => array('data' => t('Operations'),),
    );
    $rows = array();
    $rs = db_query('SELECT * FROM {twoway_servers} ORDER BY server_display');
    foreach ($rs as $row) {
      $actual = array();
      $actual['title'] = $row->server_display;
      $actual['path'] = $row->server_connection;
      $actual['credentials'] = empty($row->default_username) ? 'No' : 'Yes (' . $row->default_username . ')';
      $actual['status'] = '<div id="'.$row->server_machine.'" class="twoway_status_request"><span class="twoway_queued">Queued</span></div>';
      $actual['services'] = l('??', 'admin/services/servers/' . $row->server_machine . '/services', array(
        'attributes' => array(
          'id' => 'services_' . $row->server_machine
        )
      ));
      $actual['ops'] = implode(' | ', array(
        l('edit', 'admin/services/servers/' . $row->server_machine . '/edit'),
        l('delete', 'admin/services/servers/' . $row->server_machine . '/delete'),
      ));
      $rows[] = $actual;
    }
    return theme('table', array(
      'header' => $headers,
      'rows' => $rows
    ));
  }

  function twoway_server_status_callback($server) {
    module_load_include('inc', 'twoway', 'services/twoway.enabled');
    $value = '<span class="twoway_queued>' . t('Queued') . '</span>';
    $test = twoway_invoke_enabled($server['server_connection']);
    $total = 'N/A';
    if ($test == 'success') {
      module_load_include('inc', 'twoway', 'services/twoway.available');
      $total = count(twoway_invoke_available($server['server_connection']));
    }
    switch ($test) {
      case 'success': $value = '<span class="twoway_online">' . t('Active') . '</span>'; break;
      case 'failed': $value = '<span class="twoway_offline">' . t('Offline') . '</span><br /><span class="twoway_reason">unknown error</span>'; break;
      default: $value = '<span class="twoway_offline">' . t('Offline') . '</span><br /><span class="twoway_reason">' . $test . '</span>';
    }
    return array(
      "id" => $server['server_machine'],
      "replace" => $value,
      "total" => $total,
    );
  }

  function twoway_get_remote_service_list($server) {
    module_load_include('inc', 'twoway', 'services/twoway.available');
    $services = twoway_invoke_available($server['server_connection'], true);
    $headers = array(
      'service' => array('data' => 'Service'),
      'description' => array('data' => 'Description'),
    );
    $rows = array();
    foreach ($services as $service) {
      $service = (object) $service;
      $rows[] = array(
        'service' => array('data' => $service->module_name . '.' . $service->service_name),
        'description' => $service->description,
      );
    }
    return theme('table', array(
      'header' => $headers,
      'rows' => $rows,
      'empty' => t('No services found on remote host'),
    ));
  }