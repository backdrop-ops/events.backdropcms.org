<?php

/**
 * @file
 * Contains various relationship handlers.
 */

/**
 * Base class for all our relationship classes.
 *
 * @ingroup views
 */
abstract class flag_handler_relationship extends views_handler_relationship {

  /**
   * Every relationship has a 'flag' option.
   */
  function option_definition() {
    $options = parent::option_definition();
    $options['flag'] = array('default' => NULL);
    $options['required'] = array('default' => 1);
    return $options;
  }

  /**
   * Make sure the flag exists.
   *
   * When importing views, or when deleting flags, inconsistent views may
   * result. This validator is called by Views before saving or previewing a
   * view.
   */
  function validate() {
    $errors = array();
    $tokens = array(
      '@relationship-name' => $this->ui_name() . ' ' . $this->admin_summary(),
      '@flag-name' => $this->options['flag'],
    );
    if (!$this->options['flag']) {
      $errors[] = t('You must pick a flag to use for the relationship "@relationship-name".', $tokens);
    }
    elseif (!flag_get_flag($this->options['flag'])) {
      $errors[] = t('This view is looking for a flag by the name "@flag-name", but there is no such flag. Perhaps it was deleted. Please update the relationship "@relationship-name" in this view to use an existing flag.', $tokens);
    }
    return $errors;
  }

  /**
   * Get the type of the flag this relationship uses.
   *
   * This looks at the data set in the relationship definition in Views data.
   *
   * @return
   *  The flag's type, e.g., 'node' or 'taxonomy_term', or NULL if this is not
   *  set in data from hook_views_data().
   *
   * @see flag_views_data_alter()
   */
  function get_flag_type() {
    return isset($this->definition['flag type']) ? $this->definition['flag type'] : NULL;
  }

  /**
   * Returns the flag object.
   */
  function get_flag() {

    // Backward compatibility: There may exist old views on the system whose
    // 'flag' option isn't set. (This happens if the admin had skippped
    // clicking the 'Update' button.) When run, these views should behave as
    // if the first flag was selected.
    if (!isset($this->options['flag'])) {
      $this->options['flag'] = flag_views_flag_default($this->get_flag_type());
    }

    // Validation: Since validate() is called only when in Views's
    // administrative UI, we need to do validation at "run time" ourselves.
    if (($errors = $this->validate())) {
      foreach ($errors as $error) {
        backdrop_set_message($error, 'error');
      }
    }

    return flag_get_flag($this->options['flag']);
  }

  // @todo: It's logical that this class should also implement options_form(),
  // to show the flag selector, and query(), to filter on the flag.
}

/**
 * Views relationship handler associating flags and content.
 *
 * This forms a bridge from the entity table to the {flagging} table, with
 * options to restrict the join to only flagged content, and to flagged content
 * by the current user.
 *
 * @ingroup views
 */
class flag_handler_relationship_content extends flag_handler_relationship {

  function option_definition() {
    $options = parent::option_definition();
    $options['user_scope'] = array('default' => 'current');
    return $options;
  }

  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    $entity_type = $this->definition['flag type'];
    $form['label']['#description'] .= ' ' . t('The name of the selected flag makes a good label.');
    $form['flag'] = flag_views_flag_config_form('radios', $entity_type, $this->options['flag']);

    $form['user_scope'] = array(
      '#type' => 'radios',
      '#title' => t('By'),
      '#options' => array('current' => t('Current user'), 'any' => t('Any user')),
      '#default_value' => $this->options['user_scope'],
    );

    $form['required']['#title'] = t('Include only flagged content');
    $form['required']['#description'] = t('If checked, only content that has this flag will be included. Leave unchecked to include all content; or, in combination with the <em>Flagged</em> filter, <a href="@unflagged-url">to limit the results to specifically unflagged content</a>.', array('@unflagged-url' => 'http://drupal.org/node/299335'));

    if (!$form['flag']['#options']) {
      $form = array(
        'error' => array(
          '#markup' => '<p class="error form-item">' . t('No %type flags exist. You must first <a href="@create-url">create a %type flag</a> before being able to use this relationship type.', array('%type' => $entity_type, '@create-url' => url(FLAG_ADMIN_PATH))) . '</p>',
        ),
      );
      $form_state['no flags exist'] = TRUE;
    }
    if (module_exists('session_api')) {
      $form['session_warning'] = array(
        '#markup' => '<p class="warning form-item">' . t('<strong>Warning</strong>: Adding this relationship for any flag that contains <strong>anonymous flagging access</strong> will disable page caching for anonymous users when this view is executed. (But this is only true when the relationship is constrained to "Current user", not to "Any user".) It is recommended to create a dedicated page for views containing anonymous user data.') . '</p>',
      );
    }
  }

  function options_validate(&$form, &$form_state) {
    if (!empty($form_state['no flags exist'])) {
      form_error($form, t('You must first create a flag'));
    }
  }

  function admin_summary() {
    return $this->options['user_scope'] == 'current' ? t('by current user') : t('by any user');
  }

  function ui_name($short = FALSE) {
    // We put the bookmark name in the UI string to save space.
    return t('!group: !title', array('!group' => $this->definition['group'], '!title' => empty($this->options['flag']) ? t('(Please select a flag)') : $this->options['flag']));
  }

  /**
   * Called to implement a relationship in a query.
   */
  function query() {
    if (!($flag = $this->get_flag())) {
      return;
    }

    $this->definition['extra'][] = array(
      'field' => 'flag_name',
      'value' => $flag->name,
      'numeric' => FALSE,
    );
    if ($this->options['user_scope'] == 'current' && !$flag->global) {
      $this->definition['extra'][] = array(
        'field' => 'uid',
        'value' => '***CURRENT_USER***',
        'numeric' => TRUE,
      );
      $flag_roles = user_roles(FALSE, "flag $flag->name");
      if (isset($flag_roles[BACKDROP_ANONYMOUS_ROLE])) {
        // Disable page caching for anonymous users.
        backdrop_page_is_cacheable(FALSE);

        // Add in the SID from Session API for anonymous users.
        $this->definition['extra'][] = array(
          'field' => 'sid',
          'value' => '***FLAG_CURRENT_USER_SID***',
          'numeric' => TRUE,
        );
      }
    }
    parent::query();
  }
}

/**
 * Views relationship handler associating flag counts and content.
 *
 * This forms a bridge from the entity table to the {flag_counts} table, with
 * the option to restrict the join to include only flagged content.
 *
 * @ingroup views
 */
class flag_handler_relationship_counts extends flag_handler_relationship {

  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    $entity_type = $this->definition['flag type'];
    $form['flag'] = flag_views_flag_config_form('radios', $entity_type, $this->options['flag']);

    $form['required']['#title'] = t('Include only flagged content');
    $form['required']['#description'] = t('If checked, only content that is flagged will be included.');
  }

  function admin_summary() {
    // Nothing to show.
  }

  function ui_name($short = FALSE) {
    // We put the bookmark name in the UI string to save space.
    return t('!group: !title counter', array('!group' => $this->definition['group'], '!title' => $this->options['flag']));
  }

  /**
   * Called to implement a relationship in a query.
   */
  function query() {
    if (!($flag = $this->get_flag())) {
      return;
    }

    $this->definition['extra'][] = array(
      'field' => 'flag_name',
      'value' => $flag->name,
      'numeric' => FALSE,
    );
    if (!empty($this->options['required'])) {
      // Unfortunately, we may have zeros in our table, so having
      // parent::query() do INNER JOIN doesn't suffice. We need to filter these
      // zeros out.
      // @todo Make sure zero records aren't written in the first place, and
      // remove this code.
      $this->definition['extra'][] = array(
        'field' => 'count',
        'operator' => '>',
        'value' => '0',
        'numeric' => TRUE,
      );
    }
    parent::query();
  }
}

/**
 * Views relationship handler associating flags and users.
 *
 * This forms a bridge from the the {users} table to the {flagging} table.
 *
 * @ingroup views
 */
class flag_handler_relationship_user_content extends flag_handler_relationship {

  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    $form['label']['#description'] .= ' ' . t('Including name of the selected flag helps identify this relationship.');

    $form['flag'] = flag_views_flag_config_form('radios', NULL, $this->options['flag']);
    $form['flag']['#title'] = t('Flagged');

    $form['required']['#title'] = t('Include only users who have flagged content.');
    $form['required']['#description'] = t('If checked, only users that have flagged any content with this flag will be included.');
  }

  function admin_summary() {
    return $this->options['flag'];
  }

  /**
   * Called to implement a relationship in a query.
   */
  function query() {
    if (!($flag = $this->get_flag())) {
      return;
    }

    $this->definition['extra'][] = array(
      'field' => 'flag_name',
      'value' => $flag->name,
      'numeric' => FALSE,
    );
    parent::query();
  }
}
