<?php

/**
 * @file
 * Hooks provided by the Field Group module.
 *
 * Fieldgroup is a module that will wrap fields and other fieldgroups. Nothing more, nothing less.
 * For this there are formatters we can create on forms and view modes.
 *
 * Some of the elements defined in fieldgroup will be ported to the elements module.
 *
 * DEVELOPERS NOTES
 *
 * - Fieldgroup uses a ''#fieldgroups' property to know what fieldgroups are to be pre_rendered and
 *   rendered by the field_group module. This means we need to be sure our groups are in #fieldgroups.
 *   #fieldgroups is later merged with the normal #groups that can be used by any other module.
 *   This is done to be sure fieldgroup is not taking fieldsets from profile2, commerce line items,
 *   commerce user profiles, ... .
 *   When trying to merge a programmatically created field wrapper (div, markup, fieldset, ...) into
 *   groups, you might consider adding it in #field_groups as well if you want the element processed
 *   by fieldgroup.
 */

/**
 * @addtogroup hooks
 * @{
 */


/**
 * Javascript hooks
 *
 * Backdrop.FieldGroup.Effects.processHook.execute()
 * See field_group.js for the examples for all implemented formatters.
 */


/**
 * Implements hook_field_group_formatter_info().
 *
 * Define the information on formatters. The formatters are
 * separated by view mode type. We have "form" for all form elements
 * and "display" will be the real view modes (full, teaser, sticky, ...)
 *
 * structure:
 * @code
 * array(
 *   'form' => array(
 *     'fieldset' => array(
 *       // required, String with the name of the formatter type.
 *       'label' => t('Fieldset'),
 *       // optional, String description of the formatter type.
 *       'description' => t('This is field group that ...'),
 *       // required, Array of available formatter options.
 *       'format_types' => array('open', 'collapsible', 'collapsed'),
 *       // required, String with default value of the style.
 *       'default_formatter' => 'collapsible',
 *       // optional, Array with key => default_value pairs.
 *       'instance_settings' => array('key' => 'value'),
 *     ),
 *   ),
 *   'display' => array(
 *     'fieldset' => array(
 *       // required, String with the name of the formatter type.
 *       'label' => t('Fieldset'),
 *       // optional, String description of the formatter type.
 *       'description' => t('This is field group that ...'),
 *       // required, Array of available formatter options.
 *       'format_types' => array('open', 'collapsible', 'collapsed'),
 *       // required, String with default value of the style.
 *       'default_formatter' => 'collapsible',
 *       // optional, Array with key => default_value pairs.
 *       'instance_settings' => array('key' => 'value'),
 *     ),
 *   ),
 * ),
 * @endcode
 *
 * @return array
 *   A collection of available formatting html controls for form
 *   and display overview type.
 *
 * @see field_group_field_group_formatter_info()
 */
function hook_field_group_formatter_info() {
  return array(
    'form' => array(
      'fieldset' => array(
        'label' => t('Fieldset'),
        'description' => t('This fieldgroup renders the inner content in a fieldset with the title as legend.'),
        'format_types' => array('open', 'collapsible', 'collapsed'),
        'instance_settings' => array('classes' => ''),
        'default_formatter' => 'collapsible',
      ),
    ),
    'display' => array(
      'div' => array(
        'label' => t('Div'),
        'description' => t('This fieldgroup renders the inner content in a simple div with the title as legend.'),
        'format_types' => array('open', 'collapsible', 'collapsed'),
        'instance_settings' => array('effect' => 'none', 'speed' => 'fast', 'classes' => ''),
        'default_formatter' => 'collapsible',
      ),
    ),
  );
}

/**
 * Implements hook_field_group_format_settings().
 *
 * Defines configuration widget for the settings on a field group
 * formatter. Each formatter can have different elements and storage.
 *
 * @param object $group
 *   The Group definition.
 * @return array
 *   The form element for the format settings.
 */
function hook_field_group_format_settings($group) {
  // Add a wrapper for extra settings to use by others.
  $form = array(
    'instance_settings' => array(
      '#tree' => TRUE,
      '#weight' => 2,
    ),
  );

  $field_group_types = field_group_formatter_info();
  $mode = $group->mode == 'form' ? 'form' : 'display';
  $formatter = $field_group_types[$mode][$group->format_type];

  // Add the required formatter type selector.
  if (isset($formatter['format_types'])) {
    $form['formatter'] = array(
      '#title' => t('Fieldgroup settings'),
      '#type' => 'select',
      '#options' => backdrop_map_assoc($formatter['format_types']),
      '#default_value' => isset($group->format_settings['formatter']) ? $group->format_settings['formatter'] : $formatter['default_formatter'],
      '#weight' => 1,
    );
  }
  if ($mode == 'form') {
    $form['instance_settings']['required_fields'] = array(
      '#type' => 'checkbox',
      '#title' => t('Mark group for required fields.'),
      '#default_value' => isset($group->format_settings['instance_settings']['required_fields']) ? $group->format_settings['instance_settings']['required_fields'] : (isset($formatter['instance_settings']['required_fields']) ? $formatter['instance_settings']['required_fields'] : ''),
      '#weight' => 2,
    );
  }
  $form['instance_settings']['classes'] = array(
    '#title' => t('Extra CSS classes'),
    '#type' => 'textfield',
    '#default_value' => isset($group->format_settings['instance_settings']['classes']) ? $group->format_settings['instance_settings']['classes'] : (isset($formatter['instance_settings']['classes']) ? $formatter['instance_settings']['classes'] : ''),
    '#weight' => 3,
    '#element_validate' => array('field_group_validate_css_class'),
  );
  $form['instance_settings']['description'] = array(
    '#title' => t('Description'),
    '#type' => 'textarea',
    '#default_value' => isset($group->format_settings['instance_settings']['description']) ? $group->format_settings['instance_settings']['description'] : (isset($formatter['instance_settings']['description']) ? $formatter['instance_settings']['description'] : ''),
    '#weight' => 0,
  );

  // Add optional instance_settings.
  switch ($group->format_type) {
    case 'div':
      $form['instance_settings']['effect'] = array(
        '#title' => t('Effect'),
        '#type' => 'select',
        '#options' => array('none' => t('None'), 'blind' => t('Blind')),
        '#default_value' => isset($group->format_settings['instance_settings']['effect']) ? $group->format_settings['instance_settings']['effect'] : $formatter['instance_settings']['effect'],
        '#weight' => 2,
      );
      $form['instance_settings']['speed'] = array(
        '#title' => t('Speed'),
        '#type' => 'select',
        '#options' => array('none' => t('None'), 'slow' => t('Slow'), 'fast' => t('Fast')),
        '#default_value' => isset($group->format_settings['instance_settings']['speed']) ? $group->format_settings['instance_settings']['speed'] : $formatter['instance_settings']['speed'],
        '#weight' => 3,
      );
      break;
    case 'fieldset':
      $form['instance_settings']['classes'] = array(
        '#title' => t('Extra CSS classes'),
        '#type' => 'textfield',
        '#default_value' => isset($group->format_settings['instance_settings']['classes']) ? $group->format_settings['instance_settings']['classes'] : $formatter['instance_settings']['classes'],
        '#weight' => 3,
        '#element_validate' => array('field_group_validate_css_class'),
      );
      break;
    case 'tabs':
    case 'htabs':
    case 'accordion':
      unset($form['instance_settings']['description']);
      if (isset($form['instance_settings']['required_fields'])) {
        unset($form['instance_settings']['required_fields']);
      }
      break;
    case 'tab':
    case 'htab':
    case 'accordion-item':
    default:
  }

  return $form;
}

/**
 * Implements hook_field_group_pre_render().
 *
 * This function gives you the opportunity to create the given
 * wrapper element that can contain the fields.
 * In the example beneath, some variables are prepared and used when building the
 * actual wrapper element. All elements in Backdrop FAPI can be used.
 *
 * Note that at this point, the field group has no notion of the fields in it.
 *
 * There is also an alternative way of handling this. The default implementation
 * within field_group calls "field_group_pre_render_<format_type>".
 * @see field_group_pre_render_fieldset.
 *
 * @param array $elements
 *   Elements by address.
 * @param object $group
 *   The field group info.
 */
function hook_field_group_pre_render(&$element, $group, &$form) {

  // You can prepare some variables to use in the logic.
  $view_mode = isset($form['#view_mode']) ? $form['#view_mode'] : 'form';
  $id = $form['#entity_type'] . '_' . $form['#bundle'] . '_' . $view_mode . '_' . $group->group_name;

  $classes = '';
  // Each formatter type can have whole different set of element properties.
  switch ($group->format_type) {

    // Normal or collapsible div.
    case 'div':
      $effect = isset($group->format_settings['instance_settings']['effect']) ? $group->format_settings['instance_settings']['effect'] : 'none';
      $speed = isset($group->format_settings['instance_settings']['speed']) ? $group->format_settings['instance_settings']['speed'] : 'none';
      $add = array(
        '#type' => 'markup',
        '#weight' => $group->weight,
        '#id' => $id,
      );
      $classes .= " speed-$speed effect-$effect";
      if ($group->format_settings['formatter'] != 'open') {
        $add['#prefix'] = '<div class="field-group-format ' . $classes . '">
          <span class="field-group-format-toggler">' . check_plain(t($group->label)) . '</span>
          <div class="field-group-format-wrapper" style="display: none;">';
        $add['#suffix'] = '</div></div>';
      }
      else {
        $add['#prefix'] = '<div class="field-group-format ' . $group->group_name . ' ' . $classes . '">';
        $add['#suffix'] = '</div>';
      }
      if (!empty($description)) {
        $add['#prefix'] .= '<div class="description">' . $description . '</div>';
      }
      $element += $add;

      if ($effect == 'blind') {
        backdrop_add_library('system', 'effects.blind');
      }

      break;
    break;
  }
}

/**
 * Implements hook_field_group_pre_render().
 *
 * Last resort to alter the pre_render build.
 */
function hook_field_group_pre_render_alter(&$element, $group, &$form) {

  if ($group->format_type == 'htab') {
    $element['#theme_wrappers'] = array('my_horizontal_tab');
  }

}

/**
 * Implements hook_field_group_build_pre_render_alter().
 *
 * Last resort where you can alter things. It is expected that when you need
 * this function, you have most likely a very custom case or it is a fix that
 * can be put in field_group core.
 *
 * @param array $elements
 *   Elements by address.
 */
function hook_field_group_build_pre_render_alter(&$element) {

  // Prepare variables.
  $display = isset($element['#view_mode']);
  $groups = array_keys($element['#groups']);

  // Example from field_group itself to unset empty elements.
  if ($display) {
    field_group_remove_empty_display_groups($element, $groups);
  }

  // You might include additional javascript files and stylesheets.
  $element['#attached']['js'][] = backdrop_get_path('module', 'field_group') . '/js/field_group.js';
  $element['#attached']['css'][] = backdrop_get_path('module', 'field_group') . '/css/field_group.css';

}

/**
 * Implements hook_field_group_format_summary().
 *
 * Place to override or change default summary behavior. In most
 * cases the implementation of field group itself will be enough.
 *
 * TODO It might be better to change this hook with already created summaries,
 * giving the ability to alter or add it later on.
 *
 * @param object $group
 *   Group definition.
 *
 * @return string
 *   HTML of the format summary.
 */
function hook_field_group_format_summary($group) {
  $output = '';
  // Create additional summary or change the default setting.
  return $output;
}

/**
 * Alter the field group definitions provided by other modules.
 *
 * @param array $groups
 *   Reference to an array of field group definition objects.
 */
function hook_field_group_info_alter(&$groups) {
  if (!empty($groups['group_issue_metadata|node|project_issue|form'])) {
    $groups['group_issue_metadata|node|project_issue|form']->data['children'][] = 'taxonomy_vocabulary_9';
  }
}

/**
 * Implements hook_field_group_update_field_group().
 *
 * @param object $group
 *   The Field Group definition.
 */
function hook_field_group_update_field_group($group) {
  // Update data depending on the group.
}

/**
 * Implements hook_field_group_delete_field_group().
 *
 * @param object $group
 *   The Field Group definition.
 */
function hook_field_group_delete_field_group($group) {
  // Delete extra data depending on the group.
}

/**
 * Implements hook_field_group_create_field_group().
 *
 * @param object $group
 *   The Field Group definition.
 */
function hook_field_group_create_field_group($group) {
  // Create extra data depending on the group.
}

/**
 * Implements hook_field_group_format_settings_alter().
 *
 * @param array $form
 *   An associative array containing the structure of the form, which is passed
 *   by reference.
 * @param object $group
 *   The Group definition.
 */
function hook_field_group_format_settings_alter(&$form, &$group) {
  // Alter the group format settings that appear in the summary and form.
}

/**
 * Implements hook_field_group_field_ui_parent_requirements_alter().
 *
 * @param array $parent_requirements
 *   An associative array keyed by group type of a group parent.
 * @param array $context
 *   Array of the form structure and display overview.
 */
function hook_field_group_field_ui_parent_requirements_alter(&$parent_requirements, &$context) {
  // Alter the parent requirements array used to display warning if a container
  // has not been set up.
}

/**
 * Implements hook_field_group_html_classes_alter().
 *
 * @param object $classes
 *   An object of required and optional classes.
 * @param object $group
 *   The Group definition.
 */
function hook_field_group_html_classes_alter(&$classes, &$group) {
  // Alter the required or optional classes on a field group.
}


/**
 * @} End of "addtogroup hooks".
 */



/**
 * @addtogroup utility functions
 * @{
 */

/**
 * Get the groups for a given entity type, bundle and view mode.
 *
 * @param String $entity_type
 *   The Entity type where field groups are requested.
 * @param String $bundle
 *   The entity bundle for the field groups.
 * @param String $view_mode
 *   The view mode scope for the field groups.
 *
 * @see field_group_read_groups()
 */
function field_group_info_groups($entity_type = NULL, $bundle = NULL, $view_mode = NULL, $reset = FALSE) {
  // This function caches the result and delegates to field_group_read_groups.
}

/**
 * Get the groups for the given parameters, uncached.
 *
 * @param Array $params
 *   The Entity type where field groups are requested.
 * @param $enabled
 *   Return enabled or disabled groups.*
 *
 * @see field_group_info_groups()
 */
function field_group_read_groups($conditions = array()) {
  // This function loads the requested groups
}

/**
 * Hides field groups including children in a render array.
 *
 * @param array $element
 *   A render array. Can be a form, node, user, ...
 * @param array $group_names
 *   An array of field group names that should be hidden.
 */
function field_group_hide_field_groups(&$element, $group_names) {
}

/**
 * @} End of "addtogroup utility functions".
 */

