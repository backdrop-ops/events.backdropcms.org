<?php

/**
 * @file
 * Functionality and helper functions for CAPTCHA administration.
 */

/**
 * Return an array with the available CAPTCHA types, for use as options array for a select form elements.
 *
 * @param bool $add_special_options
 *   if true: also add a 'none' and 'default' option
 *
 * @return array
 *   Mapping "$module/$type" to
 *   "$type (from module $module)" with $module the module name implementing the CAPTCHA
 *   and $type the name of the CAPTCHA type.
 */
function _captcha_available_challenge_types($add_special_options = TRUE) {
  $captcha_types = array();
  if ($add_special_options) {
    $captcha_types['none'] = t('- No challenge -');
    $captcha_types['default'] = t('Default challenge type');
  }
  // We do our own version of Backdrop's module_invoke_all() here because
  // we want to build an array with custom keys and values.
  foreach (module_implements('captcha') as $module) {
    $result = call_user_func_array($module . '_captcha', array('list'));
    if (is_array($result)) {
      foreach ($result as $type) {
        $captcha_types["$module/$type"] = t('@type (from module @module)', array('@type' => $type, '@module' => $module));
      }
    }
  }
  return $captcha_types;
}

/**
 * Form builder function for the general CAPTCHA configuration.
 */
function captcha_admin_settings() {
  module_load_include('inc', 'captcha');

  $config = config('captcha.settings');

  // Use JavaScript for some added usability on admin form.
  backdrop_add_js(backdrop_get_path('module', 'captcha') . '/captcha.js');

  // Load languages for configurable texts.
  if (module_exists('locale')) {
    $langs = language_list(FALSE, TRUE);
  }

  $short_help = '<p>' . t('CAPTCHA is a tool to fight automated submission by malicious users (spamming) of for example comments forms, user registration forms, guestbook forms, etc. You can extend the desired forms with an additional challenge, which should be easy for a human to solve correctly, but hard enough to keep automated scripts and spam bots out. "CAPTCHA" is an acronym for "Completely Automated Public Turing test to tell Computers and Humans Apart".') . '</p>';

  $more_help = '<p>' . t('A CAPTCHA can be added to virtually each Backdrop form. Some default forms are already provided in the form list, but arbitrary forms can be easily added and managed when the option <em>Add CAPTCHA administration links to forms</em> is enabled.') . '</p>';
  $more_help .= '<p>' . t('Users with the <em>Skip CAPTCHA</em> <a href="@perm">permission</a> won\'t be offered a challenge. Be sure to grant this permission to the trusted users (e.g. site administrators). If you want to test a protected form, be sure to do it as a user without the <em>Skip CAPTCHA</em> permission (e.g. as anonymous user).', array('@perm' => url('admin/config/people/permissions', array('fragment' => 'module-captcha')))) . '</p>';
  $more_help .= '<p>' . t('Note that the CAPTCHA module interacts with page caching (see <a href="!performancesettings">performance settings</a>). Because the challenge should be unique for each generated form, the caching of the page it appears on is prevented. Make sure that these forms do not appear on too many pages or you will lose much caching efficiency. For example, if you put a CAPTCHA on the user login block, which typically appears on each page for anonymous visitors, caching will practically be disabled. The comment submission forms are another example. In this case you should set the <em>Location of comment submission form</em> to <em>Display on separate page</em> in the comment settings of the relevant <a href="!contenttypes">content types</a> for better caching efficiency.',
    array(
      '!performancesettings' => url('admin/config/development/performance'),
      '!contenttypes' => url('admin/structure/types'),
    )
  ) . '</p>';
  $more_help .= '<p><em>' . t('CAPTCHA is a trademark of Carnegie Mellon University.') . '</em></p>';

  $form['help'] = array(
    '#type' => 'help',
    '#markup' => $short_help,
  );
  $form['more_help'] = array(
    '#type' => 'fieldset',
    '#title' => t('More help'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['more_help']['help'] = array(
    '#type' => 'help',
    '#markup' => $more_help,
  );

  // Configuration of which forms to protect, with what challenge.
  $form['captcha_form_protection'] = array(
    '#type' => 'fieldset',
    '#title' => t('Form protection'),
    '#description' => t("Select the challenge type you want for each of the listed forms (identified by their so called <em>form_id</em> or <em>base_form_id</em>). You can easily add arbitrary forms with the textfield at the bottom of the table or with the help of the option <em>Add CAPTCHA administration links to forms</em> below."),
  );
  $form['captcha_form_protection']['captcha_default_challenge'] = array(
    '#type' => 'select',
    '#title' => t('Default challenge type'),
    '#description' => t('Select the default challenge type for CAPTCHAs. This can be overriden for each form if desired.'),
    '#options' => _captcha_available_challenge_types(FALSE),
    '#default_value' => $config->get('default_challenge'),
  );
  // List known form_ids.
  $form['captcha_form_protection']['captcha_form_id_overview'] = array(
    '#theme' => 'captcha_admin_settings_captcha_points',
    '#tree' => TRUE,
  );
  $form['captcha_form_protection']['captcha_form_id_overview']['captcha_captcha_points'] = array();
  $captcha_type_options = _captcha_available_challenge_types();
  $captcha_points = captcha_get_captcha_points();

  foreach ($captcha_points as $captcha_point) {
    $elem = array();
    $elem['form_id'] = array(
      '#markup' => $captcha_point['form_id'],
    );
    // Select widget for CAPTCHA type.
    if (isset($captcha_point['module']) && $captcha_point['module']) {
      $captcha_type = $captcha_point['module'] . '/' . $captcha_point['captcha_type'];
    }
    elseif (isset($captcha_point['captcha_type']) && ($captcha_point['captcha_type'] == 'default')) {
      $captcha_type = 'default';
    }
    else {
      $captcha_type = 'none';
    }
    $elem['captcha_type'] = array(
      '#type' => 'select',
      '#default_value' => $captcha_type,
      '#options' => $captcha_type_options,
    );
    $ops = array();

    $ops[] = l(t('delete'), "admin/config/people/captcha/captcha/captcha_point/{$captcha_point['form_id']}/delete");

    $elem['operations'] = array('#markup' => implode(", ", $ops));

    $form['captcha_form_protection']['captcha_form_id_overview']['captcha_captcha_points'][$captcha_point['form_id']] = $elem;
  }

  // Form items for new form_id.
  $form['captcha_form_protection']['captcha_form_id_overview']['captcha_new_captcha_point'] = array();
  // Textfield for form_id.
  $form['captcha_form_protection']['captcha_form_id_overview']['captcha_new_captcha_point']['form_id'] = array(
    '#type' => 'textfield',
    '#size' => 16,
  );
  // Select widget for CAPTCHA type.
  $form['captcha_form_protection']['captcha_form_id_overview']['captcha_new_captcha_point']['captcha_type'] = array(
    '#type' => 'select',
    '#default_value' => 'none',
    '#options' => $captcha_type_options,
  );

  // Checkbox to add default CAPTCHA to all non listed forms as well.
  $form['captcha_form_protection']['captcha_default_challenge_on_nonlisted_forms'] = array(
    '#type' => 'checkbox',
    '#title' => t('Default challenge on non-listed forms.'),
    '#default_value' => $config->get('default_challenge_on_nonlisted_forms'),
    '#description' => t('Normally, no challenge is added to forms that are not listed above. Enabling this option will add the default challenge instead.'),
  );

  // Field for the CAPTCHA administration mode.
  $form['captcha_form_protection']['captcha_administration_mode'] = array(
    '#type' => 'checkbox',
    '#title' => t('Add CAPTCHA administration links to forms'),
    '#default_value' => $config->get('administration_mode'),
    '#description' => t('This option makes it easy to manage CAPTCHA settings on forms. When enabled, users with the <em>administer CAPTCHA settings</em> permission will see a fieldset with CAPTCHA administration links on all forms, except on administrative pages.'),
  );
  // Field for the CAPTCHAs on admin pages.
  $form['captcha_form_protection']['captcha_allow_on_admin_pages'] = array(
    '#type' => 'checkbox',
    '#title' => t('Allow CAPTCHAs and CAPTCHA administration links on administrative pages'),
    '#default_value' => $config->get('allow_on_admin_pages'),
    '#description' => t("This option makes it possible to add CAPTCHAs to forms on administrative pages. CAPTCHAs are disabled by default on administrative pages (which shouldn't be accessible to untrusted users normally) to avoid the related overhead. In some situations, e.g. in the case of demo sites, it can be usefull to allow CAPTCHAs on administrative pages."),
  );

  // Button for clearing the CAPTCHA placement cache.
  // Based on Backdrop core's "Clear all caches" (performance settings page).
  $form['captcha_form_protection']['captcha_placement_caching'] = array(
    '#type' => 'item',
    '#title' => t('CAPTCHA placement caching'),
    '#description' => t('For efficiency, the positions of the CAPTCHA elements in each of the configured forms are cached. Most of the time, the structure of a form does not change and it would be a waste to recalculate the positions every time. Occasionally however, the form structure can change (e.g. during site building) and clearing the CAPTCHA placement cache can be required to fix the CAPTCHA placement.'),
  );
  $form['captcha_form_protection']['captcha_placement_caching']['captcha_placement_cache_clear'] = array(
    '#type' => 'submit',
    '#value' => t('Clear the CAPTCHA placement cache'),
    '#submit' => array('captcha_clear_captcha_placement_cache_submit'),
  );

  // Textfield(s) for editing the error message.
  if (module_exists('locale')) {
    $form['captcha_error_messages'] = array(
      '#type' => 'fieldset',
      '#title' => t('Error message'),
      '#description' => t('Message displayed when the CAPTCHA has not been solved. An empty entry will reset the error message to default.'),
    );
    foreach ($langs as $lang_code => $lang_name) {
      $form['captcha_error_messages']['captcha_error_message_' . $lang_code] = array(
        '#type' => 'textfield',
        '#title' => t('For language %lang_name (code %lang_code)', array('%lang_name' => $lang_name, '%lang_code' => $lang_code)),
        '#default_value' => _captcha_get_error_message($lang_code),
        '#maxlength' => 256,
      );
    }
  }
  else {
    $form['captcha_error_message'] = array(
      '#type' => 'textfield',
      '#title' => t('Error message'),
      '#description' => t('Message displayed when the CAPTCHA has not been solved. An empty entry will reset the error message to default.'),
      '#default_value' => _captcha_get_error_message(),
      '#maxlength' => 256,
    );
  }

  // Configuration option for adding a CAPTCHA description.
  $form['captcha_add_captcha_description'] = array(
    '#type' => 'checkbox',
    '#title' => t('Add a description to the CAPTCHA'),
    '#description' => t('Add a configurable description to explain the purpose of the CAPTCHA to the visitor.'),
    '#default_value' => $config->get('add_captcha_description'),
  );
  // Textfield(s) for the CAPTCHA description.
  if (module_exists('locale')) {
    $form['captcha_descriptions'] = array(
      '#type' => 'fieldset',
      '#title' => t('CAPTCHA description'),
      '#description' => t('Configurable description of the CAPTCHA. An empty entry will reset the description to default.'),
      '#attributes' => array('id' => 'edit-captcha-description-wrapper'),
    );
    foreach ($langs as $lang_code => $lang_name) {
      $form['captcha_descriptions']["captcha_description_$lang_code"] = array(
        '#type' => 'textfield',
        '#title' => t('For language %lang_name (code %lang_code)', array('%lang_name' => $lang_name, '%lang_code' => $lang_code)),
        '#default_value' => _captcha_get_description($lang_code),
        '#maxlength' => 256,
      );
    }
  }
  else {
    $form['captcha_description'] = array(
      '#type' => 'textfield',
      '#title' => t('Challenge description'),
      '#description' => t('Configurable description of the CAPTCHA. An empty entry will reset the description to default.'),
      '#default_value' => _captcha_get_description(),
      '#maxlength' => 256,
      '#attributes' => array('id' => 'edit-captcha-description-wrapper'),
    );
  }

  // Option for case sensitive/insensitive validation of the responses.
  $form['captcha_default_validation'] = array(
    '#type' => 'radios',
    '#title' => t('Default CAPTCHA validation'),
    '#description' => t('Define how the response should be processed by default. Note that the modules that provide the actual challenges can override or ignore this.'),
    '#options' => array(
      CAPTCHA_DEFAULT_VALIDATION_CASE_SENSITIVE => t('Case sensitive validation: the response has to exactly match the solution.'),
      CAPTCHA_DEFAULT_VALIDATION_CASE_INSENSITIVE => t('Case insensitive validation: lowercase/uppercase errors are ignored.'),
    ),
    '#default_value' => $config->get('default_validation'),
  );

  // Field for CAPTCHA persistence.
  // TODO for D7: Rethink/simplify the explanation and UI strings.
  $form['captcha_persistence'] = array(
    '#type' => 'radios',
    '#title' => t('Persistence'),
    '#default_value' => $config->get('persistence'),
    '#options' => array(
      CAPTCHA_PERSISTENCE_SHOW_ALWAYS => t('Always add a challenge.'),
      CAPTCHA_PERSISTENCE_SKIP_ONCE_SUCCESSFUL_PER_FORM_INSTANCE => t('Omit challenges in a multi-step/preview workflow once the user successfully responds to a challenge.'),
      CAPTCHA_PERSISTENCE_SKIP_ONCE_SUCCESSFUL_PER_FORM_TYPE => t('Omit challenges on a form type once the user successfully responds to a challenge on a form of that type.'),
      CAPTCHA_PERSISTENCE_SKIP_ONCE_SUCCESSFUL => t('Omit challenges on all forms once the user successfully responds to any challenge on the site.'),
    ),
    '#description' => t('Define if challenges should be omitted during the rest of a session once the user successfully responds to a challenge.'),
  );

  // Enable wrong response counter.
  $form['captcha_enable_stats'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable statistics'),
    '#description' => t('Keep CAPTCHA related counters in the <a href="!statusreport">status report</a>. Note that this comes with a performance penalty as updating the counters results in clearing the variable cache.', array('!statusreport' => url('admin/reports/status'))),
    '#default_value' => $config->get('enable_stats'),
  );

  // Option for logging wrong responses.
  $form['captcha_log_wrong_responses'] = array(
    '#type' => 'checkbox',
    '#title' => t('Log wrong responses'),
    '#description' => t('Report information about wrong responses to the <a href="!dblog">log</a>.', array('!dblog' => url('admin/reports/dblog'))),
    '#default_value' => $config->get('log_wrong_responses'),
  );

  // Submit button.
  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save configuration'),
  );

  return $form;
}

/**
 * Custom theme function for a table of (form_id -> CAPTCHA type) settings.
 */
function theme_captcha_admin_settings_captcha_points($variables) {
  $form = $variables['form'];
  $header = array('form_id', t('Challenge type'), t('Operations'));
  $rows = array();
  // Existing CAPTCHA points.
  foreach (element_children($form['captcha_captcha_points']) as $key) {
    $row = array();
    $row[] = backdrop_render($form['captcha_captcha_points'][$key]['form_id']);
    $row[] = backdrop_render($form['captcha_captcha_points'][$key]['captcha_type']);
    $row[] = backdrop_render($form['captcha_captcha_points'][$key]['operations']);
    $rows[] = $row;
  }
  // For new CAPTCHA point.
  $row = array();
  $row[] = backdrop_render($form['captcha_new_captcha_point']['form_id']);
  $row[] = backdrop_render($form['captcha_new_captcha_point']['captcha_type']);
  $row[] = '';
  $rows[] = $row;

  $output = theme('table', array('header' => $header, 'rows' => $rows));
  return $output;
}

/**
 * Validation handler for captcha_admin_settings form.
 */
function captcha_admin_settings_validate($form, $form_state) {
  $form_id = $form_state['values']['captcha_form_id_overview']['captcha_new_captcha_point']['form_id'];
  if (!preg_match('/^[a-z0-9_]*$/', $form_id)) {
    form_set_error('captcha_form_id_overview][captcha_new_captcha_point][form_id', t('Illegal form_id'));
  }
}

/**
 * Submission function for captcha_admin_settings form.
 */
function captcha_admin_settings_submit($form, &$form_state) {
  $captcha_points = array();
  $config = config('captcha.settings');
  $config->set('administration_mode', $form_state['values']['captcha_administration_mode']);
  $config->set('allow_on_admin_pages', $form_state['values']['captcha_allow_on_admin_pages']);
  $config->set('default_challenge', $form_state['values']['captcha_default_challenge']);
  $config->set('default_challenge_on_nonlisted_forms', $form_state['values']['captcha_default_challenge_on_nonlisted_forms']);

  // Load languages for configurable texts.
  if (module_exists('locale')) {
    $langs = language_list(FALSE, TRUE);
  }

  // Process CAPTCHA points.
  if (isset($form_state['values']['captcha_form_id_overview']['captcha_captcha_points'])) {
    // Load existing data.
    $captcha_points = captcha_get_captcha_points();
    foreach ($form_state['values']['captcha_form_id_overview']['captcha_captcha_points'] as $captcha_point_form_id => $data) {
      captcha_set_form_id_setting($captcha_point_form_id, $data['captcha_type']);
    }
  }

  // Add new CAPTCHA point?
  $captcha_point_form_id = $form_state['values']['captcha_form_id_overview']['captcha_new_captcha_point']['form_id'];
  if (!empty($captcha_point_form_id)) {
    $captcha_type = $form_state['values']['captcha_form_id_overview']['captcha_new_captcha_point']['captcha_type'];
    captcha_set_form_id_setting($captcha_point_form_id, $captcha_type);
    backdrop_set_message(t('Added CAPTCHA point.'), 'status');
  }

  // Error message.
  if (module_exists('locale')) {
    foreach ($langs as $lang_code => $lang_name) {
      $description = $form_state['values']['captcha_error_message_' . $lang_code];
      if ($description) {
        $config->set('error_message_' . $lang_code, $description);
      }
      else {
        $config->clear('error_message_' . $lang_code);
        backdrop_set_message(t('Reset of error message for language %language.', array('%language' => $lang_name)), 'status');
      }
    }
  }
  else {
    $description = $form_state['values']['captcha_error_message'];
    if ($description) {
      $config->set('error_message', $description);
    }
    else {
      $config->clear('error_message');
      backdrop_set_message(t('Reset of error message.'), 'status');
    }
  }

  // CAPTCHA description stuff.
  $config->set('add_captcha_description', $form_state['values']['captcha_add_captcha_description']);
  // Save (or reset) the CAPTCHA descriptions.
  if (module_exists('locale')) {
    foreach ($langs as $lang_code => $lang_name) {
      $description = $form_state['values']["captcha_description_$lang_code"];
      if ($description) {
        $config->set("description_$lang_code", $description);
      }
      else {
        $config->clear("description_$lang_code");
        backdrop_set_message(t('Reset of CAPTCHA description for language %language.', array('%language' => $lang_name)), 'status');
      }
    }
  }
  else {
    $description = $form_state['values']['captcha_description'];
    if ($description) {
      $config->set('description', $description);
    }
  }

  $config->set('default_validation', $form_state['values']['captcha_default_validation']);
  $config->set('persistence', $form_state['values']['captcha_persistence']);
  $config->set('enable_stats', $form_state['values']['captcha_enable_stats']);
  $config->set('log_wrong_responses', $form_state['values']['captcha_log_wrong_responses']);

  // Process CAPTCHA points.
  if (isset($form_state['values']['captcha_form_id_overview']['captcha_captcha_points'])) {

    $captcha_points = array();

    foreach ($form_state['values']['captcha_form_id_overview']['captcha_captcha_points'] as $captcha_point_form_id => $data) {
      $captcha_points[$captcha_point_form_id] = _captcha_format_captcha_point($captcha_point_form_id, $data['captcha_type']);
    }
  }

  // Add new CAPTCHA point?
  // Process at same time as editing other form_ids
  $captcha_point_form_id = $form_state['values']['captcha_form_id_overview']['captcha_new_captcha_point']['form_id'];
  if (!empty($captcha_point_form_id)) {
    $captcha_type = $form_state['values']['captcha_form_id_overview']['captcha_new_captcha_point']['captcha_type'];

    $captcha_points[$captcha_point_form_id] = _captcha_format_captcha_point($captcha_point_form_id, $captcha_type);

    backdrop_set_message(t('Added CAPTCHA point.'), 'status');
  }

  $config->set('captcha_points', $captcha_points);
  $config->save();

  backdrop_set_message(t('The CAPTCHA settings have been saved.'), 'status');
}



/**
 * Submit callback; clear CAPTCHA placement cache.
 */
function captcha_clear_captcha_placement_cache_submit($form, &$form_state) {
  state_set('captcha_placement_map_cache', NULL);
  backdrop_set_message(t('Cleared the CAPTCHA placement cache.'));
}

/**
 * Central handler for CAPTCHA point administration (adding, disabling, deleting).
 */
function captcha_point_admin($captcha_point_form_id = NULL, $op = NULL) {
  module_load_include('inc', 'captcha');

  // If $captcha_point_form_id and action $op given: do the action.
  if ($captcha_point_form_id) {
    if ($op == 'delete') {
      return backdrop_get_form('captcha_point_disable_confirm', $captcha_point_form_id, TRUE);
    }
    elseif ($op == 'disable') {
      return backdrop_get_form('captcha_point_disable_confirm', $captcha_point_form_id, FALSE);
    }
    // Return edit form for CAPTCHA point.
    return backdrop_get_form('captcha_point_admin_form', $captcha_point_form_id);
  }
  // Return add form for CAPTCHA point.
  return backdrop_get_form('captcha_point_admin_form');
}

/**
 * Admin form.
 */
function captcha_point_admin_form($form, $form_state, $captcha_point_form_id = NULL) {
  $form = array();
  $default_captcha_type = 'none';
  if (isset($captcha_point_form_id)) {
    // Use given CAPTCHA point form_id.
    $form['captcha_point_form_id'] = array(
      '#type' => 'textfield',
      '#title' => t('Form ID'),
      '#description' => t('The Backdrop form_id or base_form_id of the form to add the CAPTCHA to.'),
      '#value' => check_plain($captcha_point_form_id),
      '#disabled' => TRUE,
    );
    $captcha_point = captcha_get_form_id_setting($captcha_point_form_id);
    if ($captcha_point) {
      $default_captcha_type = "{$captcha_point['module']}/{$captcha_point['captcha_type']}";
    }
  }
  else {
    // Textfield for CAPTCHA point form_id.
    $form['captcha_point_form_id'] = array(
      '#type' => 'textfield',
      '#title' => t('Form ID'),
      '#description' => t('The Backdrop form_id or base_form_id of the form to add the CAPTCHA to.'),
    );
  }
  // Select widget for CAPTCHA type.
  $form['captcha_type'] = array(
    '#type' => 'select',
    '#title' => t('Challenge type'),
    '#description' => t('The CAPTCHA type to use for this form.'),
    '#default_value' => $default_captcha_type,
    '#options' => _captcha_available_challenge_types(),
  );
  // Redirect to general CAPTCHA settings page after submission.
  $form['#redirect'] = 'admin/config/people/captcha';
  // Submit button.
  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );
  return $form;
}

/**
 * Validation function for captcha_point_admin_form.
 */
function captcha_point_admin_form_validate($form, $form_state) {
  if (!preg_match('/^[a-z0-9_]+$/', $form_state['values']['captcha_point_form_id'])) {
    form_set_error('captcha_point_form_id', t('Illegal form_id'));
  }
}

/**
 * Submit function for captcha_point_admin_form.
 */
function captcha_point_admin_form_submit($form, $form_state) {
  $captcha_point_form_id = $form_state['values']['captcha_point_form_id'];
  $captcha_type = $form_state['values']['captcha_type'];
  captcha_set_form_id_setting($captcha_point_form_id, $captcha_type);
  backdrop_set_message(t('Saved CAPTCHA point settings.'), 'status');
}

/**
 * Confirm dialog for disabling/deleting a CAPTCHA point.
 */
function captcha_point_disable_confirm($form, &$form_state, $captcha_point_form_id, $delete) {
  $form = array();
  $form['captcha_point_form_id'] = array(
    '#type' => 'value',
    '#value' => $captcha_point_form_id,
  );
  $form['captcha_point_delete'] = array(
    '#type' => 'value',
    '#value' => $delete,
  );
  if ($delete) {
    $message = t('Are you sure you want to delete the CAPTCHA for form_id %form_id?', array('%form_id' => $captcha_point_form_id));
    $yes = t('Delete');
  }
  else {
    $message = t('Are you sure you want to disable the CAPTCHA for form_id %form_id?', array('%form_id' => $captcha_point_form_id));
    $yes = t('Disable');
  }
  return confirm_form($form, $message, 'admin/config/people/captcha/captcha', '', $yes);
}

/**
 * Submission handler of CAPTCHA point disabling/deleting confirm_form.
 */
function captcha_point_disable_confirm_submit($form, &$form_state) {
  $captcha_point_form_id = $form_state['values']['captcha_point_form_id'];
  $delete = $form_state['values']['captcha_point_delete'];
  if ($delete) {
    captcha_set_form_id_setting($captcha_point_form_id, NULL);
    backdrop_set_message(t('Deleted CAPTCHA for form %form_id.', array('%form_id' => $captcha_point_form_id)));
  } else {
    captcha_set_form_id_setting($captcha_point_form_id, 'none');
    backdrop_set_message(t('Disabled CAPTCHA for form %form_id.', array('%form_id' => $captcha_point_form_id)));
  }
  $form_state['redirect'] = 'admin/config/people/captcha/captcha';
}

/**
 * Helper function for generating an example challenge.
 */
function _captcha_generate_example_challenge($module, $type) {
  return array(
    '#type' => 'captcha',
    '#captcha_type' => $module . '/' . $type,
    '#captcha_admin_mode' => TRUE,
  );
}

/**
 * Funtion for generating a page with CAPTCHA examples.
 *
 * If the arguments $module and $challenge are not set, generate a list with
 * examples of the available CAPTCHA types.
 * If $module and $challenge are set, generate 10 examples of the concerning
 * CAPTCHA.
 */
function captcha_examples($form, $form_state, $module, $challenge) {
  module_load_include('inc', 'captcha');

  $form = array();
  if ($module && $challenge) {
    // Generate 10 example challenges.
    for ($i = 0; $i < 10; $i++) {
      $form["challenge_{$i}"] = _captcha_generate_example_challenge($module, $challenge);
    }
  }
  else {
    // Generate a list with examples of the available CAPTCHA types.
    $form['info'] = array(
      '#markup' => t('This page gives an overview of all available challenge types, generated with their current settings.'),
    );
    foreach (module_implements('captcha') as $mkey => $module) {
      $challenges = call_user_func_array($module . '_captcha', array('list'));
      if ($challenges) {
        foreach ($challenges as $ckey => $challenge) {
          $form["captcha_{$mkey}_{$ckey}"] = array(
            '#type' => 'fieldset',
            '#title' => t('Challenge %challenge by module %module', array('%challenge' => $challenge, '%module' => $module)),
            'challenge' => _captcha_generate_example_challenge($module, $challenge),
            'more_examples' => array(
              '#markup' => l(t('10 more examples of this challenge.'), "admin/config/people/captcha/captcha/examples/$module/$challenge"),
            ),
          );
        }
      }
    }
  }
  return $form;
}
