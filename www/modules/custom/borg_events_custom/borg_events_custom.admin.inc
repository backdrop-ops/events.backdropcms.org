<?php
/**
 * @file
 * Admin functions for borg events modile.
 */
function borg_events_custom_settings_form($form = array(), &$form_settings = array()) {
  $form['#config'] = 'borg_events_custom.settings';

  $config = config('borg_events_custom.settings');
  $event_path = 'node/' . $config->get('event_nid');
  $event_edit_path = $event_path . '/edit';

  $form['#attached']['css'] = array(
    backdrop_get_path('module', 'borg_events_custom') . '/css/borg_events_admin.css',
  );

  $form['howto_open_event'] = array(
    '#type' => 'fieldset',
    '#title' => t('How to open registration for Backdrop LIVE'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $open_regisration1 = array(
    'Remove "tentative" from node title',
    'Set <strong>Registration type</strong> to "Backdrop Live"',
    '<strong>Save</strong> the event',
  );
  $open_regisration2 = array(
    'Select the <strong>Manage registrations</strong> tab',
    'Select the <strong>Settings</strong> sub-tab',
    'Review the <strong>Scheduling</strong> fieldset',
    'Set the <strong>Confirmation</strong> message to <code>You are registered for Backdrop LIVE. We ask for $20, but any amount is appreciated.</code>',
    'Under <strong>Additional settings</strong> set <strong>Spaces allowed</strong> to 200',
    'Under <strong>Additional settings</strong> set <strong>Confirmation redirect path</strong> to <code>donate</code>',
    '<strong>Save settings</strong> for event registration',
  );

  $update_site = array(
    'Visit this Backdrop Event settings page at <code>admin/config/event</code>',
    'Under <strong>Borg event status</strong> change the answer to <strong>Has Registration been opened</strong> from "no" to "yes"',
  );
  $replacements = array('!url' => url($event_edit_path));
  $markup = '<p>' . t('First, edit the <a href="!url">current event node</a> as follows:', $replacements) . '</p>';
  $markup .= theme('item_list', array('items' => $open_regisration1, 'type' => 'ol'));
  $markup .= '<p>' . t('Next, edit the <a href="!url">current event node</a> again:', $replacements) . '</p>';
  $markup .= theme('item_list', array('items' => $open_regisration2, 'type' => 'ol'));
  $markup .= '<p>' . t('Then, tell the website that registration is open:') . '</p>';
  $markup .= theme('item_list', array('items' => $update_site, 'type' => 'ol'));
  $form['howto_open_event']['steps'] = array(
    '#type' => 'help',
    '#markup' => $markup,
  );

  $form['event_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Borg event status'),
    '#description' => t('These settings are also available while editing the main event block on the home page.'),
    '#collapsible' => FALSE,
  );

  _borg_events_add_event_settings($form['event_settings'], FALSE);

  $more_nid_desc = t('Changing the <em>event Node ID</em> will do the following:');
  // @see _borg_events_change_current_event() in .module file.
  $nid_changes = array(
    'Update the schedule view to reference the current event',
    'Update the title for the default schedule view display',
    'Update the default value for the node-referene field on Discussions',
    'Update the default value for the node-referene field on Presentations',
  );
  $form['event_settings']['event_nid']['#description'] = $more_nid_desc . theme('item_list', array('items' => $nid_changes));

  $more_reg_desc = t('Changing the <em>Has Registration been opened?</em> setting to YES will do the following:');
  // @see _borg_events_change_current_event() in .module file.
  $more_reg_changes = array(
    'Add a "Register Now" button on the home page hero block',
    'Add the "Next Steps" block into the sidebar',
  );
  $form['event_settings']['event_open']['#description'] = $more_reg_desc . theme('item_list', array('items' => $more_reg_changes));

  $more_now_desc = t('Changing the <em>happening now</em> setting will do the following:');
  $now_changes = array(
    'Enable the schuedle display on the home page',
    'Set the home page block to block A',
    'Enable the schedule menu link',
    'Make all the Zoom Links public',
  );
  $form['event_settings']['event_now'] = array(
    '#type' => 'radios',
    '#title' => t('Is the event happening now?'),
    '#description' => $more_now_desc . theme('item_list', array('items' => $now_changes)),
    '#options' => array(
      '0' => t('No'),
      '1' => t('Yes'),
    ),
    '#default_value' => $config->get('event_now'),
  );

  /** Time Slots Fieldset *****************************************************/

  $heading = t('Update time slot dates');
  $help  = '<p>' . t('All <em>Time Slot</em> taxonomy terms will need to be updated with the correct date before each event. The button below can be used to update all terms at once.') . '</p>';
  $form['time_slot_dates'] = array(
    '#type' => 'fieldset',
    '#title' => $heading,
    '#description' => $help,
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
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

  $current_event = (int) $config->get('event_nid');
  $next_event_info = _borg_events_get_event_info($current_event);
  if ($next_event_info) {
    $form['time_slot_dates']['event_next_start_date']['#default_value'] = $next_event_info['start_date_ymd'];
    $form['time_slot_dates']['event_next_start_date']['#description'] = t('This date is pulled from the event: :title (!edit)', array(
      ':title' => $next_event_info['title'],
      '!edit' => $next_event_info['edit'],
    ));
  }

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
    '#disabled' => empty($next_event_info),
    '#submit' => array('_borg_events_update_time_slots'),
  );

  if (!empty($next_event_info)) {
    $new_date = str_replace($ymd, $next_event_info['start_date_ymd'], $date_example);
    $new_description = str_replace('2022-03-11', $next_event_info['start_date_ymd'], $desc_example);

    $example  = t('<strong>Slot 1 example:</strong><br>');
    $example .= t('<strong>Date:</strong> <code>:example</code><br>', array(':example' => $new_date ));
    $example .= t('<strong>Description:</strong> <code>:example</code>', array(':example' => $new_description));
    $form['time_slot_dates']['time_slot_example'] = array(
      '#type' => 'markup',
      '#markup' => '<p>' .  $example . '</p>',
    );
  }

  /** Discussion nodes Fieldset ***********************************************/

  $dn_heading = t('Create discussion nodes');
  $dn_help  = '<p>' . t('All <em>Time Slot</em> taxonomy terms will need at least one discussion node to build the schedule. The button below can be used to generate all nodes at once.') . '</p>';
  $form['discussion_nodes'] = array(
    '#type' => 'fieldset',
    '#title' => $dn_heading,
    '#description' => $dn_help,
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $form['discussion_nodes']['discussion_nodes_button'] = array(
    '#type' => 'submit',
    '#value' => t('Create 2 discussions per time slot'),
    '#submit' => array('_borg_events_generate_discussion_nodes'),
  );

  if (!module_exists('devel')) {
    $form['discussion_nodes']['#description'] .= ' <strong>' . t('The <em>Devel</em> Module is requrired.') . '</strong>';
    $form['discussion_nodes']['discussion_nodes_button']['#disabled'] = TRUE;
  }

  /** Notes Link Fieldset *****************************************************/

  $nl_heading = t('Add shared notes link');
  $nl_help  = '<p>' . t('All <em>Discussion</em> nodes should have a link to a shared notes document for each event. The button below can be used to update all disucssions for this event, at once.') . '</p>';
  $form['notes_link'] = array(
    '#type' => 'fieldset',
    '#title' => $nl_heading,
    '#description' => $nl_help,
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,

  );

  $form['notes_link']['notes_link_url'] = array(
    '#type' => 'url',
    '#placeholder' => t('https://events.backdropcms.org/notes-008'),
    '#description' => t('A shared notes link can be generated by creating a <a href="@redirect_url">redirect</a> on events.backdropcms.org.', array('@redirect_url' => 'https://events.backdropcms.org/admin/config/urls/redirect/add')),
  );

  $form['notes_link']['notes_link_button'] = array(
    '#type' => 'submit',
    '#value' => $nl_heading,
    '#submit' => array('_borg_events_update_notes_link'),
  );

  $form['event_close'] = array(
    '#type' => 'fieldset',
    '#title' => t('How to end a successful Backdrop Live event'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $update_site = array(
    'Visit this Backdrop Event settings page at <code>admin/config/event</code>',
    'Under <strong>Borg event status</strong> change the answer to <strong>Has Registration been opened</strong> from "yes" to "no".',
  );
  $close_registration = array(
    'Select <strong>Manage registrations</strong> tab',
    'Select <strong>Settings</strong> sub-tab',
    'Un-check the <strong>Enable</strong> checkbox at the very top of the page.',
    'Save settings',
  );
  $create_next = array(
    'Create the next Event node at <code>node/add/event</code>',
    'Use the dates decided at the closing session for this Backdrop LIVE',
    'Add the word "tentative" at the end of the node title.',
    'Save the event node',
    'Edit the event node, and note the Node ID',
  );
  $stage_next = array(
    'Return to this Backdrop Event settings page at <code>admin/config/event</code>',
    'Under <strong>Borg event status</strong> change the <strong>Event Node ID </strong> to the one you just created',
  );
  $markup = '<p>' . t('Stop promoting the current event.') . '</p>';
  $markup .= theme('item_list', array('items' => $update_site, 'type' => 'ol'));
  $markup .= '<p>' . t('Shut off regisration by editing the current event node.') . '</p>';
  $markup .= theme('item_list', array('items' => $close_registration, 'type' => 'ol'));
  $markup .= '<p>' . t('Create the next Backdrop Live event node:') . '</p>';
  $markup .= theme('item_list', array('items' => $create_next, 'type' => 'ol'));
  $markup .= '<p>' . t('Update the website to promote the next event:') . '</p>';
  $markup .= theme('item_list', array('items' => $stage_next, 'type' => 'ol'));
  $form['event_close']['steps'] = array(
    '#type' => 'help',
    '#markup' => $markup,
  );

  // @todo - why is this necessary?
  $form['#submit'][] = 'borg_events_custom_settings_form_submit';

  return system_settings_form($form);
}

/**
 * Submit handler for Event Settings form.
 */
function borg_events_custom_settings_form_submit(&$form, &$form_state) {
  // Change the current event. @see borg_events_custom.module
  _borg_events_change_current_event($form_state['values']['event_nid']);

  _borg_events_change_happening_now($form_state['values']['event_now']);
}

/**
 * Submit handler changing the happening now stuff.
 */
function _borg_events_change_happening_now($event_now) {

  if ($event_now) {
    // Enable the schuedle display on the home page.
    config_set('layout.layout.home', 'content.4041d660-481e-4608-a40b-11a4749caf06.data.status', 1);
    layout_reset_caches();
    // Set the home page block to block A
    config_set('views.view.schedule', 'display.block_1.display_options.filters.field_time_block_tid.value', array(40 => 40));
    // Enable the schedule menu link
    db_query("update {menu_links} SET hidden = 0 where mlid = 489");
    // Make all the Zoom Links visible
    config_set('field.instance.node.session.field_zoom', 'display.default.type', 'link_default');
    config_set('field.instance.node.session.field_zoom', 'display.teaser.type', 'link_default');
    cache_clear_all();
  }
  else {
    // Disable the schuedle display on the home page.
    config_set('layout.layout.home', 'content.4041d660-481e-4608-a40b-11a4749caf06.data.status', 0);
    layout_reset_caches();
    // Disable the schedule menu link
    db_query("update {menu_links} SET hidden = 1 where mlid = 489");
    // Make all the Zoom Links hidden
    config_set('field.instance.node.session.field_zoom', 'display.default.type', 'hidden');
    config_set('field.instance.node.session.field_zoom', 'display.teaser.type', 'hidden');
    cache_clear_all();
  }
}

/**
 * Submit handler for the shared notes link update.
 */
function _borg_events_update_notes_link(&$form, &$form_state) {
  // Get the intended URL.
  $url = trim($form_state['values']['notes_link_url']);

  // Get the current event.
  $config = config('borg_events_custom.settings');
  $current_event = (int) $config->get('event_nid');

  // Get all the discussion node IDs for this event.
  $nl_query = "SELECT entity_id FROM {field_data_field_event}
    WHERE entity_type = 'node'
      AND bundle = 'session'
      AND language = :lang
      AND delta = '0'
      AND field_event_target_id = :event
    ORDER BY entity_id";

  $nids = db_query($nl_query, array(':lang' => LANGUAGE_NONE,':event' => $current_event))->fetchCol();
  foreach ($nids as $nid) {
    $node = node_load($nid);
    $node->field_notes[LANGUAGE_NONE][0]['url'] = $url;
    $node->save();
  }

  $node_count = count($nids);
  backdrop_set_message(t(':count discussion nodes have been updated.', array(':count' => $node_count)));
}

/**
 * Submit handler for the generate discussion nodes button.
 */
function _borg_events_generate_discussion_nodes(&$form, &$form_state) {
  if (module_exists('devel')) {
    // Get all term ids for the vocab.
    $tids = db_query("SELECT tid FROM {taxonomy_term_data} WHERE vocabulary = 'time_slot' ORDER BY weight")->fetchCol();
    foreach ($tids as $tid) {
      $term = taxonomy_term_load($tid);

      module_load_include('inc', 'devel_generate', 'devel_generate');
      $results = array(
        'node_types' => array('session'),
        'users' => array('1'),
        'title' => 'TBD',
      );
      _borg_events_generate_content_add_node($tid, 38);
      _borg_events_generate_content_add_node($tid, 39);
    }

    $node_count = count($tids) * 2;
    backdrop_set_message(t(':count discussion nodes have been created.', array(':count' => $node_count)));
  }
  else {
    backdrop_set_message(t('Please enable the Devel module.'));
  }
}

/**
 * Create one node. Used by both batch and non-batch.
 *
 * @param $tid
 *   Taxonomy Term ID for the current time slot.
 */
function _borg_events_generate_content_add_node($time_slot_tid, $room_tid) {
  $node = new Node();
  $node->nid = NULL;
  $node->type = 'session';
  node_object_prepare($node);
  $node->title = '<open>';
  $node->uid = 1; // Drop

  // Set to the current event.
  $config = config('borg_events_custom.settings');
  $current_event = (int) $config->get('event_nid');
  $node->field_event[LANGUAGE_NONE]['0']['target_id'] = $current_event;

  // Set time slot to the current time slot.
  $node->field_schedule_slot[LANGUAGE_NONE]['0']['tid'] = $time_slot_tid;

  // Set room to the current room.
  $node->field_room[LANGUAGE_NONE]['0']['tid'] = $room_tid;

  // Set the body of this node.
  $node->body[LANGUAGE_NONE]['0']['value'] = '';

  $node->sticky = 0;
  $node->revision = 0;
  $node->promote = 0;

  node_save($node);
}

/**
 * Submit handler for the update terms button.
 *
 * Note: this does not change times on any Time Slots, only changes the dates.
 */
function _borg_events_update_time_slots(&$form, &$form_state) {
  // Get the starting date from the event node.
  $nid = $form_state['values']['event_nid'];
  $start_date = $form_state['values']['event_next_start_date'];

  // Create a universal date we can use to update all time slots.
  $date_object = new DateTime();
  $date = $date_object->createFromFormat('Y-m-d', $start_date);

  // Set the time from the time in the form (which comes from the first term).
  $start_time = $form_state['values']['event_next_start_time'];
  $start_time_units = explode(':', $start_time);
  $date->setTime($start_time_units[0], $start_time_units[1], '00', '00');

  // Get all term ids for the Time Slot vocab ordered by weight.
  $query = "SELECT tid FROM {taxonomy_term_data} WHERE vocabulary = 'time_slot' ORDER BY weight";
  $tids = db_query($query)->fetchCol();
  foreach ($tids as $tid) {
    $term = taxonomy_term_load($tid);

    if ($tid == 21) {
      // On the term "Block D - Slot 10", we update the day by 1.
      $date->modify('+1 day');
    }

    // Extract the old date and time from the time slot.
    $old_date = $term->field_schedule_date1[LANGUAGE_NONE][0]['value'];
    // Update the universal date to the new day and old time.
    $new_datetime = _borg_event_update_term_date($date, $old_date);
    // Format the new date to a string for the db.
    $new_date = $new_datetime->format('Y-m-d H:i:s');
    // Update the field value for date.
    $term->field_schedule_date1[LANGUAGE_NONE][0]['value'] = $new_date;

    //dpm($term->name . ' ' . $tid . ': old: ' . $old_date . ' new: ' . $new_date);

    // Update the field value for description.
    $old_description = $term->description;
    $new_description = _borg_event_update_term_description($new_datetime, $old_description);
    $term->description = $new_description;

    //dpm($term->name . ' ' . $tid . ': old: ' . $old_description . ' new: ' . $new_description);

    $term->save();

    // Always increment time by 1 hour.
      // @TODO: instead of using the old time - set the time based on universal value.
    //$date->modify('+1 hour');

  }

  $count = count($tids);
  backdrop_set_message(t(':count time slots have been updated.', array(':count' => $count)));
}

/**
 * Helper function: Sets the Date to the universal date, but adds the old time.
 *   @TODO: Use the universal date's time instead.
 *
 * @param DateTime $new_datetime
 * @param string $old_date
 *
 * @return DateTime $datetime
 *   The new date to use for updates,
 */
function _borg_event_update_term_date($new_datetime, $old_date) {
  // Create another temporary date object.
  $date_object = new DateTime();
  // Set the temporary Date object to the previous date AND TIME.
  $datetime = $date_object->createFromFormat('Y-m-d H:i:s', $old_date);
  if ($datetime) {
    // Assemble the new universal date for day, and the previous time.
    $parts = array(
      'Y' => $new_datetime->format('Y'),
      'm' => $new_datetime->format('m'),
      'd' => $new_datetime->format('d'),
      'H' => $datetime->format('H'),
      'i' => $datetime->format('i'),
      's' => $datetime->format('s'),
      'u' => $datetime->format('u'),
    );
    // Set the date to the new date.
    $datetime->setDate($parts['Y'], $parts['m'], $parts['d']);
    // Set the time to the old time.
    $datetime->setTime($parts['H'], $parts['i'], $parts['s'], $parts['u']);

    // Return the new day + old time.
    return $datetime;
  }
  else {
    backdrop_set_message('Unable to create datetime object for ' . $old_date);
  }
}

/**
 * Helper function: update the term description.
 *
 * @param DateTime $new_date
 * @param string $old_description
 *
 * @return string $new_description
 */
function _borg_event_update_term_description($new_date, $old_description) {
  // Get the parts of the token that don't change.
  $token_front = '[utz-datetime:';
  $token_back = '|g:i a e - F j, Y]';

  // Get the description without the token. Description:
  // <strong>Slot 1</strong>: [utz-datetime:2022-03-11 8:00 PST|g:i a e - F j, Y]
  $old_description_array = explode(':', $old_description);
  $old_description_front = array_shift($old_description_array) . ': ';

  // Get the date into the format the token expects.
  $new_date_formatted = $new_date->format('Y-m-d G:i T');

  // Put the token back in the new description.
  $new_description = $old_description_front . $token_front . $new_date_formatted . $token_back;

  return $new_description;
}
