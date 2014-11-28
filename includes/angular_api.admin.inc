<?php

/**
 * @file
 * File that contains all of the configuration forms and related callbacks (ajax and others).
 */

/**
 * Modules' configuration form.
 *
 * As this is the API module, we need to provide a *right* way to integrate with all of the
 * 
 * @param array $form
 *   The settings form itself.
 * 
 * @param array $form_state
 *   Form's formstate.
 * 
 * @return array
 *   Returned form array.
 */
function angular_api_settings_form($form, $form_state) {
  $form = array();

  $form['angular_api_core'] = array(
    '#type' => 'fieldset',
    '#title' => t('AngularJS Core Library'),
    '#collapsed' => FALSE,
  );
  $form['angular_api_core']['angular_api_version'] = array(
    '#type' => 'select',
    '#title' => t('AngularJS version'),
    '#options' => array(
      'none' => '- None Installed -',
      '1.0' => '1.0.x',
      '1.1' => '1.1.x',
      '1.2' => '1.2.x' ,
      '1.3' => '1.3.x' ,
    ),
    '#default_value' => variable_get('angular_api_version', 'none'),
    '#description' => t("Please select the version of AngularJS. <br /><em>Note: If you're using CDN, this setting will be ignored, otherwise we'll check for the existence of the library (make sure you have it in site/all/libraries).</em>"),
    '#ajax' => array(
      'callback' => 'angular_api_settings_update_version',
      'wrapper' => 'angular-check-status',
      'method' => 'replace',
      'effect' => 'fade',
    ),
  );

  $found = FALSE;
  $used_version = (isset($form_state['values']['angular_api_version'])) ? $form_state['values']['angular_api_version'] : variable_get('angular_api_version', 'none');
  if ($used_version !== 'none') {
    $found = angular_api_check_version($used_version);
  }
  $check_msg = ($found) ? '<span style="color: green;">' . t('Library successfully installed.') . '</span>' : '<span style="color: red;">' . t('Make sure you unpack the zip file from https://angularjs.org/ to the libraries folder.') . '</span>';
  $form['angular_api_core']['angular_library_check'] = array(
    '#prefix' => '<div id="angular-check-status">',
    '#markup' => $check_msg,
    '#suffix' => '</div>',
  );

  $form['angular_api_core']['angular_api_cdn'] = array(
    '#type' => 'select',
    '#title' => t('AngularJS CDN'),
    '#options' => array(
      '0' => t('No'),
      '1' => t('Yes'),
    ),
    '#default_value' => variable_get('angular_api_cdn', '0'),
    '#description' => t('Should we use a CDN (Google / CDN JS)?<br />We will use the latest version available (1.08, 1.1.5, 1.2.27, 1.3.3) for the major version you specified, if you use CDN.'),
    '#ajax' => array(
      'callback' => 'angular_api_settings_update_cdn_source',
      'wrapper' => 'angular-api-cdn-source',
      'method' => 'replace',
      'effect' => 'fade',
    ),
  );

  $angular_api_cdn = (isset($form_state['values']['angular_api_cdn'])) ? $form_state['values']['angular_api_cdn'] : variable_get('angular_api_cdn', '0');
  if ($angular_api_cdn !== '0') {
    $form['angular_api_core']['angular_api_cdn_source'] = array(
      '#prefix' => '<div id="angular-api-cdn-source">',
      '#suffix' => '</div>',
      '#type' => 'select',
      '#title' => t('CDN Provider'),
      '#options' => array(
        'google' => t('Google'),
        'cdnjs' => t('CDN JS'),
      ),
      '#default_value' => variable_get('angular_api_cdn_source', 'google'),
      '#description' => t('Pick the CDN provider.'),
    );
  }
  else {
    $form['angular_api_core']['angular_api_cdn_source'] = array(
      '#markup' => '<div id="angular-api-cdn-source"></div>',
    );
  }

  $form['angular_api_core']['angular_api_compression_type'] = array(
    '#type' => 'radios',
    '#title' => t('AngularJS compression level'),
    '#options' => array(
      'minified' => t('Production (minified)'),
      'source' => t('Development (source)'),
    ),
    '#default_value' => variable_get('angular_api_compression_type', 'min'),
    '#description' => t('Which version of the library would you like to use?'),
  );

  // Extras go here.
  $form['angular_api_extra'] = array(
    '#type' => 'fieldset',
    '#title' => t('AngularJS Extra Settings'),
    '#collapsed' => FALSE,
  );
  $form['angular_api_extra']['angular_api_load_type'] = array(
    '#type' => 'select',
    '#title' => t('Library load method'),
    '#options' => array(
      'everywhere' => t('Load everywhere'),
      'path' => t('Load path-specific'),
      'manual' => t('Load manually'),
    ),
    '#default_value' => variable_get('angular_api_load_type', 'manual'),
    '#description' => t('How should we load the library? Please pick the most efficient method. If you are developing your own module, be efficient and load the library manually.'),
    '#ajax' => array(
      'callback' => 'angular_api_settings_update_load_type_details',
      'wrapper' => 'angular-api-autoload-files',
      'method' => 'replace',
      'effect' => 'fade',
    ),
  );

  $lib_files = angular_api_get_additional_files($used_version);
  $angular_api_load_type = (isset($form_state['values']['angular_api_load_type'])) ? $form_state['values']['angular_api_load_type'] : variable_get('angular_api_load_type', 'manual');
  if ($angular_api_load_type == 'everywhere' || $angular_api_load_type == 'path') {
    $form['angular_api_extra']['angular_api_autoload_files_fieldset'] = array(
      '#type' => 'fieldset',
      '#title' => t('Autoload files'),
      '#description' => t('List of files to load automatically on every page.'),
      '#prefix' => '<div id="angular-api-autoload-files">',
      '#suffix' => '</div>',
    );
    $form['angular_api_extra']['angular_api_autoload_files_fieldset']['angular_api_autoload_files'] = array(
      '#type' => 'checkboxes',
      '#title' => t('Files to load (all the pages'),
      '#options' => $lib_files,
      '#default_value' => variable_get('angular_api_autoload_files', array()),
      '#description' => t('Which files should automatically be loaded?'),
    );

    if ($angular_api_load_type == 'everywhere') {
      $form['angular_api_extra']['angular_api_autoload_files_fieldset']['angular_api_autoload_path'] = array(
        '#type' => 'hidden',
        '#value' => variable_get('angular_api_autoload_path', ''),
      );
    }
    else {
      $form['angular_api_extra']['angular_api_autoload_files_fieldset']['angular_api_autoload_path'] = array(
        '#type' => 'textarea',
        '#title' => t('Paths with Angular Library'),
        '#description' => t("Please specify paths on which you'd like to load the Angular.js library and all the related files (checked ones).<br />Don't forget to specify a <strong>path per line."),
        '#default_value' => variable_get('angular_api_autoload_path', ''),
      );
    }
  }
  else {
    $lib_files_html = implode('.js, ', array_keys(array('angular' => 'angular') + $lib_files));
    $form['angular_api_extra']['angular_api_autoload_files_fieldset'] = array(
      '#type' => 'fieldset',
      '#title' => t('Manual Load Instructions'),
      '#prefix' => '<div id="angular-api-autoload-files">',
      '#suffix' => '</div>',
    );
    $form['angular_api_extra']['angular_api_autoload_files_fieldset']['angular_api_autoload_files'] = array(
      '#markup' => t('Feel free to use <code>libraries_load</code> or to <code>#attach</code> the libraries to a form.<br />Libs you can load: !libs_html.<br />A documentation page with examples is coming soon.', array('!libs_html' => $lib_files_html)),
    );
    $form['angular_api_extra']['angular_api_autoload_files_fieldset']['angular_api_autoload_path'] = array(
      '#type' => 'hidden',
      '#value' => variable_get('angular_api_autoload_path', ''),
    );
  }

  $form = system_settings_form($form);
  $form['#submit'][] = 'angular_api_settings_submit_extra';
  return $form;
}

/**
 * Ajax-check for the version.
 */
function angular_api_settings_update_version($form, $form_state) {
  return array(
    '#type' => 'ajax',
    '#commands' => array(
      ajax_command_replace('#angular-check-status', render($form['angular_api_core']['angular_library_check'])),
      ajax_command_replace('#angular-api-autoload-files', render($form['angular_api_extra']['angular_api_autoload_files_fieldset'])),
    ),
  );
}

/**
 * Ajax callback to update the CDN Provider.
 */
function angular_api_settings_update_cdn_source($form, $form_state) {
  return $form['angular_api_core']['angular_api_cdn_source'];
}

/**
 * Ajax callback to update the Load Type details.
 */
function angular_api_settings_update_load_type_details($form, $form_state) {
  return $form['angular_api_extra']['angular_api_autoload_files_fieldset'];
}

/**
 * Additional submit callback.
 */
function angular_api_settings_submit_extra($form, $form_state) {
  // After we save all the things, we want to flush the libraries cache.
  drupal_flush_all_caches();
}
