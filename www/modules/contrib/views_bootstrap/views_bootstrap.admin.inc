<?php

/**
 * @file
 * Contains the administration pages for views_bootstrap.
 *
 */
function views_bootstrap_settings($form_state) {
  $form = array();
  $form['bootstrap_assets'] = array(
    '#type' => 'fieldset',
    '#title' => t('Bootstrap Assets'),
    '#description' => t('Bootstrap version this setting provides: %version', array('%version' => VIEWS_BOOTSTRAP_ASSETS_VERSION)),
    'include_bootstrap_assets' => array(
      '#type' => 'checkbox',
      '#title' => t('Include Bootstrap CSS & JS when used by Views'),
      '#default_value' => config_get('views_bootstrap.settings', 'include_bootstrap_assets'),
      '#description' => t('If your site does not use a Bootstrap based theme, then you will need to select this option for this module to work correctly. If your site theme includes Bootstrap assets, then this setting should be disabled (unchecked).'),
    ),
  );
  $form['actions']['#type'] = 'actions';
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save configuration'),
  );
  return $form;
}

/**
 * Submit handler for module_settings_form().
 */
function views_bootstrap_settings_submit($form, &$form_state) {
    $config = config('views_bootstrap.settings');
    foreach ($form_state['values'] as $key => $value)
    {
      $config->set($key, $value);
    }

    $config->save();
    backdrop_set_message(t('The configuration options have been saved.'));
}
