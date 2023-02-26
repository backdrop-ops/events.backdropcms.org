<?php
/**
 * @file
 * Admin functions for borg events modile.
 */
function borg_events_custom_settings_form($form = array(), &$form_settings = array()) {
  $form['#config'] = 'borg_events_custom.settings';

  $form['event_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Borg event status'),
    '#description' => t('These settings are also available while editing the main event block on the home page.'),
    '#collapsible' => FALSE,
  );

  _borg_events_add_event_settings($form['event_settings'], FALSE);

  $heading = t('Update time slot dates');
  $help  = '<p>' . t('All <em>Time Slot</em> taxonomy terms will need to be updated with the correct date before each event. The button below can be used to update all terms at once.') . '</p>';
  $form['time_slot_dates'] = array(
    '#type' => 'fieldset',
    '#title' => $heading,
    '#description' => $help,
    '#collapsible' => TRUE,
  );

  $term_example = taxonomy_term_load(4);
  // <strong>Slot 1</strong>: [utz-datetime:2022-03-11 8:00 PST|g:i a e - F j, Y]
  $desc_example = $term_example->description;
  $date_example = $term_example->field_schedule_date1[LANGUAGE_NONE][0]['value'];
  $ymd = substr($date_example, 0, 10);
  $time = substr($date_example, 11);

  $form['time_slot_dates']['event_next_start_date'] = array(
    '#type' => 'textfield',
    '#title' => t('Next event start date'),
    '#disabled' => TRUE,
    '#size' => 20,
  );

  $config = config('borg_events_custom.settings');
  $current_event = (int) $config->get('event_nid');
  $next_event_info = _borg_events_get_event_info($current_event);
  if ($next_event_info) {
    $form['time_slot_dates']['event_next_start_date']['#default_value'] = $next_event_info['start_date_ymd'];
    $form['time_slot_dates']['event_next_start_date']['#description'] = t('This date is pulled from the event: :title (!edit)', array(
      ':title' => $next_event_info['title'],
      '!edit' => $next_event_info['edit'],
    ));
  }

  $form['time_slot_dates']['event_next_start_date'] = array(
    '#type' => 'textfield',
    '#title' => t('Next event start date'),
    '#default_value' => $next_event_info['start_date_ymd'],
    '#disabled' => TRUE,
    '#size' => 20,
  );

  _borg_events_add_event_source_description($form['time_slot_dates']['event_next_start_date'],$next_event_info);

  $term_example_edit = l(t('edit time slot'), 'taxonomy/term/' . $term_example->tid . '/edit');
  $form['time_slot_dates']['event_next_start_time'] = array(
    '#type' => 'textfield',
    '#title' => t('Next event start time'),
    '#default_value' => $time,
    '#description' => t('This time is pulled from the time slot: :title (!edit)', array(
      ':title' => check_plain($term_example->name),
      '!edit' => $term_example_edit,
    )),
    '#disabled' => TRUE,
    '#size' => 20,
  );

  $form['time_slot_dates']['time_slot_button'] = array(
    '#type' => 'submit',
    '#value' => $heading,
    '#submit' => array('_borg_events_update_time_slots'),
  );

  $new_date = str_replace($ymd, $next_event_info['start_date_ymd'], $date_example);
  $new_description = str_replace('2022-03-11', $next_event_info['start_date_ymd'], $desc_example);

  $example  = t('<strong>Slot 1 example:</strong><br>');
  $example .= t('<strong>Date:</strong> <code>:example</code><br>', array(':example' => $new_date ));
  $example .= t('<strong>Description:</strong> <code>:example</code>', array(':example' => $new_description));
  $form['time_slot_dates']['time_slot_example'] = array(
    '#type' => 'markup',
    '#markup' => '<p>' .  $example . '</p>',
  );

  return system_settings_form($form);
}

/**
 * Submit handler for the update terms button.
 */
function _borg_events_update_time_slots(&$form, &$form_state) {
  $hour_count = 1;

  // Get the starting date and time.
  $nid = $form_state['values']['event_nid'];
  $start_date = $form_state['values']['event_next_start_date'];
  $start_date_ymd = $start_date;
  $start_date_ymd_array = explode('-', $start_date_ymd);
  $start_date_day = array_pop($start_date_ymd_array);

  // @todo
  //$start_time = $form_state['values']['event_next_start_time'];


  // Get all term ids for the vocab.
  $tids = db_query("SELECT tid FROM {taxonomy_term_data} WHERE vocabulary = 'time_slot' ORDER BY weight")->fetchCol();
  foreach ($tids as $tid) {
    $term = taxonomy_term_load($tid);

    if ($tid == 21) {
      $new_day = $start_date_day + 1;
      $start_date_ymd = str_replace($start_date_day, $new_day, $start_date_ymd);
    }
  
    $old_date = $term->field_schedule_date1[LANGUAGE_NONE][0]['value'];
    $new_date = _borg_event_update_term_date($start_date_ymd, $old_date);
    $term->field_schedule_date1[LANGUAGE_NONE][0]['value'] = $new_date;

    //dpm($term->description);
    $old_description = $term->description;
    $new_description = _borg_event_update_term_description($start_date_ymd, $old_description);
    $term->description = $new_description;

    $term->save();

    $hour_count++;
  }
  backdrop_set_message(t(':count time slots have been updated.', array(':count' => $hour_count)));
}

function _borg_event_update_term_date($new_date_ymd, $old_date) {
  $old_ymd = substr($old_date, 0, 10);
  //$old_time = substr($date_example, 11);
  $new_date = str_replace($old_ymd, $new_date_ymd, $old_date);
  
  return $new_date;
}

function _borg_event_update_term_description($new_date_ymd, $old_description) {
  // <strong>Slot 1</strong>: [utz-datetime:2022-03-11 8:00 PST|g:i a e - F j, Y]
  $old_description_array = explode('utz-datetime:', $old_description);
  $old_desc_date = array_pop($old_description_array);
  $old_desc_date_ymd = substr($old_desc_date, 0, 10);
  $new_description = str_replace($old_desc_date_ymd, $new_date_ymd, $old_description);

  // @todo update time.

  return $new_description;
}

/**
 * Submit handler for Event Settings form. 
 */
function borg_events_custom_settings_form_submit(&$form, &$form_state) {
  // Save old event_nid when changed.
  _borg_events_save_old_event($form_state['values']['event_nid']);
}
